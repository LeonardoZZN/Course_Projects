#include "IK.h"
#include "FK.h"
#include "minivectorTemplate.h"
#include <Eigen/Dense>
#include <adolc/adolc.h>
#include <cassert>
#if defined(_WIN32) || defined(WIN32)
  #ifndef _USE_MATH_DEFINES
    #define _USE_MATH_DEFINES
  #endif
#endif
#include <math.h>
using namespace std;

// CSCI 520 Computer Animation and Simulation
// Jernej Barbic and Yijing Li

namespace
{

// Converts degrees to radians.
template<typename real>
inline real deg2rad(real deg) { return deg * M_PI / 180.0; }

template<typename real>
Mat3<real> Euler2Rotation(const real angle[3], RotateOrder order)
{
  Mat3<real> RX = Mat3<real>::getElementRotationMatrix(0, deg2rad(angle[0]));
  Mat3<real> RY = Mat3<real>::getElementRotationMatrix(1, deg2rad(angle[1]));
  Mat3<real> RZ = Mat3<real>::getElementRotationMatrix(2, deg2rad(angle[2]));

  switch(order)
  {
    case RotateOrder::XYZ:
      return RZ * RY * RX;
    case RotateOrder::YZX:
      return RX * RZ * RY;
    case RotateOrder::ZXY:
      return RY * RX * RZ;
    case RotateOrder::XZY:
      return RY * RZ * RX;
    case RotateOrder::YXZ:
      return RZ * RX * RY;
    case RotateOrder::ZYX:
      return RX * RY * RZ;
  }
  assert(0);
}

// Performs forward kinematics, using the provided "fk" class.
// This is the function whose Jacobian matrix will be computed using adolc.
// numIKJoints and IKJointIDs specify which joints serve as handles for IK:
//   IKJointIDs is an array of integers of length "numIKJoints"
// Input: numIKJoints, IKJointIDs, fk, eulerAngles (of all joints)
// Output: handlePositions (world-coordinate positions of all the IK joints; length is 3 * numIKJoints)
template<typename real>
void forwardKinematicsFunction(
    int numIKJoints, const int * IKJointIDs, const FK & fk,
    const std::vector<real> & eulerAngles, std::vector<real> & handlePositions)
{
  // Students should implement this.
  // The implementation of this function is very similar to function computeLocalAndGlobalTransforms in the FK class.
  // The recommended approach is to first implement FK::computeLocalAndGlobalTransforms.
  // Then, implement the same algorithm into this function. To do so,
  // you can use fk.getJointUpdateOrder(), fk.getJointRestTranslation(), and fk.getJointRotateOrder() functions.
  // Also useful is the multiplyAffineTransform4ds function in minivectorTemplate.h .
  // It would be in principle possible to unify this "forwardKinematicsFunction" and FK::computeLocalAndGlobalTransforms(),
  // so that code is only written once. We considered this; but it is actually not easily doable.
  // If you find a good approach, feel free to document it in the README file, for extra credit.


   int numJoints = fk.getNumJoints();
   int jointID;
   real rotAngles[3], orientAngles[3], restTrans[3];
   Mat3<real> orientRot, localRot; // orientation of the local frame from the parent frame; rotation in the local frame
   Vec3<real> localTransVec; 
   vector<Mat3<real>> globalRot(numJoints);
   vector<Vec3<real>> globalTrans(numJoints);

   for (int i = 0; i < numJoints; i++) {
      jointID = fk.getJointUpdateOrder(i);
      // Get orientation Euler angles
      orientAngles[0] = fk.getJointOrient(jointID)[0];
      orientAngles[1] = fk.getJointOrient(jointID)[1];
      orientAngles[2] = fk.getJointOrient(jointID)[2];
      // Compute the orientation matrix
      orientRot = Euler2Rotation(orientAngles, RotateOrder::XYZ);
      // Get rotation Euler angles in the local frame
      rotAngles[0] = eulerAngles[jointID * 3 + 0];
      rotAngles[1] = eulerAngles[jointID * 3 + 1];
      rotAngles[2] = eulerAngles[jointID * 3 + 2];
      // Compute the rotation matrix in the local frame
      localRot = Euler2Rotation(rotAngles, fk.getJointRotateOrder(jointID));
      // Get the local translations
      restTrans[0] = fk.getJointRestTranslation(jointID)[0];
      restTrans[1] = fk.getJointRestTranslation(jointID)[1];
      restTrans[2] = fk.getJointRestTranslation(jointID)[2];

      // Compute the global transformation matrix of the current joint
      int parentID = fk.getJointParent(jointID);
      if (parentID < 0) {
         globalRot[jointID] = orientRot * localRot;
         globalTrans[jointID].set(restTrans); 
      }
      else {
         localTransVec.set(restTrans);
         multiplyAffineTransform4ds(globalRot[parentID],    // parent R_global
                                    globalTrans[parentID],  // parent t_global
                                    orientRot * localRot,   // R_local
                                    localTransVec,          // t_local
                                    globalRot[jointID],     // R_global
                                    globalTrans[jointID]);  // t_global
      }
   }

   // Output handle positions
   for (int i = 0; i < numIKJoints; i++) {
      jointID = IKJointIDs[i];
      handlePositions[i * 3 + 0] = globalTrans[jointID][0];
      handlePositions[i * 3 + 1] = globalTrans[jointID][1];
      handlePositions[i * 3 + 2] = globalTrans[jointID][2];
   }
  
}

} // end anonymous namespaces

