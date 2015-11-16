/**
  usage:

  Keyboard inputs:
  +  increase subdivision depth
  s  flat shading (regular object)
  w  wireless shape
  q  quit

  1  display actually refraction under ellipsoid-- determined by the white circle under that ellipsoid
  2  display an ellipsoid
  3  display actually refraction in ellipsoid   -- yellow point: reflect on surface 
                                                -- middle orange point: refraction view 
                                                -- bottom orange point: actual object position
  4  display a plane (assembly of dots)
    sub-usage: +  increase subdivision of the plane
               -  decrease subdivision of the plane
  5  display refraction view 
  
  8  display ray tracing 
  9  display refraction on a sphere 

8,9 sub-usage: r  shift right
               l  shift left
               b  shift back
               f  shift front
               z  show light trace
  0  light testing

*/
#include "e.h"
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include "tgaload.h"
#define MAX_NO_TEXTURES 4

#define MOUNTAIN_TEXTURE 0
#define CLOUD_TEXTURE 1
#define CHECKBOARD_TEXTURE 2

int current_texture = 2;

GLuint texture_id[MAX_NO_TEXTURES];

#define checkImageWidth 256
#define checkImageHeight 256
static GLubyte checkImage[checkImageHeight][checkImageWidth][4];


/** display options */
int solid = 1; // 0 for wireless; 1 for solid
int subdiv = 1;
int linetriangle = 0;
int rayplane = 0;
int circleplane = 0;
int ellipsoid = 0;
int refraction = 0;
int circleDemo = 0;
int checkboardDemo = 0;
int surfaceRenderDemo = 1;
int drawFloor_b = 0;
int test_light = 0;
int texture_map_index = 2;

GLfloat pu = 0;
GLfloat ita_r = .5; // deviation from objective point
GLfloat ita_l = .7;
GLfloat floor_d = 20;
GLfloat floor_l = 0.5;
GLfloat moveright = 0;


static int n = 300; // # of points rendering in ellipsoid example


GLfloat eye[3] = {1.0, 40, 5.0};

GLfloat checkbo[3] = {1.0, 3.0, 1.5};
GLfloat checkbor = 1.3;
int depthCheckboard = 0;


void makeCheckImage(void)
{
   int i, j, c;
    
   for (i = 0; i < checkImageHeight; i++) {
      for (j = 0; j < checkImageWidth; j++) {
         c = ((((i&0x8)==0)^((j&0x8))==0))*255;
         checkImage[i][j][0] = (GLubyte) c;
         checkImage[i][j][1] = (GLubyte) 100;
         checkImage[i][j][2] = (GLubyte) 100;
         checkImage[i][j][3] = (GLubyte) 255;
      }
   }
}



void drawPlaneWithNormal(GLfloat n[3], GLfloat v0[3]) {
  productVectorScaler(n, 10, n);
  GLfloat v1[3] = {v0[0] - n[1], v0[1] + n[0], v0[2]};
  GLfloat v2[3] = {v0[0] - n[2], v0[1], v0[2] + n[0]};
  GLfloat v3[3] = {v0[0], v0[1] - n[2], v0[2] + n[1]};

  glColor4f(1.0, 0.4, 1.0, 1);
//glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glBegin(GL_TRIANGLES);
  glVertex3fv(v1);
  glVertex3fv(v2);
  glVertex3fv(v3);
  // glVertex3fv(v0);
  glEnd();
}

void translateCoodinatewith_floor_d(GLfloat X_f, GLfloat Z_f, GLfloat *x, GLfloat *z) {
  *x = (X_f + floor_d) / (2 * floor_d);
  *z = -(Z_f - floor_d) / (2 * floor_d);
}
/** input n -- normal, v0 -- a point on plane
    P -- object point
    output P1 --projection point on the plane
*/
void projectionOnPlane(GLfloat n[3], GLfloat V0[3], GLfloat P[3], GLfloat P1[3]) {
  GLfloat V0P[3];
  vector(V0P, P, V0);
  normalize(n);
  productVectorScaler(n, innerProduct(V0P, n) , V0P);
  vector(P1, P, V0P);
}

