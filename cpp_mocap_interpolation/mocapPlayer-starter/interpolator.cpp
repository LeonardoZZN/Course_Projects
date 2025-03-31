#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <iostream>
#include "motion.h"
#include "interpolator.h"
#include "types.h"
#include "transform.h"
#include "performanceCounter.h"

/* Gets the computation time of different interpolation techniques */
PerformanceCounter pCounter;

Interpolator::Interpolator()
{
  //Set default interpolation type
  m_InterpolationType = LINEAR;

  //set default angle representation to use for interpolation
  m_AngleRepresentation = EULER;
}

Interpolator::~Interpolator()
{
}

//Create interpolated motion
void Interpolator::Interpolate(Motion * pInputMotion, Motion ** pOutputMotion, int N) 
{
  //Allocate new motion
  *pOutputMotion = new Motion(pInputMotion->GetNumFrames(), pInputMotion->GetSkeleton()); 

  //Perform the interpolation
  if ((m_InterpolationType == LINEAR) && (m_AngleRepresentation == EULER))
    LinearInterpolationEuler(pInputMotion, *pOutputMotion, N);
  else if ((m_InterpolationType == LINEAR) && (m_AngleRepresentation == QUATERNION))
    LinearInterpolationQuaternion(pInputMotion, *pOutputMotion, N);
  else if ((m_InterpolationType == BEZIER) && (m_AngleRepresentation == EULER))
    BezierInterpolationEuler(pInputMotion, *pOutputMotion, N);
  else if ((m_InterpolationType == BEZIER) && (m_AngleRepresentation == QUATERNION))
    BezierInterpolationQuaternion(pInputMotion, *pOutputMotion, N);
  else
  {
    printf("Error: unknown interpolation / angle representation type.\n");
    exit(1);
  }
}

void Interpolator::LinearInterpolationEuler(Motion * pInputMotion, Motion * pOutputMotion, int N)
{
  int inputLength = pInputMotion->GetNumFrames(); // frames are indexed 0, ..., inputLength-1

  int startKeyframe = 0;

  pCounter.StartCounter();

  while (startKeyframe + N + 1 < inputLength)
  {
    int endKeyframe = startKeyframe + N + 1;

    Posture * startPosture = pInputMotion->GetPosture(startKeyframe);
    Posture * endPosture = pInputMotion->GetPosture(endKeyframe);

    // copy start and end keyframe
    pOutputMotion->SetPosture(startKeyframe, *startPosture);
    pOutputMotion->SetPosture(endKeyframe, *endPosture);

    // interpolate in between
    for(int frame=1; frame<=N; frame++)
    {
      Posture interpolatedPosture;
      double t = 1.0 * frame / (N+1);

      // interpolate root position
      interpolatedPosture.root_pos = startPosture->root_pos * (1-t) + endPosture->root_pos * t;

      // interpolate bone rotations
      for (int bone = 0; bone < MAX_BONES_IN_ASF_FILE; bone++)
        interpolatedPosture.bone_rotation[bone] = startPosture->bone_rotation[bone] * (1-t) + endPosture->bone_rotation[bone] * t;

      pOutputMotion->SetPosture(startKeyframe + frame, interpolatedPosture);
    }

    startKeyframe = endKeyframe;
  }

  pCounter.StopCounter();
  std::cout << "Computation Time for Linear Euler Interpolation: " << pCounter.GetElapsedTime() << " sec" << std::endl;

  for (int frame = startKeyframe + 1; frame < inputLength; frame++) {
     pOutputMotion->SetPosture(frame, *(pInputMotion->GetPosture(frame)));
  }
}

void Interpolator::Rotation2Euler(double R[9], double angles[3])
{
  double cy = sqrt(R[0]*R[0] + R[3]*R[3]);

  if (cy > 16*DBL_EPSILON) 
  {
    angles[0] = atan2(R[7], R[8]);
    angles[1] = atan2(-R[6], cy);
    angles[2] = atan2(R[3], R[0]);
  } 
  else 
  {
    angles[0] = atan2(-R[5], R[4]);
    angles[1] = atan2(-R[6], cy);
    angles[2] = 0;
  }

  for(int i=0; i<3; i++)
    angles[i] *= 180 / M_PI;
}

