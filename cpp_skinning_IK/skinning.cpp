#include "skinning.h"
#include "vec3d.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <fstream>
#include <Eigen/Dense>
#include <Eigen/Geometry>
using namespace std;

// CSCI 520 Computer Animation and Simulation
// Jernej Barbic and Yijing Li

Skinning::Skinning(int numMeshVertices, const double * restMeshVertexPositions,
    const std::string & meshSkinningWeightsFilename)
{
  this->numMeshVertices = numMeshVertices;
  this->restMeshVertexPositions = restMeshVertexPositions;

  cout << "Loading skinning weights..." << endl;
  ifstream fin(meshSkinningWeightsFilename.c_str());
  assert(fin);
  int numWeightMatrixRows = 0, numWeightMatrixCols = 0;
  fin >> numWeightMatrixRows >> numWeightMatrixCols;
  assert(fin.fail() == false);
  assert(numWeightMatrixRows == numMeshVertices);
  int numJoints = numWeightMatrixCols;

  vector<vector<int>> weightMatrixColumnIndices(numWeightMatrixRows);
  vector<vector<double>> weightMatrixEntries(numWeightMatrixRows);
  fin >> ws;
  while(fin.eof() == false)
  {
    int rowID = 0, colID = 0;
    double w = 0.0;
    fin >> rowID >> colID >> w;
    weightMatrixColumnIndices[rowID].push_back(colID);
    weightMatrixEntries[rowID].push_back(w);
    assert(fin.fail() == false);
    fin >> ws;
  }
  fin.close();

  // Build skinning joints and weights.
  numJointsInfluencingEachVertex = 0;
  for (int i = 0; i < numMeshVertices; i++)
    numJointsInfluencingEachVertex = std::max(numJointsInfluencingEachVertex, (int)weightMatrixEntries[i].size());
  assert(numJointsInfluencingEachVertex >= 2);

  // Copy skinning weights from SparseMatrix into meshSkinningJoints and meshSkinningWeights.
  meshSkinningJoints.assign(numJointsInfluencingEachVertex * numMeshVertices, 0);
  meshSkinningWeights.assign(numJointsInfluencingEachVertex * numMeshVertices, 0.0);
  for (int vtxID = 0; vtxID < numMeshVertices; vtxID++)
  {
    vector<pair<double, int>> sortBuffer(numJointsInfluencingEachVertex);
    for (size_t j = 0; j < weightMatrixEntries[vtxID].size(); j++)
    {
      int frameID = weightMatrixColumnIndices[vtxID][j];
      double weight = weightMatrixEntries[vtxID][j];
      sortBuffer[j] = make_pair(weight, frameID);
    }
    sortBuffer.resize(weightMatrixEntries[vtxID].size());
    assert(sortBuffer.size() > 0);
    sort(sortBuffer.rbegin(), sortBuffer.rend()); // sort in descending order using reverse_iterators
    for(size_t i = 0; i < sortBuffer.size(); i++)
    {
      meshSkinningJoints[vtxID * numJointsInfluencingEachVertex + i] = sortBuffer[i].second;
      meshSkinningWeights[vtxID * numJointsInfluencingEachVertex + i] = sortBuffer[i].first;
    }

    // Note: When the number of joints used on this vertex is smaller than numJointsInfluencingEachVertex,
    // the remaining empty entries are initialized to zero due to vector::assign(XX, 0.0) .
  }
}

void Skinning::applySkinning(const RigidTransform4d * jointSkinTransforms, double * newMeshVertexPositions) const
{
   /* Linear Blend Skinning (LBS) */
   if (skinMethodUsed == LBS) {
      Vec4d restPos; // rest vertex positions
      Vec4d newPos; // new vertex positions
      for (int vtxID = 0; vtxID < numMeshVertices; vtxID++) {
         // Store the rest vertex position and the new vertex position into Vec4d
         restPos.set(restMeshVertexPositions[3 * vtxID + 0], restMeshVertexPositions[3 * vtxID + 1], restMeshVertexPositions[3 * vtxID + 2], 1.0);
         newPos.set(0.0);
         // Execute linear blend skinning to compute the new vertex position
         for (int i = 0; i < numJointsInfluencingEachVertex; i++) {
            newPos += meshSkinningWeights[vtxID * numJointsInfluencingEachVertex + i] *
               (jointSkinTransforms[meshSkinningJoints[vtxID * numJointsInfluencingEachVertex + i]] * restPos);
         }
         newMeshVertexPositions[3 * vtxID + 0] = newPos[0];
         newMeshVertexPositions[3 * vtxID + 1] = newPos[1];
         newMeshVertexPositions[3 * vtxID + 2] = newPos[2];
      }
   }
   /* Dual-Quaternion Skinning (DQS) */
   else {
      for (int vtxID = 0; vtxID < numMeshVertices; vtxID++) {
         // Convert the rest position of the current mesh vertex into an Eigen vector
         Eigen::Vector3d restPos(restMeshVertexPositions[3 * vtxID + 0], restMeshVertexPositions[3 * vtxID + 1], restMeshVertexPositions[3 * vtxID + 2]);
         
         Eigen::Quaterniond realSum(0.0, 0.0, 0.0, 0.0); // real part of the blend result
         Eigen::Quaterniond dualSum(0.0, 0.0, 0.0, 0.0); // dual part of the blend result
         
         int idx;
         for (int i = 0; i < numJointsInfluencingEachVertex; i++) {
            idx = vtxID * numJointsInfluencingEachVertex + i;
            // Get rotation associated with each joint
            Mat3d jointRot = jointSkinTransforms[meshSkinningJoints[idx]].getRotation();
            // Get translation associated with each joint
            Vec3d jointTranslation = jointSkinTransforms[meshSkinningJoints[idx]].getTranslation();
            // Construct real part of the dual quaternion
            Eigen::Matrix3d eigenMatRot;
            for (int rowID = 0; rowID < 3; rowID++) {
               for (int colID = 0; colID < 3; colID++) {
                  eigenMatRot(rowID, colID) = jointRot[rowID][colID];
               }
            }
            Eigen::Quaterniond realPart(eigenMatRot);
            // Ensure quaternions are in the same hemisphere
            if (realSum.dot(realPart) < 0) {
               realPart.coeffs() *= -1.0;
            }
            realPart.normalize();
            // Construct dual part of the dual quaternion
            Eigen::Quaterniond trans(0.0, jointTranslation[0], jointTranslation[1], jointTranslation[2]);
            Eigen::Quaterniond dualPart = trans * realPart;

            // Blending
            realSum.coeffs() += realPart.coeffs() * meshSkinningWeights[idx];
            dualSum.coeffs() += 0.5 * dualPart.coeffs() * meshSkinningWeights[idx]; 
         }

         // Normalize the blend result
         double realNorm = realSum.norm();
         realSum.normalize();
         dualSum.coeffs() = dualSum.coeffs() / realNorm - realSum.dot(dualSum) / realNorm * realSum.coeffs();
       
         // Update mesh vertex positions
         Eigen::Vector3d resultTranslation = 2.0 * (dualSum * realSum.conjugate()).vec();
         Eigen::Vector3d newPos = realSum._transformVector(restPos) + resultTranslation;
        
         for (int i = 0; i < 3; i++) {
            newMeshVertexPositions[3 * vtxID + i] = newPos[i];
         }
      }
   }
}


