
/*
  Keyboard inputs:
  +  increase subdivision depth
  -  decrease subdivision depth
  s  flat shading (regular object)
  w  wireless shape
  q  quit
*/

#include <stdlib.h>
#include <math.h>
#include <GLUT/glut.h>
#include "6.h"

#define X .8
#define A 4
#define B 1
#define D 9

static GLfloat e0[3]={0,3,0};


/* vertex data array */
static GLfloat vdata[12][3] = {
  {-X*2 + e0[0], 0.0+ e0[1], X*3 +e0[2]}, {X*2+ e0[0], 0.0 +e0[1], X*3 +e0[2]}, {-X*2+ e0[0], 0.0 +e0[1], -X*3 +e0[2]}, {X*2+ e0[0], 0.0 +e0[1], -X*3 +e0[2]},
  {0.0+ e0[0], X +e0[1], X*3 +e0[2]}, {0.0+ e0[0], X +e0[1], -X*3 +e0[2]}, {0.0+ e0[0], -X +e0[1], X*3 +e0[2]}, {0.0+ e0[0], -X +e0[1], -X*3 +e0[2]},
  {X*2+ e0[0], X +e0[1], 0.0 +e0[2]}, {-X*2+ e0[0], X +e0[1], 0.0 +e0[2]}, {X*2+ e0[0], -X +e0[1], 0.0 +e0[2]}, {-X*2+ e0[0], -X +e0[1], 0.0 +e0[2]}
};

/* triangle indices */
static int tindices[20][3] = {
  {1,4,0}, {4,9,0}, {4,5,9}, {8,5,4}, {1,8,4},
  {1,10,8}, {10,3,8}, {8,3,5}, {3,2,5}, {3,7,2},
  {3,10,7}, {10,6,7}, {6,11,7}, {6,0,11}, {6,1,0},
  {10,1,6}, {11,0,9}, {2,11,9}, {5,2,9}, {11,2,7}
};

// GLfloat mat_specular[] = {0.0, 0.0, 0.0, 1.0};
// GLfloat mat_diffuse[]  = {0.8, 0.6, 0.4, 1.0};
// GLfloat mat_ambient[]  = {0.8, 0.6, 0.4, 1.0};
// GLfloat mat_shininess = 100.0;	/* unused if specular is 0 */

// GLfloat light_ambient[]  = {0.2, 0.2, 0.2, 1.0};
// GLfloat light_diffuse[]  = {1.0, 1.0, 1.0, 1.0};
// GLfloat light_specular[] = {0.0, 0.0, 0.0, 1.0};

// GLfloat light_position[] = {1.8, 1.2, -2.0, 0.0}; /* directional */

int flat = 1;			/* 0 = smooth shading, 1 = flat shading */
extern int subdiv;			/* number of subdivisions */
extern int solid;

/* normalize a vector of non-zero length */
void normalizeEllipsoid(GLfloat v[3])
{
  GLfloat d = sqrt(2*X*X/(
    (v[0]-e0[0])*(v[0]-e0[0])/A + 
    (v[1]-e0[1])*(v[1]-e0[1])/B + 
    (v[2]-e0[2])*(v[2]-e0[2])/D));
  /* omit explict check for division by zero */

  v[0] = d*(v[0]-e0[0])+e0[0]; 
  v[1] = d*(v[1]-e0[1])+e0[1]; 
  v[2] = d*(v[2]-e0[2])+e0[2];
 
}

/* normalized cross product of non-parallel vectors */
void normCrossProde (GLfloat u[3], GLfloat v[3], GLfloat n[3])
{
  n[0] = u[1]*v[2] - u[2]*v[1];
  n[1] = u[2]*v[0] - u[0]*v[2];
  n[2] = u[0]*v[1] - u[1]*v[0];
  normalizeEllipsoid(n);
}

void normFacee(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3])
{
  GLfloat d1[3], d2[3], n[3];
  int k;
  for (k = 0; k < 3; k++) {
    d1[k] = v1[k] - v2[k];
    d2[k] = v2[k] - v3[k];
  }
  normCrossProde(d1, d2, n);
  glNormal3fv(n);
}

/* draw triangle using face normals */
void drawTriangleFlat(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3])
{
  glBegin(GL_TRIANGLES);
    normFacee(v1, v2, v3);
    glVertex3fv(v1);
    glVertex3fv(v2);
    glVertex3fv(v3);
  glEnd();
}

/* draw triangle using sphere normals */
void drawTriangleSmooth(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3])
{
  glBegin(GL_TRIANGLES);
    glNormal3fv(v1);
    glVertex3fv(v1);
    glNormal3fv(v2);
    glVertex3fv(v2);
    glNormal3fv(v3);
    glVertex3fv(v3);
  glEnd();
}

