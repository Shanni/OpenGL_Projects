
#ifndef _6_H_
#define _6_H_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <GLUT/glut.h>
#include <math.h>

/* global variables */
static int win(0); // GLUT window ID
static int win_width(600), win_height(600); // window's width and height
static float mouse_x(0.0), mouse_y(0.0);
static float px(0.0), py(0.0), pz(0.0), range(2.0);
static float rotation_x(0.0), rotation_y(0.0);
static float PI(3.14159);

/** for sphere-ray sample */
static float R(3.0); //to draw a sphere
static float R1(0.6); //to draw a little circle
static float R2(0.9); //to draw a little circle
static float floorSize(3.5);
static int numberOfDots(200);
static GLfloat eta(.7);

/** for sphere-spiral-curve */
static int C(3);
int linerefraction = 0;

GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_diffuse[]  = {0.6, 0.6, 0.8, 1.0};
GLfloat mat_ambient[]  = {0.6, 0.6, 0.7, 1.0};
GLfloat mat_emission[] = {0.3, 0.2, 0.2, 0.0};
GLfloat mat_shininess = 100;  /* unused if specular is 0 */

GLfloat light_ambient[]  = {0.2, 0.2, 0.2, 1.0};
GLfloat light_diffuse[]  = {1.0, 1.0, 1.0, 1.0};
GLfloat light_specular[] = {0.0, 0.0, 0.0, 1.0};

GLfloat lmodel_ambient[] = { 0.4, 0.4, 0.4, 1.0 };
GLfloat local_view[] = { 0 };

GLfloat light_position[] = {.5, 6, 2, 0.0}; /* directional */
//GLfloat light_position[] = { 0.0, 3.0, 2.0, 0.0 };


/* a = b - c */
#define vector(a,b,c) \
(a)[0] = (b)[0] - (c)[0]; \
(a)[1] = (b)[1] - (c)[1]; \
(a)[2] = (b)[2] - (c)[2];


void printVector(GLfloat v[3]) {
  printf("vector is: %f, %f, %f\n", v[0], v[1], v[2]);
}