void Interpolator::Euler2Rotation(double angles[3], double R[9])
{
   double rotX[4][4], rotY[4][4], rotZ[4][4], temp[4][4], output[4][4];
   // rotates around axis X
   rotationX(rotX, angles[0]);
   // rotates around axis Y
   rotationY(rotY, angles[1]);
   // rotates around axis Z
   rotationZ(rotZ, angles[2]);

   // combine rotations in XYZ order 
   matrix_mult(rotZ, rotY, temp);
   matrix_mult(temp, rotX, output);
   
   for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
         R[i * 3 + j] = output[i][j];
      }
   }
}

void Interpolator::BezierInterpolationEuler(Motion * pInputMotion, Motion * pOutputMotion, int N)
{
   int inputLength = pInputMotion->GetNumFrames(); // frames are indexed 0, ..., inputLength-1
   int startKeyframe = 0, endKeyframe, prevKeyframe, nextKeyframe;

   /* v1 and v2 below correspond to startKeyframe and endKeyframe respectively. */
   vector v0_r, v1_r, v2_r, v3_r; // keyframe root positions. 
   vector v0_b, v1_b, v2_b, v3_b; // keyframe bone rotations.
   /* a1, b2 correspond to a_N, and b_(N+1) respectively. a2 corresponds to a_(N+1)_bar. */
   vector a1_r, a2_r, b2_r; // spline control points of root positions.  
   vector a1_b, a2_b, b2_b; // spline control points of bone rotations. 

   pCounter.StartCounter();
   while (startKeyframe + N + 1 < inputLength) {
      endKeyframe = startKeyframe + N + 1;
      prevKeyframe = startKeyframe - N - 1;
      nextKeyframe = endKeyframe + N + 1;
      Posture* startPosture = pInputMotion->GetPosture(startKeyframe);
      Posture* endPosture = pInputMotion->GetPosture(endKeyframe);
      Posture* prevPosture;
      Posture* nextPosture;
      // copy start and end keyframe
      pOutputMotion->SetPosture(startKeyframe, *startPosture);
      pOutputMotion->SetPosture(endKeyframe, *endPosture);

      if (startKeyframe == 0) { /* first keyframe */
         nextPosture = pInputMotion->GetPosture(nextKeyframe);

         // control points of root positions
         v1_r = startPosture->root_pos;
         v2_r = endPosture->root_pos;
         v3_r = nextPosture->root_pos;
         a1_r = v1_r + (v3_r + (v2_r - v3_r) * 2.0 - v1_r) * 1.0 / 3;
         a2_r = v1_r + (v2_r - v1_r) * 2.0;
         a2_r = a2_r + (v3_r - a2_r) * 0.5; // a2_bar
         b2_r = v2_r - (a2_r - v2_r) * 1.0 / 3;   

         // interpolate in between
         for (int frame = 1; frame <= N; frame++)
         {
            Posture interpolatedPosture;
            double t = 1.0 * frame / (N + 1);

            // interpolate root position
            interpolatedPosture.root_pos = DeCasteljauEuler(t, v1_r, a1_r, b2_r, v2_r);

            // interpolate bone rotations
            for (int bone = 0; bone < MAX_BONES_IN_ASF_FILE; bone++) {
               // control points of bone rotations
               v1_b = startPosture->bone_rotation[bone];
               v2_b = endPosture->bone_rotation[bone];
               v3_b = nextPosture->bone_rotation[bone];
               a1_b = v1_b + (v3_b + (v2_b - v3_b) * 2.0 - v1_b) * 1.0 / 3;
               a2_b = v1_b + (v2_b - v1_b) * 2.0;
               a2_b = a2_b + (v3_b - a2_b) * 0.5; // a2_bar
               b2_b = v2_b - (a2_b - v2_b) * 1.0 / 3;
               interpolatedPosture.bone_rotation[bone] = DeCasteljauEuler(t, v1_b, a1_b, b2_b, v2_b);
            }
          
            pOutputMotion->SetPosture(startKeyframe + frame, interpolatedPosture);
         }
      }
      else if (nextKeyframe >= inputLength) { /* last keyframe */
         prevPosture = pInputMotion->GetPosture(prevKeyframe);

         // control points of root positions
         v0_r = prevPosture->root_pos;
         v1_r = startPosture->root_pos;
         v2_r = endPosture->root_pos;
         a1_r = v0_r + (v1_r - v0_r) * 2.0;
         a1_r = a1_r + (v2_r - a1_r) * 0.5; //a1_bar
         a1_r = v1_r + (a1_r - v1_r) * 1.0 / 3;
         b2_r = v2_r + (v0_r + (v1_r - v0_r) * 2.0 - v2_r) * 1.0 / 3;

         // interpolate in between
         for (int frame = 1; frame <= N; frame++)
         {
            Posture interpolatedPosture;
            double t = 1.0 * frame / (N + 1);

            // interpolate root position
            interpolatedPosture.root_pos = DeCasteljauEuler(t, v1_r, a1_r, b2_r, v2_r);

            // interpolate bone rotations
            for (int bone = 0; bone < MAX_BONES_IN_ASF_FILE; bone++) {
               // control points of bone rotations
               v0_b = prevPosture->bone_rotation[bone]; 
               v1_b = startPosture->bone_rotation[bone];
               v2_b = endPosture->bone_rotation[bone];
               a1_b = v0_b + (v1_b - v0_b) * 2.0;
               a1_b = a1_b + (v2_b - a1_b) * 0.5; //a1_bar
               a1_b = v1_b + (a1_b - v1_b) * 1.0 / 3;
               b2_b = v2_b + (v0_b + (v1_b - v0_b) * 2.0 - v2_b) * 1.0 / 3;
               interpolatedPosture.bone_rotation[bone] = DeCasteljauEuler(t, v1_b, a1_b, b2_b, v2_b);
            }

            pOutputMotion->SetPosture(startKeyframe + frame, interpolatedPosture);
         }
      }
      else {
         prevPosture = pInputMotion->GetPosture(prevKeyframe);
         nextPosture = pInputMotion->GetPosture(nextKeyframe);

         // control points of root positions
         v0_r = prevPosture->root_pos;
         v1_r = startPosture->root_pos;
         v2_r = endPosture->root_pos;
         v3_r = nextPosture->root_pos;
         a1_r = v0_r + (v1_r - v0_r) * 2.0;
         a1_r = a1_r + (v2_r - a1_r) * 0.5; //a1_bar
         a1_r = v1_r + (a1_r - v1_r) * 1.0 / 3;
         a2_r = v1_r + (v2_r - v1_r) * 2.0;
         a2_r = a2_r + (v3_r - a2_r) * 0.5; //a2_bar
         b2_r = v2_r - (a2_r - v2_r) * 1.0 / 3;

         // interpolate in between
         for (int frame = 1; frame <= N; frame++)
         {
            Posture interpolatedPosture;
            double t = 1.0 * frame / (N + 1);

            // interpolate root position
            interpolatedPosture.root_pos = DeCasteljauEuler(t, v1_r, a1_r, b2_r, v2_r);

            // interpolate bone rotations
            for (int bone = 0; bone < MAX_BONES_IN_ASF_FILE; bone++) {
               // control points of bone rotations
               v0_b = prevPosture->bone_rotation[bone];
               v1_b = startPosture->bone_rotation[bone];
               v2_b = endPosture->bone_rotation[bone];
               v3_b = nextPosture->bone_rotation[bone];
               a1_b = v0_b + (v1_b - v0_b) * 2.0;
               a1_b = a1_b + (v2_b - a1_b) * 0.5; //a1_bar
               a1_b = v1_b + (a1_b - v1_b) * 1.0 / 3;
               a2_b = v1_b + (v2_b - v1_b) * 2.0;
               a2_b = a2_b + (v3_b - a2_b) * 0.5; //a2_bar
               b2_b = v2_b - (a2_b - v2_b) * 1.0 / 3;
               interpolatedPosture.bone_rotation[bone] = DeCasteljauEuler(t, v1_b, a1_b, b2_b, v2_b);
            }

            pOutputMotion->SetPosture(startKeyframe + frame, interpolatedPosture);
         }
      }

      startKeyframe = endKeyframe;
   }

   pCounter.StopCounter();
   std::cout << "Computation Time for Bezier Euler Interpolation: " << pCounter.GetElapsedTime() << " sec" << std::endl;

   for (int frame = startKeyframe + 1; frame < inputLength; frame++) {
      pOutputMotion->SetPosture(frame, *(pInputMotion->GetPosture(frame)));
   }
}