/** input: n -- normal, v0 -- a point on plane
      p0, p1 -- define a vector
    output: pr -- intersection point

    r = dot(n,(v0-p0))/dot(n,(p1-p0))
    pr = p0+r*(p1-p0)
*/
GLfloat pointOnIntersectsPlaneEllipsoid(GLfloat n[3], GLfloat v0[3],
                                        GLfloat p0[3], GLfloat p1[3], GLfloat pr[3]) {
  GLfloat r;
  GLfloat v1[3], v2[3];
  vector(v1, v0, p0);
  vector(v2, p1, p0);

  r = innerProduct(n, v1) / innerProduct(n, v2);

  pr[0] = r * v2[0] + p0[0];
  pr[1] = r * v2[1] + p0[1];
  pr[2] = r * v2[2] + p0[2];

  return r;
}

/** ==========following part showing Ray-Tracing ============= */

/** input n -- normal, v0 -- a point on plane
    E -- eye, P -- object point
    output P1 -- refraction point
*/
void renderRefraction(GLfloat n[3], GLfloat V0[3], GLfloat E[3], GLfloat P[3], GLfloat P1[3]) {
  GLfloat R[3];
  GLfloat P0[3];
  GLfloat v_temp[3];
  GLfloat v1_temp[3];
  GLfloat v2_temp[3];
  normalize(n);
  GLfloat r = pointOnIntersectsPlaneEllipsoid(n, V0, E, P, R);
  if (r < 0 || r > 1)
    return;
  projectionOnPlane(n, V0, P, P0);

  vector(v_temp, P0, R);
  productVectorScaler(v_temp, ita_r , v_temp);
  addVector(v_temp, R, v_temp);

  //glBegin(GL_POINTS);
  //glColor3f(1.0,1.0,0);
  // glVertex3fv(R);
  //glColor3f(1.0,1.0,0.0);
  //glVertex3fv(E);
  // glColor3f(0.0,1.0,1.0);
  // glVertex3fv(P0);
  // glColor3f(0.5,0.7,.9);
  // glVertex3fv(v_temp);
  //glEnd();

  vector(v1_temp, v_temp, E);
  normalize(v1_temp);
  vector(v2_temp, P, R);

  GLfloat constantd = ita_l * lenthOfVector(v2_temp);
  reverseVectorSign(v2_temp, v2_temp);
  constantd *= cosBetween2Vectors(n, v2_temp);
  productVectorScaler(v1_temp, constantd , v1_temp);
  addVector(v_temp, v1_temp, P1);

}

/** input n -- normal, v0 -- a point on plane
    E -- eye, P -- object point
    output R -- refraction point
*/
void renderHalfRefraction(GLfloat n[3], GLfloat V0[3], GLfloat E[3], GLfloat P[3], GLfloat P3[3]) {
  GLfloat R[3];
  GLfloat P0[3];
  GLfloat v_temp[3];
  GLfloat v1_temp[3];
  GLfloat v2_temp[3];
  normalize(n);
  GLfloat r = pointOnIntersectsPlaneEllipsoid(n, V0, E, P, R);
  if (r < 0 || r > 1)
    return;
  projectionOnPlane(n, V0, P, P0);

  vector(v_temp, P0, R);
  productVectorScaler(v_temp, ita_r , v_temp);
  addVector(v_temp, R, P3);

  //glBegin(GL_POINTS);
  //glColor3f(1.0,1.0,0);
  // glVertex3fv(R);
  //glColor3f(1.0,1.0,0.0);
  //glVertex3fv(E);
  // glColor3f(0.0,1.0,1.0);
  // glVertex3fv(P0);
  // glColor3f(0.5,0.7,.9);
  // glVertex3fv(v_temp);
  //glEnd();

  // vector(v1_temp, v_temp, E);
  // normalize(v1_temp);
  // vector(v2_temp, P,R);
  // productVectorScaler(v1_temp, lenthOfVector(v2_temp), v1_temp);
  // addVector(v_temp, v1_temp, P1);

}


