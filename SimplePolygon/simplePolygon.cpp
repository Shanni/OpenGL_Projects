/***************************************************************
 *  
 *  useage:
 *      use mouse "MIDDLE" button to popup menu;
 *      two modes: CREATION -- create polygon
 *                 SELECTION -- select point;
 *
 ****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <GLUT/glut.h>

/* global variables */
static int win(0); // GLUT window ID
static int menu_id(0); // menu id;
static int win_width(600), win_height(600); // window's width and height
static float mouse_x(0.0), mouse_y(0.0);
static float x(0.0), y(0.0),px(0.0), py(0.0);
static int mouse_mode(-1); // flag, 0 -- selecting mode, 1 -- creating mode;
static int start(0);
static char answer[2];
static int translation(0);//flag 
static int scale(0);//flag

void MidPoint(int, int, int, int);

/* Pixel vertex */
struct MyPixelVertex{
    int x;
    int y;

    MyPixelVertex():x(-1),y(-1){};
    MyPixelVertex(int a,int b):x(a),y(b){};
};


/* vertex with a quad selection range. the edge length is 0.02 */
struct MyVertex{
    int id;
    float x;
    float y;
    float range; //mouse selection range;
    bool check = true; //to make life simpler when checking each node

    MyVertex():x(0.0),y(0.0),id(-1),range(4.0){};
    MyVertex(float a, float b):x(a),y(b),id(-1),range(4.0){};
    //MyVertex(double a, double b):x(a),y(b),id(-1),range(0.02){};

    void draw_selection_range()
    {
        glColor3f(0.2f,0.5f,0.8f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(x-range,y+range); //edge length is 0.02
        glVertex2f(x-range,y-range);
        glVertex2f(x+range,y-range);
        glVertex2f(x+range,y+range);
        glEnd();
    };

    void draw()
    {
        glColor3f(1.0f,0.8f,0.0f);
        glPointSize(4.0);
        glBegin(GL_POINTS);
        glVertex2f(x,y);
        glEnd();
    }

    bool is_selected(float mouse_x_pos, float mouse_y_pos)
    {
        if(mouse_x_pos >= (x-range) && mouse_y_pos >= (y-range) &&
           mouse_x_pos <+ (x+range) && mouse_y_pos >= (y-range)){
            return true;
        }else{
            return false;
        }
    }

    void set_range(float value){
        if(value > 0.0 && value < 1.0)
            range = value;
    }
};
struct MyLines
{
    std::vector<MyVertex> nodes; //nodes of polygon

    MyLines(){};
    int size_of_nodes(){return nodes.size();} // return size;
    bool is_empty(){return nodes.empty();}
    void clear(){nodes.clear();} //clear all nodes;

    void draw()
    {
        //1) rendering draw the lines

        //!!! Attention, you are not allowed to use "GL_LINE", 
        // you have to render the inside of polygon by drawing each pixel(points) 
        
        for(int i = 0; i < (int) nodes.size() -1; ++i){
            MidPoint(nodes[i].x,nodes[i].y,nodes[i+1].x,nodes[i+1].y);
        }
        
        // //2) rendering the vertices;
        // for(int i = 0; i < (int)nodes.size(); ++i){
        //     nodes[i].draw();
        //     if(nodes[i].is_selected(mouse_x,mouse_y)){
        //         nodes[i].draw_selection_range();
        //         printf("selected range, %f and %f\n", mouse_x, mouse_y);
        //     }
                
        // }
        
        // //3) rendering the edges;
        // glColor3f(0.8,0.5,0.2);
        // glLineWidth(2.0);
        // glBegin(GL_LINE_LOOP);
        // for(int i = 0; i < (int)nodes.size(); ++i){
        //     glVertex2f(nodes[i].x,nodes[i].y);
        // }
         glEnd();
    }