void Interpolator::LinearInterpolationQuaternion(Motion * pInputMotion, Motion * pOutputMotion, int N)
{
   int inputLength = pInputMotion->GetNumFrames(); // frames are indexed 0, ..., inputLength-1

   int startKeyframe = 0;
   Quaternion<double> q_start, q_end, q_result;

   pCounter.StartCounter();
   while (startKeyframe + N + 1 < inputLength)
   {
      int endKeyframe = startKeyframe + N + 1;

      Posture* startPosture = pInputMotion->GetPosture(startKeyframe);
      Posture* endPosture = pInputMotion->GetPosture(endKeyframe);

      // copy start and end keyframe
      pOutputMotion->SetPosture(startKeyframe, *startPosture);
      pOutputMotion->SetPosture(endKeyframe, *endPosture);

      // interpolate in between
      for (int frame = 1; frame <= N; frame++)
      {
         Posture interpolatedPosture;
         double t = 1.0 * frame / (N + 1);

         // interpolate root position
         interpolatedPosture.root_pos = startPosture->root_pos * (1 - t) + endPosture->root_pos * t;

         // interpolate bone rotations
         for (int bone = 0; bone < MAX_BONES_IN_ASF_FILE; bone++) {
            Euler2Quaternion(startPosture->bone_rotation[bone].p, q_start);
            Euler2Quaternion(endPosture->bone_rotation[bone].p, q_end);
            q_result = Slerp(t, q_start, q_end);
            Quaternion2Euler(q_result, interpolatedPosture.bone_rotation[bone].p);
         }
            
         pOutputMotion->SetPosture(startKeyframe + frame, interpolatedPosture);
      }

      startKeyframe = endKeyframe;
   }

   pCounter.StopCounter();
   std::cout << "Computation Time for SLERP Quaternion Interpolation: " << pCounter.GetElapsedTime() << " sec" << std::endl;

   for (int frame = startKeyframe + 1; frame < inputLength; frame++) {
      pOutputMotion->SetPosture(frame, *(pInputMotion->GetPosture(frame)));
   }
}