IK::IK(int numIKJoints, const int * IKJointIDs, FK * inputFK, int adolc_tagID)
{
  this->numIKJoints = numIKJoints;
  this->IKJointIDs = IKJointIDs;
  this->fk = inputFK;
  this->adolc_tagID = adolc_tagID;

  FKInputDim = fk->getNumJoints() * 3;
  FKOutputDim = numIKJoints * 3;

  train_adolc();
}

void IK::train_adolc()
{
  // Students should implement this.
  // Here, you should setup adol_c:
  //   Define adol_c inputs and outputs. 
  //   Use the "forwardKinematicsFunction" as the function that will be computed by adol_c.
  //   This will later make it possible for you to compute the gradient of this function in IK::doIK
  //   (in other words, compute the "Jacobian matrix" J).
  // See ADOLCExample.cpp .

   trace_on(adolc_tagID);
   vector<adouble> eulerAngles(FKInputDim); // define the input of the function
   for (int i = 0; i < FKInputDim; i++) {
      eulerAngles[i] <<= 0.0;
   }

   vector<adouble> handlePos(FKOutputDim); // define the output of the function
   forwardKinematicsFunction<adouble>(numIKJoints, IKJointIDs, *fk, eulerAngles, handlePos);  
   
   vector<double> output(FKOutputDim);
   for (int i = 0; i < FKOutputDim; i++) {
      handlePos[i] >>= output[i];
   }
   trace_off();
}