    bool intersaction(){

        // for(int i = 0; i < (int) nodes.size() -2; ++i){
        //     int defx = (nodes[i+1].x - nodes[i].x);
        //     int defy = (nodes[i+1].y - nodes[i].y);
        //     int sign;
        //     if ((defx *(nodes[i+2].y - nodes[i].y) - defy*(nodes[i+2].x - nodes[i].x))>0)
        //         sign = 1;
        //     else if ((defx *(nodes[i+2].y - nodes[i].y) - defy*(nodes[i+2].x - nodes[i].x))<0)
        //         sign = 0;
        //     else {printf("there is intersaction\n"); return false; }
        //     for (int j = i+2; j < (int) nodes.size() -1; ++j)
        //     {
        //        if ((sign==1) && (defx *(nodes[j].y - nodes[i].y) - defy*(nodes[j].x - nodes[i].x))<=0)
        //             return false;
        //         if ((sign==0) && (defx *(nodes[j].y - nodes[i].y) - defy*(nodes[j].x - nodes[i].x))>=0)
        //             return false;
        //     }
        // }
        for (int i = 0; i < (int) nodes.size()-3 ; ++i){
            for ( int j = i + 2; j < (int) nodes.size()-1; ++j){
                float temp=((nodes[j].x-nodes[i].x)*(nodes[i+1].y- nodes[i].y)\
                    -(nodes[j].y - nodes[i].y)*(nodes[i+1].x- nodes[i].x))/((nodes[j+1].y - nodes[j].y)\
                    *(nodes[i+1].x- nodes[i].x)-(nodes[j+1].x- nodes[j].x)*(nodes[i+1].y- nodes[i].y));

                float x = nodes[j].x + temp*\
                    (nodes[j+1].x- nodes[j].x);
                if(((nodes[i].x > x && nodes[i+1].x<x)||(nodes[i].x < x && nodes[i+1].x>x))&&\
                    ((nodes[j].x > x && nodes[j+1].x<x)||(nodes[j].x > x && nodes[j+1].x>x))){
                    //printf("float temp is %f\n", temp);
                    //printf("x1=%d x2=%d x3=%d x4=%d\n", nodes[i].x, nodes[i+1].x, nodes[j].x, nodes[j+1].x);
                    //printf("intersaction happens between node %d and %d\n", i, j);
                    return true;
                }
                    
            }
            return false;
        }
    }

    bool scanpolygon(int x, int y){
        int sign = (x-nodes[0].x)*(nodes[1].y- nodes[0].y)-(y- nodes[0].y)*(nodes[1].x -nodes[0].x) > 0 ? 1 : 0;
        for(int i = 1; i< (int)nodes.size()-1; ++i){
            if(sign == 1){
                if((x-nodes[i].x)*(nodes[i+1].y- nodes[i].y)-(y- nodes[i].y)*(nodes[i+1].x -nodes[i].x) <= 0 ){
                    return false;
                }
            }else if(sign == 0){
                if((x-nodes[i].x)*(nodes[i+1].y- nodes[i].y)-(y- nodes[i].y)*(nodes[i+1].x -nodes[i].x) >= 0 ){
                    return false;
                }
            }else{

            }
        }
        return true;
    }

    void translation(int x,int y){
        int defx=x-nodes[0].x;
        int defy=y-nodes[0].y;
        for(int i = 0; i < (int) nodes.size(); ++i){
            nodes[i].x+=defx;
            nodes[i].y+=defy;
        }
    }

};
/* Polygon */
struct MyPolygon{
    std::vector<MyVertex> nodes; //nodes of polygon

    MyPolygon(){};

    int size_of_nodes(){return nodes.size();} // return size;
    bool is_empty(){return nodes.empty();}
    void clear(){nodes.clear();} //clear all nodes;

