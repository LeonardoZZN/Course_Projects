/*

  USC/Viterbi/Computer Science
  "Jello Cube" Assignment 1 starter code

*/

#include "jello.h"
#include "physics.h"

double restLen; /* rest length of structural springs */
point L; /* vector pointing from another point to current point, can be normalized or not */
double length; /* length of L */
point vDiff; /* difference in velocities of two points */
double product; /* inner product of two vectors */

/*	Computes acceleration to every control point of the jello cube, 
	which is in state given by 'jello'.
   Returns result in array 'a'. */
void computeAcceleration(struct world * jello, struct point a[8][8][8])
{
	int i,j,k;

	/*	accelerations due to forces (Hook's + damping)
		exerted by structural, shear, and bend springs respectively,
		where the accelerations caused by structural and bend springs
		are summed together for convenience in calculation. */
	point accStructBend, accShear; 

	/* acceleration from force exerted by collision springs */
	point accCollision;

	/* acceleration derived from the external force field */
	point accFField;

	for (i = 0; i <= 7; i++) 
		for (j = 0; j <= 7; j++)
			for (k = 0; k <= 7; k++) {
				accStructBend = computeAccStructBend(jello, i, j, k);
				accShear = computeAccShear(jello, i, j, k);
				accCollision = checkCollision(jello, i, j, k);
				pSUM(accStructBend, accShear, a[i][j][k]);
				pSUM(a[i][j][k], accCollision, a[i][j][k]);
				if (jello->resolution != 0) {
					accFField = computeAccFField(jello, i, j, k);
					pSUM(a[i][j][k], accFField, a[i][j][k]);
				}
			}
}

/* Computes the combined Hook's and damping forces exerted on point pA 
	by the spring connecting pA and pB.
	Returns result in a point type. */
point computeNetForce(const point& pA, const point& pB, const point& vA, \
	const point& vB, double coeffK, double coeffD)
{
	point outF;
	pMAKE(0.0, 0.0, 0.0, outF);
	point temp;

	pDIFFERENCE(pA, pB, L);
	pNORMALIZE(L);
	/* Hook's force */
	pMULTIPLY(L, (restLen - length) * coeffK, temp);
	pSUM(temp, outF, outF);
	/* damping force */
	pDIFFERENCE(vA, vB, vDiff);
	INNERPRODUCTp(vDiff, L, product);
	pMULTIPLY(L, -1 * product * coeffD, temp);
	pSUM(temp, outF, outF);
	return outF;
}

/*	Computes acceleration due to the combined Hook's and damping forces 
	exerted by both structural and bend springs on the jello point jello->p[i][j][k].
	Returns result in a point as a 3d vector. */
point computeAccStructBend(struct world* jello, int i, int j, int k)
{
	point res; /* return variable */
	pMAKE(0.0, 0.0, 0.0, res);
	point temp;

	for (int f = 1; f <= 2; f++) {
		if (f == 1) {
			restLen = 1.0 / 7; /* for structural springs */
		}
		else {
			restLen = 2.0 / 7; /* for bend springs */
		}

		if (i + f <= 7) {
			temp = computeNetForce(jello->p[i][j][k], jello->p[i + f][j][k], \
				jello->v[i][j][k], jello->v[i + f][j][k], jello->kElastic, jello->dElastic);
			pSUM(temp, res, res);
		}

		if (i - f >= 0) {
			temp = computeNetForce(jello->p[i][j][k], jello->p[i - f][j][k], \
				jello->v[i][j][k], jello->v[i - f][j][k], jello->kElastic, jello->dElastic);
			pSUM(temp, res, res);
		}

		if (j + f <= 7) {
			temp = computeNetForce(jello->p[i][j][k], jello->p[i][j + f][k], \
				jello->v[i][j][k], jello->v[i][j + f][k], jello->kElastic, jello->dElastic);
			pSUM(temp, res, res);
		}

		if (j - f >= 0) {
			temp = computeNetForce(jello->p[i][j][k], jello->p[i][j - f][k], \
				jello->v[i][j][k], jello->v[i][j - f][k], jello->kElastic, jello->dElastic);
			pSUM(temp, res, res);
		}

		if (k + f <= 7) {
			temp = computeNetForce(jello->p[i][j][k], jello->p[i][j][k + f], \
				jello->v[i][j][k], jello->v[i][j][k + f], jello->kElastic, jello->dElastic);
			pSUM(temp, res, res);
		}

		if (k - f >= 0) {
			temp = computeNetForce(jello->p[i][j][k], jello->p[i][j][k - f], \
				jello->v[i][j][k], jello->v[i][j][k - f], jello->kElastic, jello->dElastic);
			pSUM(temp, res, res);
		}
	}

	pMULTIPLY(res, 1 / jello->mass, res);
	return res;
}

