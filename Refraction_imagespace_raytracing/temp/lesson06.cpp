// #include <windows.h>
//#include <GLEW/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include "tgaload.h"



#include <vector>

#define MAX_NO_TEXTURES 6

#define CUBE_TEXTURE 0

GLuint texture_id[MAX_NO_TEXTURES];
std::vector<const GLchar*> faces;

float xrot;
float yrot;
float zrot;
float ratio;

GLuint loadCubemap(std::vector<const GLchar*> faces)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0);

    int width=1,height=1;
    image_t image;
	
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    for(GLuint i = 0; i < faces.size(); i++)
    {
        // image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
        tgaLoad  ( faces[i], &image, TGA_FREE | TGA_LOW_QUALITY );
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
            GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, &image
        );
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return textureID;
}  

void cudeEnv(){
	vector<const GLchar*> faces;
faces.push_back("right.jpg");
faces.push_back("left.jpg");
faces.push_back("top.jpg");
faces.push_back("bottom.jpg");
faces.push_back("back.jpg");
faces.push_back("front.jpg");
GLuint cubemapTexture = loadCubemap(faces);  

}
void init(void)
{

   glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	 glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
   glEnable ( GL_COLOR_MATERIAL );
   glColorMaterial ( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );

	glEnable ( GL_TEXTURE_2D );
   glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );
   glGenTextures (1, texture_id);
   glActiveTexture(GL_TEXTURE0);



   image_t   temp_image;

   glBindTexture ( GL_TEXTURE_2D, texture_id[CUBE_TEXTURE] );
   tgaLoad  ( "5.tga", &temp_image, TGA_FREE | TGA_LOW_QUALITY );

   glEnable ( GL_CULL_FACE );


   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void reshape( int w, int h )
{
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0)
		h = 1;

	ratio = 1.0f * w / h;
	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set the clipping volume
	gluPerspective(80,ratio,1,200);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 30,
		      0,0,10,
			  0.0f,1.0f,0.0f);
}

void display( void )
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
   glLoadIdentity ( );
   glPushMatrix();
   glTranslatef ( 0.0, 0.0, -5.0 );
   glRotatef ( xrot, 1.0, 0.0, 0.0 );
   glRotatef ( yrot, 0.0, 1.0, 0.0 );
   glRotatef ( zrot, 0.0, 0.0, 1.0 );

   glBindTexture ( GL_TEXTURE_2D, texture_id[0] );
    glutSolidTeapot(2.0);
 //   glBegin ( GL_QUADS );
 //   // Front Face
	// 	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
	// 	glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
	// 	glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
	// 	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
	// 	// Back Face
	// 	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	// 	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
	// 	glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
	// 	glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
	// 	// Top Face
	// 	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
	// 	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
	// 	glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
	// 	glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
	// 	// Bottom Face
	// 	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	// 	glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
	// 	glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
	// 	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
	// 	// Right face
	// 	glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
	// 	glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
	// 	glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
	// 	glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
	// 	// Left Face
	// 	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	// 	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
	// 	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
	// 	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
	// glEnd();
   glPopMatrix();
   xrot+=0.3f;
	yrot+=0.2f;
	zrot+=0.4f;
   glutSwapBuffers();
}

void keyboard ( unsigned char key, int x, int y )  // Create Keyboard Function
{
  switch ( key ) {
    case 27:        // When Escape Is Pressed...
      exit ( 0 );   // Exit The Program
      break;        // Ready For Next Case
    default:        // Now Wrap It Up
      break;
  }
}

void arrow_keys ( int a_keys, int x, int y )  // Create Special Function (required for arrow keys)
{
  switch ( a_keys ) {
    case GLUT_KEY_UP:     // When Up Arrow Is Pressed...
      glutFullScreen ( ); // Go Into Full Screen Mode
      break;
    case GLUT_KEY_DOWN:               // When Down Arrow Is Pressed...
      glutReshapeWindow ( 500, 500 ); // Go Into A 500 By 500 Window
      break;
    default:
      break;
  }
}

int main ( int argc, char** argv )   // Create Main Function For Bringing It All Together
{
  glutInit            ( &argc, argv ); // Erm Just Write It =)
  glutInitDisplayMode ( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA ); // Display Mode
  glutInitWindowPosition (0,0);
  glutInitWindowSize  ( 500, 500 ); // If glutFullScreen wasn't called this is the window size
  glutCreateWindow    ( "NeHe Lesson 6- Ported by Rustad" ); // Window Title (argv[0] for current directory as title)
  init ();
  glutReshapeWindow ( 500, 500 );
  glutDisplayFunc     ( display );  // Matching Earlier Functions To Their Counterparts
  glutReshapeFunc     ( reshape );
  glutKeyboardFunc    ( keyboard );
  glutSpecialFunc     ( arrow_keys );
  glutIdleFunc			 ( display );
  glutMainLoop        ( );          // Initialize The Main Loop
}