    void draw()
    {
        //1) rendering draw the polygon

        //!!! Attention, you are not allowed to use "GL_POLYGON", 
        // you have to render the inside of polygon by drawing each pixel(points) 
        
        glBegin(GL_POLYGON);
        for(int i = 0; i < (int) nodes.size(); ++i){
            glVertex2f(nodes[i].x,nodes[i].y);
        }
        glEnd();
        
        //2) rendering the vertices;
        for(int i = 0; i < (int)nodes.size(); ++i){
            nodes[i].draw();
            if(nodes[i].is_selected(mouse_x,mouse_y))
                nodes[i].draw_selection_range();
        }
        
        //3) rendering the edges;
        glColor3f(0.8,0.5,0.2);
        glLineWidth(2.0);
        glBegin(GL_LINE_LOOP);
        for(int i = 0; i < (int)nodes.size(); ++i){
            glVertex2f(nodes[i].x,nodes[i].y);
        }
        glEnd();
    };
};

MyPolygon polygon_buffer; // a polygon buffer;
std::vector<MyPolygon>   polygons; //record all polygons on the screen.

MyLines lines_buffer; // a polygon buffer;
std::vector<MyLines>   lines; //record all polygons on the screen.

/* Pixel to World coordinates*/
MyPixelVertex WorldToPixel(MyVertex& v)
{
    MyPixelVertex pxl;
    pxl.x = (int) ((v.x+1.0)*win_width*0.5);
    pxl.y = (int) ((1.0-v.y)*win_height*0.5);
    return pxl;
};

MyVertex PixelToWorld(MyPixelVertex& pxl)
{
    MyVertex v;
    v.x = (pxl.x*2.0)/win_width - 1.0;
    v.y = 1.0 - (pxl.y*2.0)/win_height;
    return v;
};


void swap(int *x0, int *y0, int *x1, int *y1, float slope) {
    if(slope>0){
    int t = *y1 + *x0 - *y0;
    *y1 = *x1 + *y0 - *x0;
    *x1 = t;
    }
    if(slope<0){
    int t = -*y1 + *y0 + *x0;
    *y1 = -*x1 + *x0 + *y0;
    *x1 = t;
    }
}


void MidPoint(int x0, int y0, int x1, int y1){
   
    glColor3f (1.0, 1.0, 1.0);
    glBegin(GL_POINTS);
 
    int case1 = 0, case2=0, case3=0, case4=0;   

    if (x0==x1){
        case4=1;
    }

    else{
    float slope = float(y1-y0)/float(x1-x0);
    if (slope > 1 || slope < -1){
        swap(&x0,&y0,&x1,&y1,slope);
        case3=1;
        //printf("case3\n");
    }

    if(x1<x0){
        x1=2*x0-x1;
        case2=1;
        //printf("case2\n");
    }

    if(y1<y0){
        y1=2*y0-y1;
        case1=1;
        //printf("case1\n");
    
    }

     int dx, dy, incrE, incrSE, d, x, y;

   dx = x1 - x0;
   dy = y1 - y0;
   d  = dy * 2 - dx;
   incrE = dy * 2;
   incrSE = (dy - dx) * 2;
   x = x0;
   y = y0;
    
    glVertex2i (x, y);
    glColor3f (0.0, 1.0, 1.0);

    int printy,printx;
   while ( x < x1 ) {
      if ( d <= 0 ) {
         d += incrE;
     x ++;
      } 
      else {
         d += incrSE;
     x ++;
     y ++;
      }

      printx=x;
      printy=y;


    if(case1){
        printy=2*y0-printy;
    }

      if(case2){
        printx=2*x0-printx;
      }

      if (case3){
        swap(&x0,&y0,&printx,&printy,slope);
      }  
     
        glVertex2i(printx, printy);

   }

    }//end of case1,2,3

    if(case4){
    int i;
    if(y0<y1){
    for(i=y0; i<y1; i++)
        glVertex2i(x0,i);
    }else if(y0>y1){
    for(i=y0; i>y1; i--)
        glVertex2i(x0,i);
    }else{
        glVertex2i(x0,y0);
    }
    }

    glEnd();


    glFlush ();
}


/* set menu */
void Menu(int value) //Menu function;
{
    if(value == 3)
        exit(-1);  // quit
    else if(value == 0){
        mouse_mode = 0; // selection
    }else if(value == 1){
        mouse_mode = 1; // creation
    }else if(value == 2){
        mouse_mode = 2; // NONE
    }else if(value == 4){
        polygons.clear(); // clear polygons
        glutPostRedisplay();
    }else if(value == 5){
        mouse_mode == 5; //draw
    }
}

/* menu creation */
void CreateMenu(void)
{
    menu_id = glutCreateMenu(Menu);
    //create an entry

    glutAddMenuEntry("NONE",2);
    glutAddMenuEntry("SELECTION",0);
    glutAddMenuEntry("DRAW",5);
    glutAddMenuEntry("CREATION",1);
    
    glutAddMenuEntry("CLEAR",4);
    glutAddMenuEntry("Quit",3);

    //let the menu respnd on the right button
    glutAttachMenu(GLUT_MIDDLE_BUTTON);
}


// Call-back function glutMouseFunc();
// get the mouse position from here
void MouseMotion(int button, int state, int x, int y)
{
    // we calculate the coordiante transform by ourself;
    mouse_x = (x*2.0)/win_width - 1.0;
    mouse_y = 1.0 - (y*2.0)/win_height;
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        printf("mouse screen(pixel) pos: (%d %d) window pos: (%f %f)\n", \
                x,y,mouse_x,mouse_y);
        if(!translation){

        lines_buffer.nodes.push_back(MyVertex(x,y));
        
        if(lines_buffer.nodes[0].is_selected(x,y)==true){
            if(start)
            printf("A polygon is created, please do right click on mouse to finish\n");
            printf("Please click the right button on wherever you wanna test if a point is inside a polygon or not\n");
        }
        

        if(mouse_mode == 5){
            //selecting mode;
            lines_buffer.nodes.push_back(MyVertex(x,y));

        }else if(mouse_mode == 0){
           
        }else if(mouse_mode == 1){
            //creating mode;
            polygon_buffer.nodes.push_back(MyVertex(mouse_x,mouse_y));
        } 
        if(!start)
            start=1;
    }else if(translation){
        lines_buffer.translation(x,y);
        lines_buffer.draw();
        translation=0;
    }
        glutPostRedisplay(); // for redraw the current mouse position;

    }else if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){

        lines.push_back(lines_buffer); //put polygon buffer to container;
        
        if(lines_buffer.intersaction()){
         printf("Intersaction happens, This is not a simple polygon \n", lines_buffer.intersaction());
        }else{
        printf("There's no intersaction \n", lines_buffer.intersaction());
        }
         printf("Is the selection point inside the last polygon? %s \n",  lines_buffer.scanpolygon(x,y) ? "true" : "false");

        printf("Do you want to do translation to the polygon? (answer 'y' or 'n')\n");
        
        scanf("%c",answer);
        if(answer[0]=='y'){
            printf("Please left click target position\n");
            glutMouseFunc(MouseMotion);
        }

        if(mouse_mode == 1){
            // creating ending;
            polygons.push_back(polygon_buffer); //put polygon buffer to container;
            polygon_buffer.clear(); // clear the buffer for next creating;
        }else if(mouse_mode == 0){
            // creating ending;
             
        }else if(mouse_mode == 5){
            lines.push_back(lines_buffer); //put polygon buffer to container;
            lines_buffer.clear(); // clear the buffer for next creating;
        }

        lines_buffer.clear(); // clear the buffer for next creating;
    }
};

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
    gluOrtho2D(-1.0f,1.0f,-1.0f,1.0f);
}


