
/*
  Keyboard inputs:
  +  increase subdivision depth
  -  decrease subdivision depth
  s  flat shading (regular object)
  m  smooth shading (approximate sphere)
  w  wireless shape
  q  quit
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GLUT/glut.h>

#define R 1
#define r .3
#define p0 0
#define p1 1.5707963
#define p2 3.1415926
#define p3 4.7123889
#define p4 6.2831853

#define MAXDEPTH 4 // maximum subdivision recursion

static int win(0); // GLUT window ID
static int win_width(600), win_height(600); // window's width and height
static float mouse_x(0.0), mouse_y(0.0);
static float rotation_x(0.0),rotation_y(0.0);


/* vertex data array */
static GLfloat idatat[25][2]={
  {p0,p0},{p0,p1},{p0,p2},{p0,p3},{p0,p4},
  {p1,p0},{p1,p1},{p1,p2},{p1,p3},{p1,p4},
  {p2,p0},{p2,p1},{p2,p2},{p2,p3},{p2,p4},
  {p3,p0},{p3,p1},{p3,p2},{p3,p3},{p3,p4},
  {p4,p0},{p4,p1},{p4,p2},{p4,p3},{p4,p4},
 };

static int indicest[32][3] = {
  {0,5,6},{0,6,1},{1,6,7},{1,7,2},{2,7,8},{2,8,3},{3,8,9},{3,9,4},
  {5,10,11},{5,11,6},{6,11,12},{6,12,7},{7,12,13},{7,13,8},{8,13,14},{8,14,9},
  {10,15,16},{10,16,11},{11,16,17},{11,17,12},{12,17,18},{12,18,13},{13,18,19},{13,19,14},
  {15,20,21},{15,21,16},{16,21,22},{16,22,17},{17,22,23},{17,23,18},{18,23,24},{18,24,19}
}; 


GLfloat mat_specular[] = {0.0, 0.0, 0.0, 1.0};
GLfloat mat_diffuse[]  = {0.8, 0.6, 0.4, 1.0};
GLfloat mat_ambient[]  = {0.8, 0.6, 0.4, 1.0};
GLfloat mat_shininess = 100.0;	/* unused if specular is 0 */

GLfloat light_ambient[]  = {0.2, 0.2, 0.2, 1.0};
GLfloat light_diffuse[]  = {1.0, 1.0, 1.0, 1.0};
GLfloat light_specular[] = {0.0, 0.0, 0.0, 1.0};

GLfloat light_position[] = {0.5, 0.5, 1.2, 0.0}; /* directional */

int flat = 1;			/* 0 = smooth shading, 1 = flat shading */
int subdiv = 0;			/* number of subdivisions */
int solid = 1;
float cutoff=.3;
  GLfloat d1[2]={2,3};
  GLfloat d2[2]={4,3};
  GLfloat d3[2]={6,2};
int rotatex = 0; 

void surf(GLfloat u[2], GLfloat v[3], GLfloat n[3]){ 
  /*
  u[0] = theta -> angle of R
  u[1] = phi -> angle of r
  */
  v[0]= (R + r*cos(u[1]))*cos(u[0]);
  v[1]= (R + r*cos(u[1]))*sin(u[0]);
  v[2]= r*sin(u[1]);


  n[0]=cos(u[1])*cos(u[0]);
  n[1]=cos(u[1])*sin(u[0]);
  n[2]=sin(u[1]);

}