/*	Computes acceleration due to the combined Hook's and damping forces
	exerted by shear springs on the jello point jello->p[i][j][k].
	Returns result in a point as a 3d vector. */
point computeAccShear(struct world* jello, int i, int j, int k)
{
	point res; /* return variable */
	pMAKE(0.0, 0.0, 0.0, res);
	point temp;
	
	for (int dx = -1; dx <= 1; dx++) 
		for (int dy = -1; dy <= 1; dy++) 
			for (int dz = -1; dz <= 1; dz++) {
				if ((dx == 0 && dy == 0 && dz == 0) || (dx != 0 && dy == 0 && dz == 0)\
					|| (dx == 0 && dy != 0 && dz == 0) || (dx == 0 && dy == 0 && dz != 0)) {
					continue;
				}

				if (dx * dy * dz == 0) {
					restLen = sqrt(2) / 7;
				}
				else {
					restLen = sqrt(3) / 7;
				}

				if (i + dx >= 0 && i + dx <= 7 && j + dy >= 0 && j + dy <= 7 && k + dz >= 0 \
					&& k + dz <= 7) {
					temp = computeNetForce(jello->p[i][j][k], jello->p[i + dx][j + dy][k + dz], \
						jello->v[i][j][k], jello->v[i + dx][j + dy][k + dz], jello->kElastic, jello->dElastic);
					pSUM(temp, res, res);
				}
			}

	pMULTIPLY(res, 1 / jello->mass, res); 
	return res;
}

/* Performs collision detection at boundaries of the bounding box.
	If collision occurs, computes the acceleartion caused by the collision spring 
	located at the contact point, and returns the result in a point type.
	If no collision occurs, returns a zero vector. */
