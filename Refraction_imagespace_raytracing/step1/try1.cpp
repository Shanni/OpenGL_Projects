#include <GLUT/glut.h>
#include <stdlib.h>
 
float rotationAngle = 0;
 
// Draw the box
void drawBox(float width, float length, float height) 
{    
    glPushMatrix();
     
    // Rotate the cube by the specific angle and vector
    glRotatef(rotationAngle, 1.0f, 1.0f, 1.0f);
     
    // Begin drawing the cube
    glBegin(GL_QUADS);
     
    // Top face
    //glColor3f(1.0f, 0.0f, 0.0f);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-width / 2, height / 2, -length / 2);
    glVertex3f(-width / 2, height / 2, length / 2);
    glVertex3f(width / 2, height / 2, length / 2);
    glVertex3f(width / 2, height / 2, -length / 2);
     
    // Bottom face
    //glColor3f(0.0f, 1.0f, 0.0f);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-width / 2, -height / 2, -length / 2);
    glVertex3f(width / 2, -height / 2, -length / 2);
    glVertex3f(width / 2, -height / 2, length / 2);
    glVertex3f(-width / 2, -height / 2, length / 2);
     
    // Left face
    glColor3f(0.0f, 0.0f, 1.0f);
    glNormal3f(-1.0f, 0.0f, 0.0f);    
    glVertex3f(-width / 2, -height / 2, -length / 2);
    glVertex3f(-width / 2, -height / 2, length / 2);
    glVertex3f(-width / 2, height / 2, length / 2);
    glVertex3f(-width / 2, height / 2, -length / 2);
     
    // Right face
    glColor3f(1.0f, 1.0f, 0.0f);
    glNormal3f(1.0f, 0.0f, 0.0f);    
    glVertex3f(width / 2, -height / 2, -length / 2);
    glVertex3f(width / 2, height / 2, -length / 2);
    glVertex3f(width / 2, height / 2, length / 2);
    glVertex3f(width / 2, -height / 2, length / 2);
     
    // Front face
    glColor3f(1.0f, 0.0f, 1.0f);
    glNormal3f(0.0f, 0.0f, 1.0f);    
    glVertex3f(-width / 2, -height / 2, length / 2);
    glVertex3f(width / 2, -height / 2, length / 2);    
    glVertex3f(width / 2, height / 2, length / 2);    
    glVertex3f(-width / 2, height / 2, length / 2);
     
    // Back face
    glColor3f(0.0f, 1.0f, 1.0f);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(-width / 2, -height / 2, -length / 2);    
    glVertex3f(-width / 2, height / 2, -length / 2);    
    glVertex3f(width / 2, height / 2, -length / 2);    
    glVertex3f(width / 2, -height / 2, -length / 2);
     
    glEnd();
     
    glPopMatrix();
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
 
// Handle keyboard input
void handleKeyPress(unsigned char key, int x, int y) 
{
    switch (key) 
    {
        // Close on escape key
        case 27: 
            exit(0);
    }
}
 
void initRendering() 
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LIGHTING);                
    glEnable(GL_LIGHT0);
                 
    // Setup light
    GLfloat lightAmbient[] = {0.5f, 0.5f ,0.5f, 1.0f}; 
    GLfloat lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f}; 
    GLfloat lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
 
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient); 
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse); 
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular); 
     
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}
 
void handleResize(int w, int h) 
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / (double)h, 1.0, 1000.0);
}
 
void drawScene() 
{
    // Clear the stencil buffer
    glClearStencil(0); 
    // Clear depth
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
     
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
     
    glTranslatef(0.0f, 0.0f, -45.0f);
    glRotatef(45, 1.0f, 0.0f, 0.0f);
         
    // Set light position
    GLfloat lightPosition[] = {0.0f, 10.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
     
    // Draw box
    glPushMatrix();
    glTranslatef(0.0f, 5.0f, 0.0f);
    drawBox(5.0f, 5.0f, 5.0f);
    glPopMatrix();
     
    // Enable the stencil buffer
    glEnable(GL_STENCIL_TEST); 
    // Disable drawing colors
    glColorMask(0, 0, 0, 0); 
    // Disable depth testing
    glDisable(GL_DEPTH_TEST); 
    // Make the stencil test always pass
    glStencilFunc(GL_ALWAYS, 1, 1); 
    // Make pixels in the stencil buffer be set to 1 when the stencil test passes
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    // Set all of the pixels covered by the floor to be 1 in the stencil buffer
    drawFloor(30.0f, 30.f, 1.0f);
     
    // Enable drawing colors to the screen
    glColorMask(1, 1, 1, 1); 
    // Enable depth testing
    glEnable(GL_DEPTH_TEST); 
    // Make the stencil test pass only when the pixel is 1 in the stencil buffer
    glStencilFunc(GL_EQUAL, 1, 1);
    // Make the stencil buffer not change
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); 
     
    //Draw the cube, reflected vertically, at all pixels where the stencil
    //buffer is 1
    glPushMatrix();
    glScalef(1, -1, 1);
    glTranslatef(0, 5.0f, 0);
    drawBox(5.0f, 5.0f, 5.0f);
    glPopMatrix();
     
    // Disable using the stencil buffer
    glDisable(GL_STENCIL_TEST); 
     
    // Blend the floor onto the screen
    glEnable(GL_BLEND);    
    drawFloor(30.0f, 30.f, 0.8f);
    glDisable(GL_BLEND);
     
    glutSwapBuffers();    
}
 
void update(int value) 
{
    rotationAngle += 1.0f;
    if (rotationAngle > 360) 
        rotationAngle = 0;        
     
    glutPostRedisplay();
    glutTimerFunc(25, update, 0);
}
 
 
int main(int argc, char *argv[])
{
    // Initialize Glut
    glutInit(&argc, argv);
    // Initialize display mode and use stencil
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
    // Initialize a Glut window
    glutInitWindowSize(640, 480);
    // Create glut window
    glutCreateWindow("OpenGL GLUT reflection example");
    // Initialize rendering
    initRendering();
    // Set display function
    glutDisplayFunc(drawScene);
    // Set timer function
    glutTimerFunc(25, update, 0);
    // Set keyPress function
    glutKeyboardFunc(handleKeyPress);
    // Set resize function 
    glutReshapeFunc(handleResize);    
    // Start OpenGL loop
    glutMainLoop();
     
    return 0;
}