void refractionPlaneDome() {

  float n = 150;
  glPointSize(1.5);
  GLfloat n1[3] = {0, 1, 0}; n1[2] += pu;
  GLfloat v1[3] = {0, 3, 0};
  //drawPlaneWithNormal(n1,v1);
  GLfloat pr[3], v0[3];

// GLfloat o[3]={0,0,0}; //this is static

  float angle = 2 * PI / n;
  float ax, ay, az;

  for (int i = 0; i < C * n; i++) {
    // ax=(R1+float(i)/float(n*C/4)*sin(2*PI*float(i)/float(n*C)))*cos(angle*float(i));
    // ay=2+(2*float(i)/float(n*C/4))*.3;
    // az=(R2+float(i)/float(n*C/4)*sin(2*PI*float(i)/float(n*C)))*sin(angle*float(i));
    ax = 2 * cos(angle * float(i));
    ay = 1;
    az = 2 * sin(angle * float(i));

    ax += px;
    az += pz;

    GLfloat p0[3] = {ax, ay, az};

    renderRefraction(n1, v1, eye, p0, v0);

    glBegin(GL_POINTS);
    glColor4f(1.0, 0, 0, 1);
    glVertex3fv(p0);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glVertex3fv(v0);
    // glColor3f(1.0,1.0,0);
    // glVertex3fv(pr);
    glEnd();

    if (linerefraction) {
      glBegin(GL_LINES);
      glColor4f(0.4, 0.4, 1.0, 1);
      glVertex3fv(p0);
      //  glVertex3fv(v0);
      glColor4f(0.8, 0.5, 1.0, 1);
      glVertex3fv(v0);
      //  glVertex3fv(pr);
      glEnd();
    }

  }//end of for loop


}

/** =======start to produce a ellipsoid demo here========= **/
void drawPointOnTriangle(GLfloat a[3], GLfloat b[3], GLfloat c[3], GLfloat e[3], GLfloat p[3]) {
  GLfloat r[3];
  GLfloat ab[3], ac[3], cross[3]; GLfloat p_refraction[3];
  vector(ab, b, a);
  vector(ac, c, a);

  crossProduct(ab, ac, cross);
  renderHalfRefraction(cross, a, e, p, r);

  if (critirianPointInTriangle(a, b, c, r) == 1) {
    //draw
    glBegin(GL_POINTS);
    glColor4f(1.0, 1.0, 0, 1);
    glVertex3fv(r);
    glEnd();

    renderRefraction(cross, a, e, p, p_refraction);

    glBegin(GL_POINTS);
    glColor4f(0.9, .7, 0.4, 1.0);
    glVertex3fv(p_refraction);
    glEnd();

  }
}

void drawCircleOnTirangleWithRefraction(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3]) {
  float angle = 2 * PI / n;
  float ax, ay, az;

  for (int i = 0; i < C * n; i++) {
    ax = 1 * cos(angle * float(i));
    ay = 3;
    az = 1 * sin(angle * float(i));

    ax += px;
    az += pz;
    GLfloat p0[3] = {ax, ay, az};

    glBegin(GL_POINTS);
    glColor4f(1.0, .7, 0.3, 1.0);
    glVertex3fv(p0);
    glEnd();

    drawPointOnTriangle(v1, v2, v3, eye, p0);
  }
}