void Interpolator::BezierInterpolationQuaternion(Motion * pInputMotion, Motion * pOutputMotion, int N)
{
   int inputLength = pInputMotion->GetNumFrames(); // frames are indexed 0, ..., inputLength-1
   int startKeyframe = 0, endKeyframe, prevKeyframe, nextKeyframe;

   /* v1 and v2 below correspond to startKeyframe and endKeyframe respectively. */
   vector v0_r, v1_r, v2_r, v3_r; // keyframe root positions. 
   Quaternion<double> q0_b, q1_b, q2_b, q3_b; // keyframe bone rotations.
   /* a1, b2 correspond to a_N, and b_(N+1) respectively. a2 corresponds to a_(N+1)_bar. */
   vector a1_r, a2_r, b2_r; // spline control Euler angles of root positions.  
   Quaternion<double> a1_b, a2_b, b2_b; // spline control quaternions of bone rotations. 
   Quaternion<double> q_result;

   pCounter.StartCounter();
   while (startKeyframe + N + 1 < inputLength) {
      endKeyframe = startKeyframe + N + 1;
      prevKeyframe = startKeyframe - N - 1;
      nextKeyframe = endKeyframe + N + 1;
      Posture* startPosture = pInputMotion->GetPosture(startKeyframe);
      Posture* endPosture = pInputMotion->GetPosture(endKeyframe);
      Posture* prevPosture;
      Posture* nextPosture;
      // copy start and end keyframe
      pOutputMotion->SetPosture(startKeyframe, *startPosture);
      pOutputMotion->SetPosture(endKeyframe, *endPosture);

      if (startKeyframe == 0) { /* first keyframe */
         nextPosture = pInputMotion->GetPosture(nextKeyframe);

         // control points of root positions
         v1_r = startPosture->root_pos;
         v2_r = endPosture->root_pos;
         v3_r = nextPosture->root_pos;
         a1_r = v1_r + (v3_r + (v2_r - v3_r) * 2.0 - v1_r) * 1.0 / 3;
         a2_r = v1_r + (v2_r - v1_r) * 2.0;
         a2_r = a2_r + (v3_r - a2_r) * 0.5; // a2_bar
         b2_r = v2_r - (a2_r - v2_r) * 1.0 / 3;

         // interpolate in between
         for (int frame = 1; frame <= N; frame++)
         {
            Posture interpolatedPosture;
            double t = 1.0 * frame / (N + 1);

            // interpolate root position
            interpolatedPosture.root_pos = DeCasteljauEuler(t, v1_r, a1_r, b2_r, v2_r);

            // interpolate bone rotations
            for (int bone = 0; bone < MAX_BONES_IN_ASF_FILE; bone++) {
               // control quaternions of bone rotations
               Euler2Quaternion(startPosture->bone_rotation[bone].p, q1_b);
               Euler2Quaternion(endPosture->bone_rotation[bone].p, q2_b);
               Euler2Quaternion(nextPosture->bone_rotation[bone].p, q3_b);
               a1_b = Slerp(1.0 / 3, q1_b, Double(q3_b, q2_b)); 
               a2_b = Slerp(0.5, Double(q1_b, q2_b), q3_b); // a2_bar
               b2_b = Slerp(-1.0 / 3, q2_b, a2_b);
               q_result = DeCasteljauQuaternion(t, q1_b, a1_b, b2_b, q2_b);
               Quaternion2Euler(q_result, interpolatedPosture.bone_rotation[bone].p);
            }

            pOutputMotion->SetPosture(startKeyframe + frame, interpolatedPosture);
         }
      }
      else if (nextKeyframe >= inputLength) { /* last keyframe */
         prevPosture = pInputMotion->GetPosture(prevKeyframe);

         // control points of root positions
         v0_r = prevPosture->root_pos;
         v1_r = startPosture->root_pos;
         v2_r = endPosture->root_pos;
         a1_r = v0_r + (v1_r - v0_r) * 2.0;
         a1_r = a1_r + (v2_r - a1_r) * 0.5; //a1_bar
         a1_r = v1_r + (a1_r - v1_r) * 1.0 / 3;
         b2_r = v2_r + (v0_r + (v1_r - v0_r) * 2.0 - v2_r) * 1.0 / 3;

         // interpolate in between
         for (int frame = 1; frame <= N; frame++)
         {
            Posture interpolatedPosture;
            double t = 1.0 * frame / (N + 1);

            // interpolate root position
            interpolatedPosture.root_pos = DeCasteljauEuler(t, v1_r, a1_r, b2_r, v2_r);

            // interpolate bone rotations
            for (int bone = 0; bone < MAX_BONES_IN_ASF_FILE; bone++) {
               // control quaternions of bone rotations
               Euler2Quaternion(prevPosture->bone_rotation[bone].p, q0_b);
               Euler2Quaternion(startPosture->bone_rotation[bone].p, q1_b);
               Euler2Quaternion(endPosture->bone_rotation[bone].p, q2_b);
               a1_b = Slerp(0.5, Double(q0_b, q1_b), q2_b); //a1_bar
               a1_b = Slerp(1.0 / 3, q1_b, a1_b);
               b2_b = Slerp(1.0 / 3, q2_b, Double(q0_b, q1_b));
               q_result = DeCasteljauQuaternion(t, q1_b, a1_b, b2_b, q2_b);
               Quaternion2Euler(q_result, interpolatedPosture.bone_rotation[bone].p); 
            }

            pOutputMotion->SetPosture(startKeyframe + frame, interpolatedPosture);
         }
      }
      else {
         prevPosture = pInputMotion->GetPosture(prevKeyframe);
         nextPosture = pInputMotion->GetPosture(nextKeyframe);

         // control points of root positions
         v0_r = prevPosture->root_pos;
         v1_r = startPosture->root_pos;
         v2_r = endPosture->root_pos;
         v3_r = nextPosture->root_pos;
         a1_r = v0_r + (v1_r - v0_r) * 2.0;
         a1_r = a1_r + (v2_r - a1_r) * 0.5; //a1_bar
         a1_r = v1_r + (a1_r - v1_r) * 1.0 / 3;
         a2_r = v1_r + (v2_r - v1_r) * 2.0;
         a2_r = a2_r + (v3_r - a2_r) * 0.5; //a2_bar
         b2_r = v2_r - (a2_r - v2_r) * 1.0 / 3;

         // interpolate in between
         for (int frame = 1; frame <= N; frame++)
         {
            Posture interpolatedPosture;
            double t = 1.0 * frame / (N + 1);

            // interpolate root position
            interpolatedPosture.root_pos = DeCasteljauEuler(t, v1_r, a1_r, b2_r, v2_r);

            // interpolate bone rotations
            for (int bone = 0; bone < MAX_BONES_IN_ASF_FILE; bone++) {
               // control quaternions of bone rotations
               Euler2Quaternion(prevPosture->bone_rotation[bone].p, q0_b);
               Euler2Quaternion(startPosture->bone_rotation[bone].p, q1_b);
               Euler2Quaternion(endPosture->bone_rotation[bone].p, q2_b);
               Euler2Quaternion(nextPosture->bone_rotation[bone].p, q3_b); 
               a1_b = Slerp(0.5, Double(q0_b, q1_b), q2_b); //a1_bar
               a1_b = Slerp(1.0 / 3, q1_b, a1_b);
               a2_b = Slerp(0.5, Double(q1_b, q2_b), q3_b); //a2_bar
               b2_b = Slerp(-1.0 / 3, q2_b, a2_b); 
               q_result = DeCasteljauQuaternion(t, q1_b, a1_b, b2_b, q2_b);
               Quaternion2Euler(q_result, interpolatedPosture.bone_rotation[bone].p); 
            }

            pOutputMotion->SetPosture(startKeyframe + frame, interpolatedPosture);
         }
      }

      startKeyframe = endKeyframe;
   }

   pCounter.StopCounter();
   std::cout << "Computation Time for Bezier SLERP Quaternion Interpolation: " << pCounter.GetElapsedTime() << " sec" << std::endl;

   for (int frame = startKeyframe + 1; frame < inputLength; frame++) {
      pOutputMotion->SetPosture(frame, *(pInputMotion->GetPosture(frame)));
   }
}

