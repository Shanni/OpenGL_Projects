
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
static GLfloat eta(.5);

/** for sphere-spiral-curve */
static int C(3);
int linerefraction = 0;

int solid = 0; // 0 for wireless; 1 for solid
int linetriangle = 0;
int rayplane = 1;
int circleplane = 1;

GLfloat mat_specular[] = {0.0, 0.0, 0.0, 1.0};
GLfloat mat_diffuse[]  = {0.8, 0.6, 0.4, 1.0};
GLfloat mat_ambient[]  = {0.8, 0.6, 0.4, 1.0};
GLfloat mat_shininess = 100.0;  /* unused if specular is 0 */

GLfloat light_ambient[]  = {0.2, 0.2, 0.2, 1.0};
GLfloat light_diffuse[]  = {1.0, 1.0, 1.0, 1.0};
GLfloat light_specular[] = {0.0, 0.0, 0.0, 1.0};

GLfloat light_position[] = {1.8, 1.2, -2.0, 0.0}; /* directional */

/* a = b - c */
#define vector(a,b,c) \
(a)[0] = (b)[0] - (c)[0]; \
(a)[1] = (b)[1] - (c)[1]; \
(a)[2] = (b)[2] - (c)[2];

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

void drawLineTriangle() {
  GLfloat p0[3] = {0.1, 0.2, -0.7}, p1[3] = {0.1, 0.2, 1}, pr[3];
  GLfloat a[3] = {0, 0, 0}, b[3] = {1, 1, 0}, c[3] = {0, 1, 0};
  glBegin(GL_TRIANGLES);
  glPointSize(2.0);
  glColor3f(1.0f, 0.0f, 0.0f);
  glVertex3fv(a);
  glVertex3fv(b);
  glVertex3fv(c);
  glEnd();

  glBegin(GL_LINES);
  glColor3f(0.7f, 0.7f, 0.0f);
  glVertex3fv(p1);
  glVertex3fv(p0);
  glEnd();

  pointOnIntersectsTriangle(a, b, c, p0, p1, pr);

  glBegin(GL_POINTS);
  glColor3f(1.0f, 1.0f, 1.0f);
  glVertex3fv(pr);
  glEnd();
}

/** n--normal of plane
    TODO: details
    */
void drawRefraction(GLfloat n[3], GLfloat v0[3],
                    GLfloat p0[3], GLfloat p1[3],
                    GLfloat pl[3], GLfloat ita) {

  GLfloat v[3];
  vector(v, p1, p0); //
  GLfloat sintheta = sinBetween2Vectors(v, n) * ita;

  GLfloat pr[3], vn[3], t[3];
  pointOnIntersectsPlane(n, v0, p0, p1, pr);

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
  glColor3f(1.0, 0, 0);
  glVertex3fv(p0);
  glColor3f(1.0, 1.0, 1.0);
  glVertex3fv(v0);
  glColor3f(1.0, 1.0, 0);
  glVertex3fv(pr);
  glEnd();

  glBegin(GL_LINES);
  glColor3f(0.4, 0.4, 1.0);
  glVertex3fv(p0);
  glVertex3fv(v0);
  glColor3f(0.8, 0.5, 1.0);
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
    glColor3f(1.0, 1.0, 1.0);
    glVertex3fv(v0);
    glColor3f(1.0, 1.0, 0);
    glVertex3fv(pr);
    glEnd();

    if (linerefraction) {
      glBegin(GL_LINES);
      glColor3f(0.4, 0.4, 1.0);
      glVertex3fv(p0);
      glVertex3fv(v0);
      glColor3f(0.8, 0.5, 1.0);
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

    glColor3f(1.0, 1.0, 0.5); // Draw the outlines in black.
  }
  //clean the scene
  glClearColor(0.3, 0.3, 0.3, 1.0); //set background
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //set matrix mode to GL_MODELVIEW
  glMatrixMode(GL_MODELVIEW);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  glColor3f(1.0f, 0.4f, 0.0f);

  /** #d: display line-triangle intersection*/
  glPushMatrix();
  glLoadIdentity();
  glRotatef(rotation_x, 0.0, 1.0, 0.0);
  glRotatef(rotation_y, 1.0, 0.0, 0.0);
  drawFloor(7.0f, 7.0f, 0.8f);
  if (linetriangle) //if desplay line intersection:
    drawLineTriangle();
  glPopMatrix();
  /** #d ends */

  glPushMatrix();
  glRotatef(rotation_x, 0.0, 1.0, 0.0);
  glRotatef(rotation_y, 1.0, 0.0, 0.0);
  /** #p: ray plane basic refraction demo */
  if (rayplane) {
    glPushMatrix();
    //glLoadIdentity();
    glScalef(1.0f, 1.0f, 1.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    glutWireSphere(R, 20, 20);
    drawDemoRaySemisphere();
    glPopMatrix();
  } /** #p ends */

  /** #o circle plane basic refraction demo*/
  if (circleplane) {
    glPushMatrix();
    //glLoadIdentity();
    glColor3f(0.0f, 0.0f, 0.0f);
    glutWireSphere(R, 20, 20);
    drawDemoCircleSemisphere(numberOfDots);
    glPopMatrix();
  }

  glPopMatrix();


  glutSwapBuffers();
};

// bool is_selected(float mouse_x_pos, float mouse_y_pos, float v[3])
//     {
//         if(mouse_x_pos >= (x-range) && mouse_y_pos >= (y-range) &&
//            mouse_x_pos <+ (x+range) && mouse_y_pos >= (y-range)){
//             return true;
//         }else{
//             return false;
//         }
//     }

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
  if (key == 'd') {
    if (linetriangle == 0) linetriangle = 1;
    else if (linetriangle == 1) linetriangle = 0;
  }

  if (key == 'p') {
    if (rayplane == 0) rayplane = 1;
    else if (rayplane == 1) rayplane = 0;
  }

  if (key == 'o') {
    if (circleplane == 0) circleplane = 1;
    else if (circleplane == 1) circleplane = 0;
  }
  if (key == 'z') {
    if (linerefraction == 0) linerefraction = 1;
    else if (linerefraction == 1) linerefraction = 0;
  }

  if (key == 'r') px += .05; //left
  if (key == 'l') px -= .05; //right
  if (key == 'f') pz += .05; //forward
  if (key == 'b') pz -= .05; //backward


  glutPostRedisplay();
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
            0.0, 4.0, 0.0, //center
            0.0, 1.0, 0.0); //upY

  glMatrixMode(GL_MODELVIEW);
}

void init()
{
  glClearColor(0.0, 0.0, 0.0, 1.0);

  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialf(GL_LIGHT0, GL_SHININESS, mat_shininess);

  glShadeModel(GL_SMOOTH);  /* enable smooth shading */
// glEnable(GL_LIGHTING);    /* enable lighting */
  glEnable(GL_LIGHT0);      /* enable light 0 */

  glEnable(GL_DEPTH_TEST);
}

/* MAIN */
int main(int argc, char* argv[])
{
  //step 1. initialize GLUT
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
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
  glutReshapeFunc(WindowSizeReShape);

  //step 4, initialization of opengl statues
  glEnable(GL_DEPTH_TEST);

  //step 5. enter the event processing loop
  glutMainLoop();
  return 0;
}