void drawsubdivideCheckboardSquare(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3], GLfloat v4[3],
                                   GLfloat a[3], GLfloat b[3], GLfloat c[3], int depth) {
  GLfloat v12[3], v23[3], v34[3], v41[3], v00[3];
  int i;
  if (depth == 0) {

    glBegin(GL_POINTS);
    glColor4f(.6, 1.0, .8, 1);
    glVertex3fv(v1);
    glVertex3fv(v2);
    glVertex3fv(v3);
    glVertex3fv(v4);
    glEnd();

    drawPointOnTriangle(a, b, c, eye, v1);
    drawPointOnTriangle(a, b, c, eye, v2);
    drawPointOnTriangle(a, b, c, eye, v3);
    drawPointOnTriangle(a, b, c, eye, v4);

    return;
  }
  for (i = 0; i < 3; i++) {
    v12[i] = (v1[i] + v2[i]) / 2.0;
    v23[i] = (v2[i] + v3[i]) / 2.0;
    v34[i] = (v3[i] + v4[i]) / 2.0;
    v41[i] = (v1[i] + v4[i]) / 2.0;
    v00[i] = (v41[i] + v23[i]) / 2.0;
  }
  /* recursively subdivide new triangles */
  drawsubdivideCheckboardSquare(v1, v12, v00, v41, a, b, c, depth - 1);
  drawsubdivideCheckboardSquare(v2, v23, v00, v12, a, b, c, depth - 1);
  drawsubdivideCheckboardSquare(v3, v34, v00, v23, a, b, c, depth - 1);
  drawsubdivideCheckboardSquare(v4, v41, v00, v34, a, b, c, depth - 1);
}

void drawSurfaceRenderWithTexture(GLfloat a[3], GLfloat b[3], GLfloat c[3],
                                  GLfloat e[3], GLfloat l) {
  GLfloat Normal[3], pl_a[3], pl_b[3], pl_c[3];
  computeNormalwith3pointsOnPlane(a, b, c, Normal);
  draw2Refraction(Normal, a, e, a, pl_a, eta);
  draw2Refraction(Normal, a, e, b, pl_b, eta);
  draw2Refraction(Normal, a, e, c, pl_c, eta);
// printVector(pl_a);

  GLfloat np[3] = {0, 1, 0}, v0[3] = {np[0], l, np[2]};
  GLfloat pl2_a[3], pl2_b[3], pl2_c[3], p_intersect_a[3], p_intersect_b[3], p_intersect_c[3];

  addVector(pl_a, a, pl2_a);
  pointOnIntersectsPlane(np, v0, a, pl2_a, p_intersect_a);
  // printf("p_intersect_a is:");
  // printVector(p_intersect_a);

  addVector(pl_b, b, pl2_b);
  pointOnIntersectsPlane(np, v0, b, pl2_b, p_intersect_b);
  // printf("p_intersect_b is: ");
  // printVector(p_intersect_b);

  addVector(pl_c, c, pl2_c);
  pointOnIntersectsPlane(np, v0, c, pl2_c, p_intersect_c);
  // printf("p_intersect_c is: ");
  // printVector(p_intersect_c);

  GLfloat xa, xb, xc, za, zb, zc;
  translateCoodinatewith_floor_d(p_intersect_a[0], p_intersect_a[2], &xa, &za);
  translateCoodinatewith_floor_d(p_intersect_b[0], p_intersect_b[2], &xb, &zb);
  translateCoodinatewith_floor_d(p_intersect_c[0], p_intersect_c[2], &xc, &zc);

  // printf("xa is: %f ", xa);
  // printf("xb is: %f ", xb);
  // printf("xc is: %f\n", xc);
  if (xa > 0 && xa < 1 && xb > 0 && xb < 1 && xc > 0 && xc < 1
      && za > 0 && za < 1 && zb > 0 && zb < 1 && zc > 0 && zc < 1) {
    glBindTexture ( GL_TEXTURE_2D, texture_id[current_texture] );
    glBegin ( GL_TRIANGLES);
    glTexCoord2f(xa, za); glVertex3fv(a);
    glTexCoord2f(xb, zb); glVertex3fv(b);
    glTexCoord2f(xc, zc); glVertex3f(c[0], c[1], c[2]);
    glEnd();
    // printf("fit.\n");

  }
  else
    drawTriangleFlat(a, b, c);

}

