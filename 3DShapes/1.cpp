//No.1,2
#include <GLUT/glut.h>

GLsizei winWidth=500, winHeight=500;

void init(void){
    glClearColor(1.0,1.0,1.0,0.0);
}
void displayWireCube(void){
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0,0.0,1.0);
    
    glLoadIdentity();

    gluLookAt(5.0,5.0,5.0,0.0,0.0,0.0,0.0,1.0,0.0);
    glTranslatef(0.0, 0.0, -2.0);
    glScalef(1.5,2.0,1.0);
    glutWireCube(1.0);

    glScalef(.8,0.5,.8);
    glTranslatef(1.6,1.6,6.0);
    glutWireTetrahedron();
    glFlush();
}
void winReshapeFunc(GLint newWidth, GLint newHeight){
   double viewRangeX = (double)newWidth / winWidth,
        viewRangeY = (double)newHeight / winHeight;
    glViewport(0, 0, newWidth, newHeight);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();   // ** Clear the matrix stack
    glFrustum(-viewRangeX, viewRangeX, -viewRangeY, viewRangeY, 2.0, 20.0); // ** Adjust the view range
    glMatrixMode(GL_MODELVIEW);

    glClear(GL_COLOR_BUFFER_BIT);
}
int main(int argc, char **argv){
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(winWidth,winHeight);
    glutCreateWindow("draw wireframe shapes");

    init();
    glClear(GL_COLOR_BUFFER_BIT);
    glutDisplayFunc(displayWireCube);
    glutReshapeFunc(winReshapeFunc);

    glutMainLoop();
    return 0;
}