void Interpolator::Euler2Quaternion(double angles[3], Quaternion<double> & q) 
{  
   double R[9]; // intermediate rotation matrix
   Euler2Rotation(angles, R);
   q = Quaternion<double>::Matrix2Quaternion(R);
   q.Normalize();
}

void Interpolator::Quaternion2Euler(Quaternion<double> & q, double angles[3]) 
{ 
   double R[9]; // intermediate rotation matrix
   q.Quaternion2Matrix(R);
   Rotation2Euler(R, angles);
}

Quaternion<double> Interpolator::Slerp(double t, Quaternion<double> & qStart, Quaternion<double> & qEnd_)
{
   Quaternion<double> result; 
   double cosTheta, theta;
   cosTheta = qStart.Gets() * qEnd_.Gets() + qStart.Getx() * qEnd_.Getx() + qStart.Gety() * qEnd_.Gety() + qStart.Getz() * qEnd_.Getz();
   // To slerp over the shorter great arc, enforce cos(theta) >= 0 by flipping any one quaternion
   if (cosTheta < 0) {
      qStart = (-1.0) * qStart;
      cosTheta *= -1.0;
   }

   theta = acos(cosTheta);
   if (theta == 0.0) { return qStart; }
   result = sin((1 - t) * theta) / sin(theta) * qStart + sin(t * theta) / sin(theta) * qEnd_;

   return result;
}