/* recursively subdivide face `depth' times */
/* and draw the resulting triangles */
void subdivide(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3], int depth)
{
  GLfloat v12[3], v23[3], v31[3];
  int i;

  if (depth == 0) {
    glColor4f(.6, .9, 1.0, 1.0);
    //  drawTriangleFlat(v1, v2, v3);

    if (circleDemo) {
      drawTriangleFlat(v1, v2, v3);
      drawCircleOnTirangleWithRefraction(v1, v2, v3);
    }
    if (checkboardDemo) {
      drawTriangleFlat(v1, v2, v3);
      GLfloat a[3] = {checkbo[0] + checkbor, checkbo[1], checkbo[2] + checkbor};
      GLfloat b[3] = {checkbo[0] - checkbor, checkbo[1], checkbo[2] + checkbor};
      GLfloat c[3] = {checkbo[0] - checkbor, checkbo[1], checkbo[2] - checkbor};
      GLfloat d[3] = {checkbo[0] + checkbor, checkbo[1], checkbo[2] - checkbor};

      drawsubdivideCheckboardSquare(a, b, c, d, v1, v2, v3, depthCheckboard);
    }

    if (surfaceRenderDemo) {
      drawSurfaceRenderWithTexture(v1, v2, v3, eye, floor_l);
    }
    return;
  }

  /* calculate midpoints of each side */
  for (i = 0; i < 3; i++) {
    v12[i] = (v1[i] + v2[i]) / 2.0;
    v23[i] = (v2[i] + v3[i]) / 2.0;
    v31[i] = (v3[i] + v1[i]) / 2.0;
  }
  /* extrude midpoints to lie on unit sphere */
  normalizeEllipsoid(v12);
  normalizeEllipsoid(v23);
  normalizeEllipsoid(v31);

  /* recursively subdivide new triangles */
  subdivide(v1, v12, v31, depth - 1);
  subdivide(v2, v23, v12, depth - 1);
  subdivide(v3, v31, v23, depth - 1);
  subdivide(v12, v23, v31, depth - 1);
}

/** to reduce calculation in Ecllipse sureface rendering example
    input: 3 points of triangle--v1,v2,v3
        a -- oringinal point on the line, pl -- direction
    output: Rr -- intersection point
    return 1 for intersected; 0 for not intersected with trianlge
*/
int lineIntersectTriangle(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3],
                          GLfloat a[3], GLfloat pl[3], GLfloat Rr[3]) {
  GLfloat n[3], R[3];
  computeNormalwith3pointsOnPlane(v1, v2, v3, n);
  addVector(pl, a, R);
  GLfloat r = pointOnIntersectsPlaneEllipsoid(n, v1, a, R, Rr);
  if (r < 0 || r > 1)
    return 0;
  else return 1;
}

/* recursively subdivide face `depth' times */
/* and draw the resulting triangles */
void subdivide2R(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3], int depth,
                 GLfloat a[3], GLfloat b[3], GLfloat c[3])
{
  GLfloat v12[3], v23[3], v31[3];
  int i;

  if (depth == 0) {
    glColor4f(.6, .9, 1.0, 1.0);
    //  drawTriangleFlat(v1, v2, v3);

    //TODO here.
    return;
  }

  /* calculate midpoints of each side */
  for (i = 0; i < 3; i++) {
    v12[i] = (v1[i] + v2[i]) / 2.0;
    v23[i] = (v2[i] + v3[i]) / 2.0;
    v31[i] = (v3[i] + v1[i]) / 2.0;
  }
  /* extrude midpoints to lie on unit sphere */
  normalizeEllipsoid(v12);
  normalizeEllipsoid(v23);
  normalizeEllipsoid(v31);

  /* recursively subdivide new triangles */

  GLfloat Rr[3], pl[3];
  if (lineIntersectTriangle(v1, v12, v31, a, pl, Rr) == 1)
    subdivide(v1, v12, v31, depth - 1);
  if (lineIntersectTriangle(v2, v23, v12, a, pl, Rr) == 1)
    subdivide(v2, v23, v12, depth - 1);
  if (lineIntersectTriangle(v3, v31, v23, a, pl, Rr) == 1)
    subdivide(v3, v31, v23, depth - 1);
  if (lineIntersectTriangle(v12, v23, v31, a, pl, Rr) == 1)
    subdivide(v12, v23, v31, depth - 1);
}