GLfloat norm(GLfloat v[3]) {
  return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

void normalize(GLfloat v[3]) {
  GLfloat d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
  v[0] /= d; v[1] /= d; v[2] /= d;
}

void crossProduct(GLfloat a[3], GLfloat b[3], GLfloat r[3]) {
  r[0] = a[1] * b[2] - a[2] * b[1];
  r[1] = a[2] * b[0] - a[0] * b[2];
  r[2] = a[0] * b[1] - a[1] * b[0];
}

GLfloat innerProduct(GLfloat a[3], GLfloat b[3]) {
  return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

GLfloat lenthOfVector(GLfloat v[3]) {
  return sqrt(innerProduct(v, v));
}

void normCrossProd (GLfloat u[3], GLfloat v[3], GLfloat n[3])
{
  n[0] = u[1] * v[2] - u[2] * v[1];
  n[1] = u[2] * v[0] - u[0] * v[2];
  n[2] = u[0] * v[1] - u[1] * v[0];
  normalize(n);
}

void normFace(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3])
{
  GLfloat d1[3], d2[3], n[3];
  int k;
  for (k = 0; k < 3; k++) {
    d1[k] = v1[k] - v2[k];
    d2[k] = v2[k] - v3[k];
  }
  normCrossProd(d1, d2, n);
  glNormal3fv(n);
}

void setVectorValue(GLfloat v[3], GLfloat a, GLfloat b, GLfloat c) {
  v[0] = a;
  v[1] = b;
  v[2] = c;
}

GLfloat det(GLfloat v1[2], GLfloat v2[2]) {
  return GLfloat(v1[0] * v2[1] - v1[1] * v2[0]);
}

GLfloat solveDeterminant(GLfloat a[3], GLfloat b[3], GLfloat c[3], GLfloat r[3], int x) {
  GLfloat ab[3], ac[3], ar[3];
  vector(ab, b, a);
  vector(ac, c, a);
  vector(ar, r, a);

  GLfloat v[2] = {innerProduct(ar, ab), innerProduct(ar, ac)};
  GLfloat vy[2] = {innerProduct(ac, ab), innerProduct(ac, ac)};

  GLfloat vx[2] = {innerProduct(ab, ab), innerProduct(ab, ac)};

  if (x)
    return GLfloat(det(v, vy) / GLfloat(det(vx, vy)));
  else
    return GLfloat(det(vx, v) / GLfloat(det(vx, vy)));
}


/** input: a,b,c -- three points define a triangle
      r -- testing point
    output: return a boolean whether r is in triangle abc
*/
int critirianPointInTriangle(GLfloat a[3], GLfloat b[3], GLfloat c[3], GLfloat r[3]) {
  GLfloat x = solveDeterminant(a, b, c, r, 1);
  GLfloat y = solveDeterminant(a, b, c, r, 0);
  if (x >= 0 && y >= 0 && x + y <= 1) {

    return 1;

  }
  return 0;
}



/** input: n -- normal, v0 -- a point on plane
      p0, p1 -- define a vector
    output: pr -- intersection point

    r = dot(n,(v0-p0))/dot(n,(p1-p0))
    pr = p0+r*(p1-p0)
*/
void pointOnIntersectsPlane(GLfloat n[3], GLfloat v0[3],
                            GLfloat p0[3], GLfloat p1[3], GLfloat pr[3]) {
  GLfloat r;
  GLfloat v1[3], v2[3];
  vector(v1, v0, p0);
  vector(v2, p1, p0);

  r = innerProduct(n, v1) / innerProduct(n, v2);

  pr[0] = r * v2[0] + p0[0];
  pr[1] = r * v2[1] + p0[1];
  pr[2] = r * v2[2] + p0[2];
}

/** eadges of triangle: a,b,c
    endpoint of line: p0,p1
    return: pr--the intersected point*/
void pointOnIntersectsTriangle(GLfloat a[3], GLfloat b[3], GLfloat c[3],
                               GLfloat p0[3], GLfloat p1[3], GLfloat pr[3]) {
  GLfloat r;
  GLfloat e1[3], e2[3], n[3], v1[3], v2[3];
  vector(e1, b, a);
  vector(e2, c, a);

  vector(v1, a, p0);
  vector(v2, p1, p0);

  crossProduct(e1, e2, n);
  r = innerProduct(n, v1) / innerProduct(n, v2);

  pr[0] = r * v2[0] + p0[0];
  pr[1] = r * v2[1] + p0[1];
  pr[2] = r * v2[2] + p0[2];
}

/** sintheta = a x b / (||a|| * ||b||) */
GLfloat sinBetween2Vectors(GLfloat a[3], GLfloat b[3]) {
  GLfloat n[3];
  crossProduct(a, b, n);
  return norm(n) / (norm(a) * norm(b));
}

GLfloat cosBetween2Vectors(GLfloat a[3], GLfloat b[3]) {
  return innerProduct(a, b) / (norm(a) * norm(b));
}

/** s=a+b */
void addVector(GLfloat a[3], GLfloat b[3],  GLfloat s[3]) {
  s[0] = a[0] + b[0];
  s[1] = a[1] + b[1];
  s[2] = a[2] + b[2];
}
/** va=s*v */
void productVectorScaler(GLfloat v[3], GLfloat a, GLfloat va[3]) {
  va[0] = a * v[0];
  va[1] = a * v[1];
  va[2] = a * v[2];
}
/** v1=-v */
void reverseVectorSign(GLfloat v[3], GLfloat v1[3]) {
  v1[0] = -v[0];
  v1[1] = -v[1];
  v1[2] = -v[2];
}
/** cos=sqrt(1-sin**2) and vis versa */
GLfloat computeSinCos(GLfloat a) {
  return sqrt(1 - a * a);
}

/** output: d[3] */
void computeNormalwith3pointsOnPlane(GLfloat a[3], GLfloat b[3], GLfloat c[3], GLfloat d[3]) {
  GLfloat ab[3], ac[3];
  vector(ab, b, a);
  vector(ac, c, a);

  crossProduct(ab, ac, d);
  normalize(d);
}

void drawLineTriangle() {
  GLfloat p0[3] = {0.1, 0.2, -0.7}, p1[3] = {0.1, 0.2, 1}, pr[3];
  GLfloat a[3] = {0, 0, 0}, b[3] = {1, 1, 0}, c[3] = {0, 1, 0};
  glBegin(GL_TRIANGLES);
  glPointSize(2.0);
  glColor4f(1.0f, 0.0f, 0.0f, 1);
  glVertex3fv(a);
  glVertex3fv(b);
  glVertex3fv(c);
  glEnd();

  glBegin(GL_LINES);
  glColor4f(0.7f, 0.7f, 0.0f, 1);
  glVertex3fv(p1);
  glVertex3fv(p0);
  glEnd();

  pointOnIntersectsTriangle(a, b, c, p0, p1, pr);

  glBegin(GL_POINTS);
  glColor3f(1.0f, 1.0f, 1.0f);
  glVertex3fv(pr);
  glEnd();
}


/** input: n--normal of plane, v0 -- a point on the plane
        p0, p1 -- 2 points of the light (striaght line)
        ita -- refraction constant
    output: pl -- direction of refraction vector, which is normalized
    */
void drawRefraction(GLfloat n[3], GLfloat v0[3],
                    GLfloat p0[3], GLfloat p1[3],
                    GLfloat pl[3], GLfloat ita) {

  GLfloat v[3];
  vector(v, p1, p0);
  GLfloat sintheta = sinBetween2Vectors(v, n) * ita;

  GLfloat pr[3], vn[3], t[3];
  //pointOnIntersectsPlane(n,v0,p0,p1,pr);

  normalize(n);
  productVectorScaler(n, innerProduct(n, v), vn);
  vector(t, v, vn);
  normalize(t);

  GLfloat costheta = sqrt(1 - sintheta * sintheta);
  productVectorScaler(t, sintheta, t);
  reverseVectorSign(n, n);
  productVectorScaler(n, costheta, n);
  addVector(t, n, pl);
}

/** input: n--normal of plane, v0 -- a point on the plane
        p0, p1 -- 2 points of the light (striaght line)
        ita -- refraction constant
    output: pl -- direction of refraction vector, which is normalized
    */
void draw2Refraction(GLfloat n[3], GLfloat v0[3],
                     GLfloat p0[3], GLfloat p1[3],
                     GLfloat pl[3], GLfloat ita) {

  GLfloat v[3];
  vector(v, p1, p0);
  GLfloat sintheta = sinBetween2Vectors(v, n) / ita;

  GLfloat pr[3], vn[3], t[3];
  //pointOnIntersectsPlane(n,v0,p0,p1,pr);

  reverseVectorSign(n, n);
  normalize(n);
  productVectorScaler(n, innerProduct(n, v), vn);
  vector(t, v, vn);
  normalize(t);

  GLfloat costheta = sqrt(1 - sintheta * sintheta);
  productVectorScaler(t, sintheta, t);
  reverseVectorSign(n, n);
  productVectorScaler(n, costheta, n);
  addVector(t, n, pl);
}

/** input:  r--radius, o--center point, x-- x-axis of porjecting point
    output: pr -- point on the plane.
*/
void moveOnSemiSphere(GLfloat o[3], GLfloat x, GLfloat z,
                      GLfloat pr[3], GLfloat v0[3]) {

  GLfloat p0[3] = {x, 1, z}, p1[3] = {x, -1, z}, N[3] = {0, 1, 0};

  GLfloat pl[3], n[3], v1[3]; //to find another point on pr vector

  v0[0] = x;
  v0[1] = sqrt(R * R - x * x - z * z);
  v0[2] = z;

  n[0] = v0[0] - o[0];
  n[1] = v0[1] - o[1];
  n[2] = v0[2] - o[2];

  drawRefraction(n, v0, p0, p1, pl, eta);
  addVector(v0, pl, v1);
  pointOnIntersectsPlane(N, o, v0, v1, pr);
}

/** demo: ray pass a sphere*/
void drawDemoRaySemisphere() {
  glPointSize(4.0);

  GLfloat pr[3], v0[3];
  GLfloat p0[3] = {px, 4, pz};
  GLfloat p1[3] = {px, -1, pz};
  GLfloat o[3] = {0, 0, 0};

  if ((px * px + pz * pz) <= R * R) {
    moveOnSemiSphere(o, px, pz, pr, v0);
  } else {
    if (abs(px) > floorSize || abs(pz) > floorSize ) {
      setVectorValue(pr, px, -4, pz);
      setVectorValue(v0, px, -4, pz);
    } else {
      setVectorValue(pr, px, 0, pz);
      setVectorValue(v0, px, 0, pz);
    }
  }

  glBegin(GL_POINTS);
  glColor4f(1.0, 0, 0, 1);
  glVertex3fv(p0);
  glColor4f(1.0, 1.0, 1.0, 1);
  glVertex3fv(v0);
  glColor4f(1.0, 1.0, 0, 1);
  glVertex3fv(pr);
  glEnd();

  glBegin(GL_LINES);
  glColor4f(0.4, 0.4, 1.0, 1);
  glVertex3fv(p0);
  glVertex3fv(v0);
  glColor4f(0.8, 0.5, 1.0, 1);
  glVertex3fv(v0);
  glVertex3fv(pr);
  glEnd();

}

/** demo: ray pass a sphere*/
void drawDemoCircleSemisphere(int n) {
  glPointSize(1.5);
  GLfloat o1[3] = {px, 0, pz};
  GLfloat pr[3], v0[3];

  GLfloat o[3] = {0, 0, 0}; //this is static

  float angle = 2 * PI / n;
  float ax, ay, az;

  for (int i = 0; i < C * n; i++) {
    // ax=(R1+float(i)/float(n*C/4)*sin(2*PI*float(i)/float(n*C)))*cos(angle*float(i));
    // ay=4+2*float(i)/float(n*C/4);
    // az=(R2+float(i)/float(n*C/4)*sin(2*PI*float(i)/float(n*C)))*sin(angle*float(i));
    ax = (float(i) / float(n * C / 4) * sin(2 * PI * float(i) / float(n * C))) * cos(angle * float(i));
    ay = 4 + 2 * float(i) / float(n * C / 4);
    az = (float(i) / float(n * C / 4) * sin(2 * PI * float(i) / float(n * C))) * sin(angle * float(i));

    ax += px;

    az += pz;

    GLfloat p0[3] = {ax, ay, az};
    GLfloat p1[3] = {ax, -1, az};

    if ((ax * ax + az * az) <= R * R) {
      moveOnSemiSphere(o, ax, az, pr, v0);
    } else {
      setVectorValue(pr, ax, 0, az);
      setVectorValue(v0, ax, 0, az);
    }

    glBegin(GL_POINTS);
    glColor3f(1.0, 0, 0);
    glVertex3fv(p0);
    glColor4f(1.0, 1.0, 1.0, 1);
    glVertex3fv(v0);
    glColor4f(1.0, 1.0, 0, 1);
    glVertex3fv(pr);
    glEnd();

    if (linerefraction) {
      glBegin(GL_LINES);
      glColor4f(0.4, 0.4, 1.0, 1);
      glVertex3fv(p0);
      glVertex3fv(v0);
      glColor4f(0.8, 0.5, 1.0, 1);
      glVertex3fv(v0);
      glVertex3fv(pr);
      glEnd();
    }

  }//end of for loop


}

// Draw the floor
void drawFloor(float width, float length, float alpha)
{
  // Begin drawing the floor
  glBegin(GL_QUADS);

  glColor4f(1.0f, 1.0f, 1.0f, alpha);
  glNormal3f(0.0f, 1.0f, 0.0f);
  glVertex3f(-width / 2, 0.0f, length / 2);
  glVertex3f(-width / 2, 0.0f, -length / 2);
  glVertex3f(width / 2, 0.0f, -length / 2);
  glVertex3f(width / 2, 0.0f, length / 2);

  glEnd();
}

