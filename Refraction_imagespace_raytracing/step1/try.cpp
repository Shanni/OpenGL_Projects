// #include <OpenGL/glext.h>
// void main(void)
// {
//    vec4 a = gl_Vertex;
//    a.x = a.x * 0.5;
//    a.y = a.y * 0.5;


//    gl_Position = gl_ModelViewProjectionMatrix * a;

// }  


#include <GLEW/glew.h> 
//#include <OpenGL/gl3.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include <GLUT/glut.h>

#include <stdio.h>

	int main(int argc, char **argv) {
	
		glutInit(&argc, argv);
		
		//...
		
		glewInit();
		if (glewIsSupported("GL_VERSION_2_0"))
			printf("Ready for OpenGL 2.0\n");
		else {
			printf("OpenGL 2.0 not supported\n");
			return 1;
		}
	
		//setShaders();
	
		glutMainLoop();
	}