void subdividetoFind2RefractionPoint(GLfloat a[3], GLfloat b[3], GLfloat c[3]) {
  for (int i = 0; i < 20; i++) {
    subdivide2R(&vdata[tindices[i][0]][0],
                &vdata[tindices[i][1]][0],
                &vdata[tindices[i][2]][0],
                subdiv,
                a, b, c);
  }
}


void displayElipsoidDemo() {
  for (int i = 0; i < 20; i++) {
    subdivide(&vdata[tindices[i][0]][0],
              &vdata[tindices[i][1]][0],
              &vdata[tindices[i][2]][0],
              subdiv);
  }
}


/* display */
void Display(void)
{
  if (solid) {
    glEnable(GL_LIGHTING);   // Draw polygon interiors with lighting enabled.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // Draw polygon interiors.

    glEnable(GL_POLYGON_OFFSET_FILL);  // Turn on offset for filled polygons.
    glPolygonOffset(1, 1);     // Set polygon offset amount.

  } else {
    glDisable(GL_LIGHTING);  // Draw polygon outlines with lighting disabled.
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Draw polygon outlines.

    glColor4f(1.0, 1.0, .5, 1); // Draw the outlines in black.
  }
  //clean the scene
  glClearColor(0.1, 0.1, 0.1, 1.0); //set background
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //set matrix mode to GL_MODELVIEW
  glMatrixMode(GL_MODELVIEW);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  glColor4f(1.0f, 0.4f, 0.0f, 1.0f);



  glPushMatrix();
  glRotatef(rotation_x, 0.0, 1.0, 0.0);
  glRotatef(rotation_y, 1.0, 0.0, 0.0);
  if (drawFloor_b) {
    drawFloor(7.0f, 7.0f, 0.8f);
  }
  if (test_light) {
    glutSolidSphere(3.0, 16, 16);
  }
  if (ellipsoid) {
    glBegin(GL_POINTS);
    glColor4f(1.0, 1, 0, 1);
    glVertex3fv(eye);
    glEnd();
    displayElipsoidDemo();
  }

  if (refraction) {
    refractionPlaneDome();
  }

  if (surfaceRenderDemo) {
    eye[1] += moveright;
    displayElipsoidDemo();
    //   GLfloat a[3]={1,1,0}, b[3]={0,1,2}, c[3]={0,1,0};
//    drawSurfaceRenderWithTexture(a,b,c,eye,0);
    glBindTexture ( GL_TEXTURE_2D, texture_id[current_texture] );
    glBegin ( GL_QUADS);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( floor_d, floor_l,  -floor_d);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( -floor_d, floor_l,  -floor_d);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( -floor_d,  floor_l,  floor_d);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( floor_d,  floor_l, floor_d);
    glEnd();
    glutPostRedisplay();

    // glBegin ( GL_TRIANGLES);
    // glTexCoord2f(1.0f, .5f); glVertex3f( floor_d, floor_l-3,  -floor_d);
    // glTexCoord2f(.7f, .5f); glVertex3f( -floor_d, floor_l-3,  -floor_d);

    // glTexCoord2f(1.0f, 0.0f); glVertex3f( floor_d,  floor_l-3, floor_d);
    // glEnd();

    // glutSwapBuffers();
  }
  /** #p: ray plane basic refraction demo */
  if (rayplane) {
    glPushMatrix();
    //glLoadIdentity();
    glScalef(1.0f, 1.0f, 1.0f);
    glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
    glutWireSphere(R, 20, 20);
    drawDemoRaySemisphere();
    glPopMatrix();
  } /** #p ends */

  /** #o circle plane basic refraction demo*/
  if (circleplane) {
    glPushMatrix();
    //glLoadIdentity();
    glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
    glutWireSphere(R, 20, 20);
    drawDemoCircleSemisphere(numberOfDots);
    glPopMatrix();
  }

  glPopMatrix();


  glutSwapBuffers();
};