void IK::doIK(const Vec3d* targetHandlePositions, Vec3d* jointEulerAngles)
{
  // Students should implement this.
  // Use adolc to evalute the forwardKinematicsFunction and its gradient (Jacobian). It was trained in train_adolc().
  // Specifically, use ::function, and ::jacobian .
  // See ADOLCExample.cpp .
  //
  // Use it implement the Tikhonov IK method (or the pseudoinverse method for extra credit).
  // Note that at entry, "jointEulerAngles" contains the input Euler angles. 
  // Upon exit, jointEulerAngles should contain the new Euler angles.

   int numJoints = fk->getNumJoints(); 

   vector<double> currHandlePositions(FKOutputDim); // output of FK
   vector<double> jacobianMatrix(FKOutputDim * FKInputDim);
   vector<double*> jacobianMatrixRows(FKOutputDim); 
   // Initialize jacobianMatrixRows
   for (int i = 0; i < FKOutputDim; i++) {
      jacobianMatrixRows[i] = &jacobianMatrix[i * FKInputDim];
   }
   // Compute the current handle positions
   ::function(adolc_tagID, FKOutputDim, FKInputDim, jointEulerAngles->data(), currHandlePositions.data());
   // Evaluate the Jacobian matrix 
   ::jacobian(adolc_tagID, FKOutputDim, FKInputDim, jointEulerAngles->data(), jacobianMatrixRows.data());

   Eigen::MatrixXd J(FKOutputDim, FKInputDim);
   Eigen::MatrixXd J_T(FKInputDim, FKOutputDim);
   Eigen::MatrixXd I(FKInputDim, FKInputDim); 
   I = Eigen::MatrixXd::Identity(FKInputDim, FKInputDim);
   for (int rowID = 0; rowID < FKOutputDim; rowID++) {
      for (int colID = 0; colID < FKInputDim; colID++) {
         J(rowID, colID) = jacobianMatrix[rowID * FKInputDim + colID];
      }
   }
   J_T = J.transpose();

   /* Tikhonov Regularization */
   if (IKMethodUsed == Tikhonov) {
      double alpha = 0.006; // alpha of Tikhonov regularization

      // Compute LHS matrix: (J^T)J + alpha*I
      Eigen::MatrixXd LHS(FKInputDim, FKInputDim);
      LHS = J_T * J + alpha * I;
      // Compute RHS: (J^T)*(deltaPos)
      Eigen::VectorXd deltaPos(FKOutputDim), RHS(FKInputDim);
      for (int i = 0; i < numIKJoints; i++) {
         deltaPos(i * 3 + 0) = targetHandlePositions[i][0] - currHandlePositions[i * 3 + 0];
         deltaPos(i * 3 + 1) = targetHandlePositions[i][1] - currHandlePositions[i * 3 + 1];
         deltaPos(i * 3 + 2) = targetHandlePositions[i][2] - currHandlePositions[i * 3 + 2];
      }
      RHS = J_T * deltaPos;
      // Use ldlt() function in Eigen library to solve for changes in joint Euler angles
      Eigen::VectorXd changeInAngles = LHS.ldlt().solve(RHS);

      // Update joint Euler angles
      for (int i = 0; i < numJoints; i++) {
         jointEulerAngles[i] += Vec3d(changeInAngles(i * 3 + 0), changeInAngles(i * 3 + 1), changeInAngles(i * 3 + 2));
      }
   }
   /* Pseudoinverse (least squares) */
   else if (IKMethodUsed == Pseudoinverse) { 
      /* deltaTheta = J_dagger * deltaPos, where J_dagger = J_T * (J * J_T) ^ (-1) */
      // Compute the pseudoinverse of the Jacobian matrix
      Eigen::MatrixXd J_dagger = J_T * (J * J_T).inverse();;
      // Compute deltaPos
      Eigen::VectorXd deltaPos(FKOutputDim);
      for (int i = 0; i < numIKJoints; i++) {
         deltaPos(i * 3 + 0) = targetHandlePositions[i][0] - currHandlePositions[i * 3 + 0];
         deltaPos(i * 3 + 1) = targetHandlePositions[i][1] - currHandlePositions[i * 3 + 1];
         deltaPos(i * 3 + 2) = targetHandlePositions[i][2] - currHandlePositions[i * 3 + 2];
      }
      // Compute changes in joint Euler angles
      Eigen::VectorXd changeInAngles = J_dagger * deltaPos;

      // Update joint Euler angles
      for (int i = 0; i < numJoints; i++) {
         jointEulerAngles[i] += Vec3d(changeInAngles(i * 3 + 0), changeInAngles(i * 3 + 1), changeInAngles(i * 3 + 2));
      }
   }
}