point checkCollision(struct world* jello, int i, int j, int k)
{
	point res;
	point temp;
	point pA;
	point pB;
	point vA;
	point vB;
	pMAKE(0.0, 0.0, 0.0, res);
	pCPY(jello->v[i][j][k],vA);
	pMAKE(0.0, 0.0, 0.0, vB);
	restLen = 0.0;

	/* Composition of forces */
	if (jello->p[i][j][k].x <= -2.0) {
		pMAKE(jello->p[i][j][k].x, 0.0, 0.0, pA);
		pMAKE(-2.0, 0.0, 0.0, pB);
		temp = computeNetForce(pA, pB, vA, vB, jello->kCollision, jello->dCollision);
		pSUM(temp, res, res);
	}

	if (jello->p[i][j][k].x >= 2.0) {
		pMAKE(jello->p[i][j][k].x, 0.0, 0.0, pA);
		pMAKE(2.0, 0.0, 0.0, pB);
		temp = computeNetForce(pA, pB, vA, vB, jello->kCollision, jello->dCollision);
		pSUM(temp, res, res);
	}

	if (jello->p[i][j][k].y <= -2.0) {
		pMAKE(0.0, jello->p[i][j][k].y, 0.0, pA);
		pMAKE(0.0, -2.0, 0.0, pB);
		temp = computeNetForce(pA, pB, vA, vB, jello->kCollision, jello->dCollision);
		pSUM(temp, res, res);
	}

	if (jello->p[i][j][k].y >= 2.0) {
		pMAKE(0.0, jello->p[i][j][k].y, 0.0, pA);
		pMAKE(0.0, 2.0, 0.0, pB);
		temp = computeNetForce(pA, pB, vA, vB, jello->kCollision, jello->dCollision);
		pSUM(temp, res, res);
	}

	if (jello->p[i][j][k].z <= -2.0) {
		pMAKE(0.0, 0.0, jello->p[i][j][k].z, pA);
		pMAKE(0.0, 0.0, -2.0, pB);
		temp = computeNetForce(pA, pB, vA, vB, jello->kCollision, jello->dCollision);
		pSUM(temp, res, res);
	}

	if (jello->p[i][j][k].z >= 2.0) {
		pMAKE(0.0, 0.0, jello->p[i][j][k].z, pA); 
		pMAKE(0.0, 0.0, 2.0, pB);
		temp = computeNetForce(pA, pB, vA, vB, jello->kCollision, jello->dCollision);
		pSUM(temp, res, res);
	}

	// collision detection with the input inclined plane
	if (jello->incPlanePresent) {
		double check = jello->p[i][j][k].x * jello->a + jello->p[i][j][k].y * jello->b \
			+ jello->p[i][j][k].z * jello->c + jello->d;
		double t;
		double contactX; // x coordinate of the contact point
		double contactY; // y coordinate of the contact point
		double contactZ; // z coordinate of the contact point
		if (jello->d >= 0) {
			if (check <= 0) {
				pMAKE(jello->p[i][j][k].x, jello->p[i][j][k].y, jello->p[i][j][k].z, pA); 
				t = -check / (jello->a * jello->a + jello->b * jello->b + jello->c * jello->c);
				contactX = jello->p[i][j][k].x + jello->a * t;
				contactY = jello->p[i][j][k].y + jello->b * t;
				contactZ = jello->p[i][j][k].z + jello->c * t;
				pMAKE(contactX, contactY, contactZ, pB);
				temp = computeNetForce(pA, pB, vA, vB, jello->kCollision, jello->dCollision);
				pSUM(temp, res, res);
			}
		}
		else {
			if (check >= 0) {
				pMAKE(jello->p[i][j][k].x, jello->p[i][j][k].y, jello->p[i][j][k].z, pA);
				t = -check / (jello->a * jello->a + jello->b * jello->b + jello->c * jello->c);
				contactX = jello->p[i][j][k].x + jello->a * t;
				contactY = jello->p[i][j][k].y + jello->b * t;
				contactZ = jello->p[i][j][k].z + jello->c * t;
				pMAKE(contactX, contactY, contactZ, pB);
				temp = computeNetForce(pA, pB, vA, vB, jello->kCollision, jello->dCollision);
				pSUM(temp, res, res);
			}
		}
	}

	pMULTIPLY(res, 1 / jello->mass, res);
	return res;
}

/*	Computes acceleration due to the external non-homogeneous time-independent
	force field. Supports a grid resolution between 2 and 30.
	Applies trilinear interpolation to find the field force exerted on
	one jello sampling point.
	Returns result in a point type as a 3d vector. */
