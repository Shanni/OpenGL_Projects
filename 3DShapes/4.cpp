#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>

GLuint startList;

/* global variables */
static int win(0); // GLUT window ID
static int win_width(600), win_height(600); // window's width and height
static float mouse_x(0.0), mouse_y(0.0);
static float rotation_x(0.0),rotation_y(0.0);

void errorCallback(GLenum errorCode)
{
   const GLubyte *estring;

   estring = gluErrorString(errorCode);
   fprintf(stderr, "Quadric Error: %s\n", estring);
   exit(0);
}

void init(void) 
{
   GLUquadricObj *qobj;
   GLfloat mat_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
   GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat mat_shininess[] = { 25.0 };
   GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
   GLfloat model_ambient[] = { 0.5, 0.5, 0.5, 1.0 };

   glClearColor(0.0, 0.0, 0.0, 0.0);

   glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, model_ambient);

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);

/* Create 4 display lists, each with a different quadric object.
 * Different drawing styles and surface normal specifications
 * are demonstrated.
 */
   startList = glGenLists(4);
   qobj = gluNewQuadric();
   //gluQuadricCallback(qobj, GLU_ERROR, errorCallback);

   gluQuadricDrawStyle(qobj, GLU_FILL); /* smooth shaded */
   gluQuadricNormals(qobj, GLU_FLAT); //GLU_SMOOTH
   glNewList(startList, GL_COMPILE);
      gluSphere(qobj, 0.75, 40, 20);
   glEndList();

   gluQuadricDrawStyle(qobj, GLU_FILL); /* flat shaded */
   gluQuadricNormals(qobj, GLU_FLAT);
   glNewList(startList+1, GL_COMPILE);
      gluCylinder(qobj, 0.5, 0.3, 1.0, 15, 5);
   glEndList();

   gluQuadricDrawStyle(qobj, GLU_FILL); /* flat shaded */
   gluQuadricNormals(qobj, GLU_FLAT);
   glNewList(startList+4, GL_COMPILE);
      gluCylinder(qobj, 0.5, 0.0, 1.0, 50, 5);
   glEndList();

   gluQuadricDrawStyle(qobj, GLU_LINE); /* wireframe */
   gluQuadricNormals(qobj, GLU_NONE);
   glNewList(startList+2, GL_COMPILE);
      gluCylinder(qobj, 0.5, 0.3, 1.0, 15, 10);
   glEndList();

   gluQuadricDrawStyle(qobj, GLU_LINE); /* wireframe */
   gluQuadricNormals(qobj, GLU_NONE);
   glNewList(startList+5, GL_COMPILE);
      gluCylinder(qobj, 0.5, 0.0, 1.0, 30, 10);
   glEndList();

   gluQuadricDrawStyle(qobj, GLU_LINE);
   gluQuadricNormals(qobj, GLU_NONE);
   glNewList(startList+3, GL_COMPILE);
      gluSphere(qobj, 0.75, 25, 10);
   glEndList();
}

void display(void)
{
   glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glPushMatrix();

   glEnable(GL_LIGHTING);
   glShadeModel (GL_FLAT); //GL_SMOOTH
   glTranslatef(-2.0, -1.0, 0.0);
   glPushMatrix();
   glRotatef(rotation_x,0.0,1.0,0.0);
   glRotatef(rotation_y,1.0,0.0,0.0);
   glCallList(startList);
   glPopMatrix();

   glShadeModel (GL_FLAT);
   glTranslatef(0.0, 2.0, 0.0);
   glPushMatrix();
   glRotatef(rotation_x,0.0,1.0,0.0);
   glRotatef(rotation_y,1.0,0.0,0.0);
   glCallList(startList+1);
   glPopMatrix();

   glShadeModel (GL_FLAT); 
   glTranslatef(2.0, -2.0, 0.0);
   glPushMatrix();
   glRotatef(rotation_x,0.0,1.0,0.0);
   glRotatef(rotation_y,1.0,0.0,0.0);
   glCallList(startList+4);
   glPopMatrix();

   glDisable(GL_LIGHTING);
   glColor3f(0.0, 1.0, 1.0);
   glTranslatef(0.0, 2.0, 0.0);
   glPushMatrix();
   glRotatef(rotation_x,0.0,1.0,0.0);
   glRotatef(rotation_y,1.0,0.0,0.0);
   glCallList(startList+2);
   glPopMatrix();

   glColor3f(1.0, 1.0, 0.0);
   glTranslatef(2.0, -2.0, 0.0);
   glPushMatrix();
   glRotatef(rotation_x,0.0,1.0,0.0);
   glRotatef(rotation_y,1.0,0.0,0.0);
   glCallList(startList+3);
   glPopMatrix();

   glColor3f(1.0, 1.0, 0.0);
   glTranslatef(0.0, 2.0, 0.0);
   glPushMatrix();
   glRotatef(rotation_x,0.0,1.0,0.0);
   glRotatef(rotation_y,1.0,0.0,0.0);
   glCallList(startList+5);
   glPopMatrix();

   glPopMatrix();
   glFlush();
}

void reshape (int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   if (w <= h)
      glOrtho(-2.5, 2.5, -2.5*(GLfloat)h/(GLfloat)w,
         2.5*(GLfloat)h/(GLfloat)w, -10.0, 10.0);
   else
      glOrtho(-2.5*(GLfloat)w/(GLfloat)h,
         2.5*(GLfloat)w/(GLfloat)h, -2.5, 2.5, -10.0, 10.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
      case 27:
         exit(0);
         break;
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
int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize(500, 500); 
   glutInitWindowPosition(100, 100);
   glutCreateWindow(argv[0]);
   init();
   glutDisplayFunc(display); 
   glutMouseFunc(Mouse);
   glutMotionFunc(MouseMotion);
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutMainLoop();
   return 0;
}