void Mouse(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    mouse_x = (float) x;
    mouse_y = (float) y;


  }
}

// Call-back function glutMouseFunc();
void MouseMotion(int x, int y)
{
  rotation_x += (float) x - mouse_x;
  rotation_y += (float) y - mouse_y;

  mouse_x = (float) x;
  mouse_y = (float) y;

  glutPostRedisplay(); // for redraw the current mouse position;
};

void keyboard(unsigned char key, int x, int y)
{
  if (key == 'q') exit(0);
  if (key == 'w') solid = 0;
  if (key == 's') solid = 1;


  if (key == '1') {
    if (refraction == 0) refraction = 1;
    else if (refraction == 1) refraction = 0;
  }
  if (key == '2') {
    if (ellipsoid == 0) ellipsoid = 1;
    else if (ellipsoid == 1) ellipsoid = 0;
  }
  if (key == '3') {
    if (circleDemo == 0) circleDemo = 1;
    else if (circleDemo == 1) circleDemo = 0;
  }
  if (key == '4') {
    if (checkboardDemo == 0) checkboardDemo = 1;
    else if (checkboardDemo == 1) checkboardDemo = 0;
  }

  if (key == '5') {
    if (surfaceRenderDemo == 0) surfaceRenderDemo = 1;
    else if (surfaceRenderDemo == 1) surfaceRenderDemo = 0;
  }
  if (key == '8') {
    if (rayplane == 0) rayplane = 1;
    else if (rayplane == 1) rayplane = 0;
  }

  if (key == '9') {
    if (circleplane == 0) circleplane = 1;
    else if (circleplane == 1) circleplane = 0;
  }
  if (key == '0') {
    if (test_light == 0) test_light = 1;
    else if (test_light == 1) test_light = 0;
  }

  if (key == 'z') {
    if (linerefraction == 0) linerefraction = 1;
    else if (linerefraction == 1) linerefraction = 0;
  }

  if (key == 'F') {
    if (drawFloor_b == 0) drawFloor_b = 1;
    else if (drawFloor_b == 1) drawFloor_b = 0;
  }

  if (key == 'u') pu += .05; //plane refraction demo plane tilt up
  if (key == 'U') pu -= .05;

  if (key == 'r') px += .05; //left
  if (key == 'l') px -= .05; //right
  if (key == 'f') pz += .05; //forward
  if (key == 'b') pz -= .05; //backward

  if (key == '+') subdiv++;
  if (key == '-') subdiv--;
  if (subdiv < 0) subdiv = 0;
  if (key == '>') depthCheckboard++;
  if (key == '<') depthCheckboard--;
  if (depthCheckboard < 0) depthCheckboard = 0;


  glutPostRedisplay();
}

void arrow_keys ( int a_keys, int x, int y )  // Create Special Function (required for arrow keys)
{
  switch ( a_keys ) {
  case GLUT_KEY_LEFT:
    current_texture = --texture_map_index  % MAX_NO_TEXTURES;//% MAX_NO_TEXTURES ;
    break;
  case GLUT_KEY_RIGHT:
    current_texture = ++texture_map_index % MAX_NO_TEXTURES ;
    break;
  default:
    break;
  }
}