//calculate norm for a torus
float curv(GLfloat u[2]){
  return abs(2*cos(u[1])/(r*(R+r*cos(u[1]))));
}
/* recursively subdivide face `depth' times */
/* and draw the resulting triangles */
void subdividet(GLfloat u1[2], GLfloat u2[2], GLfloat u3[2], float cutoff, int depth)
{
   GLfloat v1[3], v2[3], v3[3], n1[3], n2[3], n3[3];
   GLfloat u12[2], u23[2], u31[2];
   int i;

   if(depth == MAXDEPTH || (curv(u1) < cutoff && curv(u2) < cutoff && curv(u3) < cutoff)){
    surf(u1,v1,n1);
    surf(u2,v2,n2);
    surf(u3,v3,n3);
    glBegin(GL_TRIANGLES);
      glNormal3fv(n1); 
      glVertex3fv(v1);
      //glNormal3fv(n2); 
      glVertex3fv(v2);
      //glNormal3fv(n3);
      glVertex3fv(v3);
    glEnd();
    return;
   } 

   for(i=0;i<2;i++){
    u12[i]=(u1[i]+u2[i])/2.0;
    u23[i]=(u2[i]+u3[i])/2.0;
    u31[i]=(u1[i]+u3[i])/2.0;
   }
   subdividet(u1,u12,u31,cutoff,depth+1);
   subdividet(u2,u23,u12,cutoff,depth+1);
   subdividet(u3,u31,u23,cutoff,depth+1);
   subdividet(u12,u23,u31,cutoff,depth+1);
}



void displayt(void)
{
  int i;
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

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
    glRotatef(rotation_x,0.0,1.0,0.0);
    glRotatef(rotation_y,1.0,0.0,0.0);
   glScalef(1.0f,1.0f,1.0f);
  //gluPerspective(60.0,(float)win_width/win_height,0.01,50);
  gluLookAt(0.5, 0.6, -0.5, /* eye */
            0.0, 0.0, 0.0,  /* at */
      0.0, 1.0, 0.0); /* up */
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  for (i = 0; i < 32; i++) {
    subdividet(
        idatat[indicest[i][0]],
        idatat[indicest[i][1]],
        idatat[indicest[i][2]],
        cutoff,
	      subdiv);
  }

  glFlush();

  glPopMatrix();

  glutSwapBuffers();
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
void reshape(GLint newWidth, GLint newHeight)
{
  glViewport(0, 0, newWidth, newHeight);

  double viewRangeX = (double) newWidth / win_width,
    viewRangeY = (double) newHeight / win_height;

  glMatrixMode(GL_PROJECTION); // Set the view volume
  glLoadIdentity();
  glFrustum(-viewRangeX, viewRangeX, -viewRangeY, viewRangeY, 2.0, 20.0);

  glMatrixMode(GL_MODELVIEW);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// clear the colors as well as the depth buffer


  glutPostRedisplay();
}

void reshape1(int w, int h)
{
  GLfloat aspect = (GLfloat) w / (GLfloat) h;
  glViewport(0, 0, w, h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (w <= h)
    glOrtho(-1.25, 1.25, -1.25 * aspect, 1.25 * aspect, -2.0, 2.0);
  else
    glOrtho(-1.25 * aspect, 1.25 * aspect, -1.25, 1.25, -2.0, 2.0);
  glMatrixMode(GL_MODELVIEW);

  glutPostRedisplay();
}
void initt()
{
  glClearColor(0.3, 0.3, 0.3, 0.0);

  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialf(GL_LIGHT0, GL_SHININESS, mat_shininess);

  glShadeModel(GL_SMOOTH);	/* enable smooth shading */
  glEnable(GL_LIGHTING);	/* enable lighting */
  glEnable(GL_LIGHT0);		/* enable light 0 */

  //glEnable(GL_DEPTH_TEST);
}

void keyboardt(unsigned char key, int x, int y)
{
  if (key=='q') exit(0);
  if (key=='+') subdiv++;
  if (key=='-') subdiv--;
  if (key=='s') {flat = 1; solid=1;}
  if (key=='m') {flat = 0; solid=1;}
  if (key=='w') solid=0;
  if (key=='u') rotatex--;

  if (subdiv<0) subdiv = 0;

  glutPostRedisplay();
}

int main(int argc, char **argv)
{
  /* create window */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH); /* single buffering */
  glutInitWindowSize(500, 500);
  glutCreateWindow("Subdivide");

  /* set callbacks */
  glutReshapeFunc(reshape1);
  glutDisplayFunc(displayt);
  glutMouseFunc(Mouse);
  glutMotionFunc(MouseMotion);
  glutKeyboardFunc(keyboardt);

  /* initialize GL */
  initt();


  glutMainLoop();
  return(0);
}