point computeAccFField(struct world* jello, int i, int j, int k)
{
	point res;
	pMAKE(0.0, 0.0, 0.0, res);
	point temp;

	/* Out of the force field */
	if (jello->p[i][j][k].x < -2.0 || jello->p[i][j][k].x > 2.0 || jello->p[i][j][k].y < -2.0 || jello->p[i][j][k].y > 2.0 
		|| jello->p[i][j][k].z < -2.0 || jello->p[i][j][k].z > 2.0) { 
			return res;
	}

	int iBase = floor((jello->p[i][j][k].x + 2.0) / 4.0 * (jello->resolution - 1)); // nearest lower field index along x-axis 
	int jBase = floor((jello->p[i][j][k].y + 2.0) / 4.0 * (jello->resolution - 1)); // nearest lower field index along y-axis
	int kBase = floor((jello->p[i][j][k].z + 2.0) / 4.0 * (jello->resolution - 1)); // nearest lower field index along z-axis

	/* ratios along three axes in each grid unit cube */
	double rx = (jello->p[i][j][k].x \
		- jello->forceField[iBase * jello->resolution * jello->resolution + jBase * jello->resolution + kBase].x) / 4.0 * (jello->resolution - 1);
	double ry = (jello->p[i][j][k].y \
		- jello->forceField[iBase * jello->resolution * jello->resolution + jBase * jello->resolution + kBase].y) / 4.0 * (jello->resolution - 1);
	double rz = (jello->p[i][j][k].z \
		- jello->forceField[iBase * jello->resolution * jello->resolution + jBase * jello->resolution + kBase].z) / 4.0 * (jello->resolution - 1);

	/* Trilinear Interpolation */
	pMULTIPLY(jello->forceField[iBase * jello->resolution * jello->resolution + jBase * jello->resolution + kBase], (1 - rx) * (1 - ry) * (1 - rz), temp);
	pSUM(res, temp, res);
	if (iBase + 1 < jello->resolution) {
		pMULTIPLY(jello->forceField[(iBase + 1) * jello->resolution * jello->resolution + jBase * jello->resolution + kBase], rx * (1 - ry) * (1 - rz), temp);
		pSUM(res, temp, res);
	}
	if (jBase + 1 < jello->resolution) {
		pMULTIPLY(jello->forceField[iBase * jello->resolution * jello->resolution + (jBase + 1) * jello->resolution + kBase], (1 - rx) * ry * (1 - rz), temp);
		pSUM(res, temp, res);
	}
	if (kBase + 1 < jello->resolution) {
		pMULTIPLY(jello->forceField[iBase * jello->resolution * jello->resolution + jBase * jello->resolution + (kBase + 1)], (1 - rx) * (1 - ry) * rz, temp);
		pSUM(res, temp, res);
	}
	if ((iBase + 1 < jello->resolution) && (kBase + 1 < jello->resolution)) {
		pMULTIPLY(jello->forceField[(iBase + 1) * jello->resolution * jello->resolution + jBase * jello->resolution + (kBase + 1)], rx * (1 - ry) * rz, temp);
		pSUM(res, temp, res);
	}
	if ((iBase + 1 < jello->resolution) && (jBase + 1 < jello->resolution)) {
		pMULTIPLY(jello->forceField[(iBase + 1) * jello->resolution * jello->resolution + (jBase + 1) * jello->resolution + kBase], rx * ry * (1 - rz), temp);
		pSUM(res, temp, res);
	}
	if ((jBase + 1 < jello->resolution) && (kBase + 1 < jello->resolution)) {
		pMULTIPLY(jello->forceField[iBase * jello->resolution * jello->resolution + (jBase + 1) * jello->resolution + (kBase + 1)], (1 - rx) * ry * rz, temp);
		pSUM(res, temp, res);
	}
	if ((iBase + 1 < jello->resolution) && (jBase + 1 < jello->resolution) && (kBase + 1 < jello->resolution)) {
		pMULTIPLY(jello->forceField[(iBase + 1) * jello->resolution * jello->resolution + (jBase + 1) * jello->resolution + (kBase + 1)], rx * ry * rz, temp);
		pSUM(res, temp, res); 
	}

	pMULTIPLY(res, 1 / jello->mass, res);
	return res;
}

/* performs one step of Euler Integration */
/* as a result, updates the jello structure */
void Euler(struct world * jello)
{
  int i,j,k;
  point a[8][8][8];

  computeAcceleration(jello, a);
  
  for (i=0; i<=7; i++)
	for (j=0; j<=7; j++)
	  for (k=0; k<=7; k++)
	  {
		jello->p[i][j][k].x += jello->dt * jello->v[i][j][k].x;
		jello->p[i][j][k].y += jello->dt * jello->v[i][j][k].y;
		jello->p[i][j][k].z += jello->dt * jello->v[i][j][k].z;
		jello->v[i][j][k].x += jello->dt * a[i][j][k].x;
		jello->v[i][j][k].y += jello->dt * a[i][j][k].y;
		jello->v[i][j][k].z += jello->dt * a[i][j][k].z;
	  }
}

