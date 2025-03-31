/*

  USC/Viterbi/Computer Science
  "Jello Cube" Assignment 1 starter code

*/

#ifndef _PHYSICS_H_
#define _PHYSICS_H_

void computeAcceleration(struct world * jello, struct point a[8][8][8]);
point computeNetForce(const point& pA, const point& pB, const point& vA, \
   const point& vB, double coeffK, double coeffD);
point computeAccStructBend(struct world* jello, int i, int j, int k);
point computeAccShear(struct world* jello, int i, int j, int k);
point checkCollision(struct world* jello, int i, int j, int k);
point computeAccFField(struct world* jello, int i, int j, int k);

// perform one step of Euler and Runge-Kutta-4th-order integrators
// updates the jello structure accordingly
void Euler(struct world * jello);
void RK4(struct world * jello);

#endif

