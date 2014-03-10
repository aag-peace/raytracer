#include<iostream>
#include<Windows.h>
#include "loader.h"
#include "wingedEdge.h"
#include "loopSubdivision.h"
#include "raytracer.h"
#include "checkerBoard.h"
#include <cstdlib>
#include <cstdarg>

using namespace std;
#define GAP  25             /* gap between subwindows */

GLboolean world_draw = GL_TRUE;
wingedEdge* we = new wingedEdge(); /* our new model (formerly pmodel) */
GLint selection = 0;

void redisplay_all(void);
void setTitle(char* filename);

GLuint window, screen;
GLuint sub_width = 512, sub_height = 512;
int last_x, last_y, spin_x, spin_y;
int wire = 0;
int material = 0;
int animate = 0;
char *filename;

GLvoid *font_style = GLUT_BITMAP_TIMES_ROMAN_10;
loader* load = new loader();
loopSubdivision* loop = new loopSubdivision();
vector<wingedEdge*> objects;
raytracer* r1;


void main_reshape(int width,  int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, height, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    sub_width = (width-GAP*2);
    sub_height = (height-GAP*2);
    
    glutSetWindow(screen);
    glutPositionWindow(GAP, GAP+GAP/2);
    glutReshapeWindow(sub_width, sub_height);
}


void setfont(char* name, int size) {
    font_style = GLUT_BITMAP_HELVETICA_10;
    if (strcmp(name, "helvetica") == 0) {
        if (size == 12) 
            font_style = GLUT_BITMAP_HELVETICA_12;
        else if (size == 18)
            font_style = GLUT_BITMAP_HELVETICA_18;
    } else if (strcmp(name, "times roman") == 0) {
        font_style = GLUT_BITMAP_TIMES_ROMAN_10;
        if (size == 24)
            font_style = GLUT_BITMAP_TIMES_ROMAN_24;
    } else if (strcmp(name, "8x13") == 0) {
        font_style = GLUT_BITMAP_8_BY_13;
    } else if (strcmp(name, "9x15") == 0) {
        font_style = GLUT_BITMAP_9_BY_15;
    }
}

void drawstr(GLuint x, GLuint y, char* format, ...) {
    va_list args;
    char buffer[255], *s;
    
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    
    glRasterPos2i(x, y);
    for (s = buffer; *s; s++)
        glutBitmapCharacter(font_style, *s);
}


void main_display(void) {
    glClearColor(0.8, 0.8, 0.8, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3ub(0, 0, 0);
    setfont("helvetica", 12);
    drawstr(GAP, GAP-10, "[ W ] wireframe [ S ] subdivide [ A ] average [ L ] Loop's subdivision [ 1 ] firstpass [ V ] verify");
    drawstr(GAP, GAP+5, "[ B ] ball.obj [ P ] prism.obj [ T ] top.obj [ D ] drum.obj [ R ] Raytrace [ O ] open [ X ] export [ Q ] quit");
    glutSwapBuffers();
}


void screen_reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0,1.0,-1.0,1.0,1.0,3.5);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0,0.0,2.0,0.0,0.0,0.0,0.0, 1.0, 0.0);
    glClearColor(0.2, 0.2, 0.2, 0.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

void screen_display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glRotatef((float)spin_x,0.0,1.0,0.0);
	glRotatef((float)spin_y,1.0,0.0,0.0);
	we->render();
	glPopMatrix();
	glutSwapBuffers();
}


void screen_mouse(int button, int state, int x, int y) {
    if (state & GLUT_UP) {
        if (x != last_x || y != last_y) {
            animate = 1;
        }
    } else {
        animate = 0;
        last_x = x;
        last_y = y;
    }
    redisplay_all();
}

void screen_motion(int x, int y) {
    spin_x += x-last_x;
    spin_y += y-last_y;
    last_x = x;
    last_y = y;
    redisplay_all();
}

void redisplay_all(void) {
    glutSetWindow(screen);
    screen_reshape(sub_width, sub_height);
    glutPostRedisplay();
}


void screen_keyboard(unsigned char key, int x, int y) {
	char* name = 0;
	Vertex* vertex;
	long lines = 0;
	
	switch (key) {
		case 'T':
		case 't':
			name = "data/tet.obj";
			break;
		case 'D':
		case 'd':
			name = "data/drum.obj";
			break;
		case 'R':
		case 'r':
			//ratrace
			cout << "rendering\n";
			r1->render(objects);
			cout << "rendered\n";
			break;
		case 'B':
		case 'b':
			name = "data/ball.obj";
			break;
		case 'P':
		case 'p':
			name = "data/pawn.obj";
			break;
		case 'L':
		case 'l':
			we = loop->subdivide(we); 
			we->unitize();
			break;
		case 'Q':
		case 'q':
			exit(1);
			break;
	}
    
	if (name) {
		delete(we);
		we = new wingedEdge();
		load->readOBJ(we,name); 
		objects.pop_back();
		we->unitize();
		objects.push_back(we);
		we->ka = vec3<float>(0.5,0.0,0.0);
		we->kd = vec3<float>(0.3,0.1,0.0);
		we->ks = vec3<float>(0.7,0.7,0.7);
	}
    
	redisplay_all();
}



int main(int argc, char**argv){
	int a;
	//load->readOBJ(we,"data/tet.obj");
	//cin>>a;

	char * filename;
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(512+GAP*3, 512+GAP*3);
	glutInitWindowPosition(50, 50);
	glutInit(&argc, argv);

	wingedEdge * we1 = new wingedEdge();
	load->readOBJ(we1,"data/board.obj");
	we1->ka = vec3<float>(0.3,0.3,0.3);
	we1->kd = vec3<float>(0.3,0.3,0.3);
	we1->ks = vec3<float>(0.7,0.7,0.7);
	we1->isCheckerboard = true;
	checkerBoard *board = new checkerBoard(we1);
	objects.push_back(we1);
	

	
	load->readOBJ(we,"data/tet.obj");
	we->unitize();
	we->ka = vec3<float>(0.5,0.0,0.0);
	we->kd = vec3<float>(0.3,0.1,0.0);
	we->ks = vec3<float>(0.7,0.7,0.7);
	objects.push_back(we);


	r1 = new raytracer();
	r1->board = board;
	r1->set_img_size(2000,2000);
	cout << "rendering\n";
	r1->render(objects);
	cout << "rendered\n";

	GLuint window = glutCreateWindow("Object Viewer");
	glutReshapeFunc(main_reshape);
	glutDisplayFunc(main_display);
	setTitle(filename);
	screen = glutCreateSubWindow(window, GAP, GAP, 512+GAP, 512+GAP);
	glutReshapeFunc(screen_reshape);
	glutDisplayFunc(screen_display);
	glutKeyboardFunc(screen_keyboard);            
	glutMouseFunc(screen_mouse);            
	glutMotionFunc(screen_motion);            
	redisplay_all();
	glutMainLoop();

	return 0;
}


void setTitle(char* filename) {
	char title[256];
	strcpy(title, "Object Viewer - ");
	glutSetWindowTitle(strcat(title, filename));
}