/* performs one step of RK4 Integration */
/* as a result, updates the jello structure */
void RK4(struct world * jello)
{
  point F1p[8][8][8], F1v[8][8][8], 
		F2p[8][8][8], F2v[8][8][8],
		F3p[8][8][8], F3v[8][8][8],
		F4p[8][8][8], F4v[8][8][8];

  point a[8][8][8];


  struct world buffer;

  int i,j,k;

  buffer = *jello; // make a copy of jello

  computeAcceleration(jello, a);

  for (i=0; i<=7; i++)
	for (j=0; j<=7; j++)
	  for (k=0; k<=7; k++)
	  {
		 pMULTIPLY(jello->v[i][j][k],jello->dt,F1p[i][j][k]);
		 pMULTIPLY(a[i][j][k],jello->dt,F1v[i][j][k]);
		 pMULTIPLY(F1p[i][j][k],0.5,buffer.p[i][j][k]);
		 pMULTIPLY(F1v[i][j][k],0.5,buffer.v[i][j][k]);
		 pSUM(jello->p[i][j][k],buffer.p[i][j][k],buffer.p[i][j][k]);
		 pSUM(jello->v[i][j][k],buffer.v[i][j][k],buffer.v[i][j][k]);
	  }

  computeAcceleration(&buffer, a);

  for (i=0; i<=7; i++)
	for (j=0; j<=7; j++)
	  for (k=0; k<=7; k++)
	  {
		 // F2p = dt * buffer.v;
		 pMULTIPLY(buffer.v[i][j][k],jello->dt,F2p[i][j][k]);
		 // F2v = dt * a(buffer.p,buffer.v);     
		 pMULTIPLY(a[i][j][k],jello->dt,F2v[i][j][k]);
		 pMULTIPLY(F2p[i][j][k],0.5,buffer.p[i][j][k]);
		 pMULTIPLY(F2v[i][j][k],0.5,buffer.v[i][j][k]);
		 pSUM(jello->p[i][j][k],buffer.p[i][j][k],buffer.p[i][j][k]);
		 pSUM(jello->v[i][j][k],buffer.v[i][j][k],buffer.v[i][j][k]);
	  }

  computeAcceleration(&buffer, a);

  for (i=0; i<=7; i++)
	for (j=0; j<=7; j++)
	  for (k=0; k<=7; k++)
	  {
		 // F3p = dt * buffer.v;
		 pMULTIPLY(buffer.v[i][j][k],jello->dt,F3p[i][j][k]);
		 // F3v = dt * a(buffer.p,buffer.v);     
		 pMULTIPLY(a[i][j][k],jello->dt,F3v[i][j][k]);
		 pMULTIPLY(F3p[i][j][k],1.0,buffer.p[i][j][k]);
		 pMULTIPLY(F3v[i][j][k],1.0,buffer.v[i][j][k]);
		 pSUM(jello->p[i][j][k],buffer.p[i][j][k],buffer.p[i][j][k]);
		 pSUM(jello->v[i][j][k],buffer.v[i][j][k],buffer.v[i][j][k]);
	  }
		 
  computeAcceleration(&buffer, a);


  for (i=0; i<=7; i++)
	for (j=0; j<=7; j++)
	  for (k=0; k<=7; k++)
	  {
		 // F3p = dt * buffer.v;
		 pMULTIPLY(buffer.v[i][j][k],jello->dt,F4p[i][j][k]);
		 // F3v = dt * a(buffer.p,buffer.v);     
		 pMULTIPLY(a[i][j][k],jello->dt,F4v[i][j][k]);

		 pMULTIPLY(F2p[i][j][k],2,buffer.p[i][j][k]);
		 pMULTIPLY(F3p[i][j][k],2,buffer.v[i][j][k]);
		 pSUM(buffer.p[i][j][k],buffer.v[i][j][k],buffer.p[i][j][k]);
		 pSUM(buffer.p[i][j][k],F1p[i][j][k],buffer.p[i][j][k]);
		 pSUM(buffer.p[i][j][k],F4p[i][j][k],buffer.p[i][j][k]);
		 pMULTIPLY(buffer.p[i][j][k],1.0 / 6,buffer.p[i][j][k]);
		 pSUM(buffer.p[i][j][k],jello->p[i][j][k],jello->p[i][j][k]);

		 pMULTIPLY(F2v[i][j][k],2,buffer.p[i][j][k]);
		 pMULTIPLY(F3v[i][j][k],2,buffer.v[i][j][k]);
		 pSUM(buffer.p[i][j][k],buffer.v[i][j][k],buffer.p[i][j][k]);
		 pSUM(buffer.p[i][j][k],F1v[i][j][k],buffer.p[i][j][k]);
		 pSUM(buffer.p[i][j][k],F4v[i][j][k],buffer.p[i][j][k]);
		 pMULTIPLY(buffer.p[i][j][k],1.0 / 6,buffer.p[i][j][k]);
		 pSUM(buffer.p[i][j][k],jello->v[i][j][k],jello->v[i][j][k]);
	  }

  return;  
}
