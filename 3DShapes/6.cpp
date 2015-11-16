// http://www3.cs.stonybrook.edu/~shhan/cse328/hw_two/glut_3d_rotation.cc

#include <stdio.h>
#include <stdlib.h>
#include <GLUT/glut.h>
#include <math.h>


#define M_PI 3.1415926

/* global variables */
static int win(0); // GLUT window ID
static int win_width(600), win_height(600); // window's width and height
static float mouse_x(0.0), mouse_y(0.0);
static float rotation_x(0.0),rotation_y(0.0);

int solid=0; // 0 for wireless; 1 for solid

GLfloat mat_specular[] = {0.0, 0.0, 0.0, 1.0};
GLfloat mat_diffuse[]  = {0.8, 0.6, 0.4, 1.0};
GLfloat mat_ambient[]  = {0.8, 0.6, 0.4, 1.0};
GLfloat mat_shininess = 100.0;  /* unused if specular is 0 */

GLfloat light_ambient[]  = {0.2, 0.2, 0.2, 1.0};
GLfloat light_diffuse[]  = {1.0, 1.0, 1.0, 1.0};
GLfloat light_specular[] = {0.0, 0.0, 0.0, 1.0};

GLfloat light_position[] = {1.8, 1.2, -2.0, 0.0}; /* directional */


static void
doughnut(GLfloat r, GLfloat R, GLint nsides, GLint rings)
{
  int i, j;
  GLfloat theta, phi, theta1;
  GLfloat cosTheta, sinTheta;
  GLfloat cosTheta1, sinTheta1;
  GLfloat ringDelta, sideDelta;

  ringDelta = 2.0 * M_PI / rings;
  sideDelta = 2.0 * M_PI / nsides;

  theta = 0.0;
  cosTheta = 1.0;
  sinTheta = 0.0;
  for (i = rings - 1; i >= 0; i--) {
    theta1 = theta + ringDelta;
    cosTheta1 = cos(theta1);
    sinTheta1 = sin(theta1);
    glBegin(GL_QUAD_STRIP);
    phi = 0.0;
    for (j = nsides; j >= 0; j--) {
      GLfloat cosPhi, sinPhi, dist;

      phi += sideDelta;
      cosPhi = cos(phi);
      sinPhi = sin(phi);
      dist = R + r * cosPhi;

      glNormal3f(cosTheta1 * cosPhi, -sinTheta1 * cosPhi, sinPhi);
      glVertex3f(cosTheta1 * dist, -sinTheta1 * dist, r * sinPhi);
      glNormal3f(cosTheta * cosPhi, -sinTheta * cosPhi, sinPhi);
      glVertex3f(cosTheta * dist, -sinTheta * dist,  r * sinPhi);
    }
    glEnd();
    theta = theta1;
    cosTheta = cosTheta1;
    sinTheta = sinTheta1;
  }
}

void Mouse(int button, int state, int x, int y)
{
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
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
  if (key=='q') exit(0);
  if (key=='w') solid=0;
  if (key=='s') solid=1;

  glutPostRedisplay();
}

// Call-back function for windows size changed;
void WindowSizeReShape(int w, int h)
{
    win_width = w;
    win_height = h;

    //update the viewport;
    glViewport(0,0,(GLsizei)w,(GLsizei)h);

    //update the projection matrix;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0,(float)win_width/win_height,0.01,50);
	gluLookAt(5.0,5.0,5.0, //eye
              0.0,0.0,0.0, //center
              0.0,1.0,0.0); //upY

    glMatrixMode(GL_MODELVIEW);
}

/* display */
void Display(void)
{
   if(solid){
    glEnable(GL_LIGHTING);   // Draw polygon interiors with lighting enabled.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // Draw polygon interiors.

    glEnable(GL_POLYGON_OFFSET_FILL);  // Turn on offset for filled polygons.
    glPolygonOffset(1,1);      // Set polygon offset amount.

  }else{
    glDisable(GL_LIGHTING);  // Draw polygon outlines with lighting disabled.
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Draw polygon outlines.

    glColor3f(1.0,1.0,0.5);  // Draw the outlines in black.
  }
    //clean the scene
    glClearColor(0.3,0.3,0.3,1.0); //set background
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    //set matrix mode to GL_MODELVIEW
    glMatrixMode(GL_MODELVIEW);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    
    glColor3f(1.0f,0.4f,0.0f);

    glPushMatrix();
    glLoadIdentity();
    glRotatef(rotation_x,0.0,1.0,0.0);
    glRotatef(rotation_y,1.0,0.0,0.0);
    glScalef(1.0f,1.0f,1.0f);
    
    doughnut(.7, 2, 100, 20);

    glPopMatrix();

    glutSwapBuffers();
};

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
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE);
    glutInitWindowSize(win_width,win_height);
    glutInitWindowPosition(100,100);

    //step 2. Open a window named "GLUT DEMO"
    win = glutCreateWindow("Torus");

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