Quaternion<double> Interpolator::Double(Quaternion<double> p, Quaternion<double> q)
{
  Quaternion<double> result;

  // 2(p*q)q - p, where (p*q) is a dot product
  result = 2 * (p.Gets() * q.Gets() + p.Getx() * q.Getx() + p.Gety() * q.Gety() + p.Getz() * q.Getz()) * q - p;
  return result;
}

vector Interpolator::DeCasteljauEuler(double t, vector p0, vector p1, vector p2, vector p3)
{
  vector result, Q0, Q1, Q2, R0, R1;
  Q0 = p0 * (1 - t) + p1 * t;
  Q1 = p1 * (1 - t) + p2 * t;
  Q2 = p2 * (1 - t) + p3 * t;
  R0 = Q0 * (1 - t) + Q1 * t;
  R1 = Q1 * (1 - t) + Q2 * t;
  result = R0 * (1 - t) + R1 * t;
  return result;
}

Quaternion<double> Interpolator::DeCasteljauQuaternion(double t, Quaternion<double> p0, Quaternion<double> p1, Quaternion<double> p2, Quaternion<double> p3)
{
  Quaternion<double> result, Q0, Q1, Q2, R0, R1;
  Q0 = Slerp(t, p0, p1);
  Q1 = Slerp(t, p1, p2);
  Q2 = Slerp(t, p2, p3);
  R0 = Slerp(t, Q0, Q1);
  R1 = Slerp(t, Q1, Q2);
  result = Slerp(t, R0, R1);
  return result;
}