// Call-back function for windows size changed;
void WindowSizeReShape(int w, int h)
{
  win_width = w;
  win_height = h;

  //update the viewport;
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);

  //update the projection matrix;
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, (float)win_width / win_height, 0.01, 50);
  gluLookAt(0.0, 2.0, 15.0, //eye
            0.0, 2.5, 0.0, //center
            0.0, 1.0, 0.0); //upY

  glMatrixMode(GL_MODELVIEW);
}

void init()
{
  glClearColor(0.0, 0.0, 0.0, 1.0);

  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
  glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);

  glMaterialfv(GL_LIGHT0, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialf(GL_LIGHT0, GL_SHININESS, mat_shininess);
  glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);

  glShadeModel(GL_SMOOTH);  /* enable smooth shading */
  glEnable(GL_LIGHTING);    /* enable lighting */
  glEnable(GL_LIGHT0);      /* enable light 0 */

  glEnable(GL_DEPTH_TEST);

// following are texture mapping part
 // glClearColor(0.0f, 0.0f, 0.0f, 0.5f);        // Black Background
  glEnable ( GL_COLOR_MATERIAL );
  glColorMaterial ( GL_LIGHT0, GL_AMBIENT_AND_DIFFUSE );


  glEnable ( GL_TEXTURE_2D );
  glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );
 // glGenTextures (1, texture_id);

  image_t   temp_image;


  glBindTexture ( GL_TEXTURE_2D, texture_id[MOUNTAIN_TEXTURE] );
  tgaLoad  ( "mountains-hero.tga", &temp_image, TGA_FREE | TGA_LOW_QUALITY );


//try to load a second picture

  glPixelStorei ( GL_UNPACK_ALIGNMENT, 2);
 // glGenTextures (2, texture_id);

  image_t   temp_image2;

  glBindTexture ( GL_TEXTURE_2D, texture_id[CLOUD_TEXTURE] );
  tgaLoad  ( "atmosphere_clouds.tga", &temp_image2, TGA_FREE | TGA_LOW_QUALITY );

  //try to load a third texture
  glPixelStorei ( GL_UNPACK_ALIGNMENT, 3 );
  glGenTextures (3, texture_id);

  image_t   temp_image3;


  glBindTexture ( GL_TEXTURE_2D, texture_id[CHECKBOARD_TEXTURE] );
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                                           GL_LINEAR_MIPMAP_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
  tgaLoad  ( "color_checkboard.tga", &temp_image3, TGA_FREE | TGA_LOW_QUALITY );


// a flat image
   glShadeModel(GL_FLAT);

   makeCheckImage();
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   glGenTextures(4, &texture_id[3]);
   glBindTexture(GL_TEXTURE_2D, texture_id[3]);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
                   GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
                   GL_NEAREST);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth, 
                checkImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
                checkImage);

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glEnable(GL_TEXTURE_2D);
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
   glBindTexture(GL_TEXTURE_2D, texture_id[3]);

 

  glEnable ( GL_CULL_FACE );


  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

/* MAIN */
int main(int argc, char* argv[])
{
  //step 1. initialize GLUT
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE);
  glutInitWindowSize(win_width, win_height);
  glutInitWindowPosition(100, 100);

  //step 2. Open a window named "GLUT DEMO"
  win = glutCreateWindow("Reflection");

  //misc
  glutSetCursor(GLUT_CURSOR_CROSSHAIR);

  init();


  //step 3. register the display function
  glutDisplayFunc(Display);
  glutMouseFunc(Mouse);
  glutMotionFunc(MouseMotion);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc( arrow_keys );
  glutReshapeFunc(WindowSizeReShape);

  //step 4, initialization of opengl statues
  glEnable(GL_DEPTH_TEST);

  //step 5. enter the event processing loop
  glutMainLoop();
  return 0;
}