/* display */
void Display(void)
{
    //clean the scene
    glClear(GL_COLOR_BUFFER_BIT);
    
    // //set matrix mode to GL_MODELVIEW
    // glMatrixMode(GL_MODELVIEW);
    // glLoadIdentity();

    // //draw a unit cube;
    // glColor3f(1.0f,1.0f,1.0f);
    // glutWireCube(1.0);

    //draw mouse pointer;
    glColor3f(1.0f,1.0f,0.0f);
    glPointSize(1.0);
    glBegin(GL_POINTS);
    glVertex2f(mouse_x,mouse_y);
    glEnd();


    // //draw polygons
     lines_buffer.draw();
     
    // polygon_buffer.draw();

    for(int i = 0; i < (int) lines.size(); ++i)
    {
        lines[i].draw();
    }


     glFlush();
};

/* MAIN */
int main(int argc, char* argv[])
{
    
    //step 1. initialize GLUT
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGB|GLUT_SINGLE);
    glutInitWindowSize(win_width,win_height);
    glutInitWindowPosition(100,100);

    //step 2. Open a window named "GLUT DEMO"
    win = glutCreateWindow("GLUT Mouse Motion");
    glClearColor(0.0,0.0,0.0,0.0); //set background
    
  
   //misc
   glutSetCursor(GLUT_CURSOR_CROSSHAIR);

    //disable depth test as only draw 2D objects;
    //glDisable(GL_DEPTH_TEST);

    glMatrixMode (GL_PROJECTION);
    
     gluOrtho2D (0.0, win_width, win_height, 0.0);
     
    //step 3. register the display function
    glutDisplayFunc(Display);
    glutMouseFunc(MouseMotion);
    //glutReshapeFunc(WindowSizeReShape);
    
    CreateMenu();

    //step 4. enter the event processing loop
    glutMainLoop();
}
