#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <gl/glut.h>
#include <iostream>
#include <vector>
#include <sstream>



int nFPS = 30;
int frameCount = 0;
int currentTime;
int previousTime;
int fps;


void init(void)
{
	// init your data, setup OpenGL environment here
	glClearColor(1.0f, 1.0f, 0.f, 1.0f);

}



void drawFPS(int x, int y, float r, float g, float b, int font, char *string)
{
	glColor3f(r, g, b);
	glRasterPos2f(x, y); 
	int len, i;
	len = (int)strlen(string);
	for (i = 0; i < len; i++) {
		glutBitmapCharacter((void *) font, string[i]);
	}
}

void display(void)
{
	// put your OpenGL display commands here
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// reset OpenGL transformation matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); // reset transformation matrix to identity

	gluLookAt(0.f, 0.f, 3.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//glFlush();
	glutSwapBuffers();	// swap front/back framebuffer to avoid flickering 
}

void reshape(int w, int h)
{
	// reset viewport ( drawing screen ) size
	glViewport(0, 0, w, h);
	float fAspect = ((float)w) / h;
	// reset OpenGL projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70.f, fAspect, 0.001f, 30.f);
}



void keyboard(unsigned char key, int x, int y)
{
	// put your keyboard control here
	if (key == 27)
	{
		// ESC hit, so quit
		printf("demonstration finished.\n");
		exit(0);
	}
}

void calculateFPS()
{
	//  Increase frame count
	frameCount++;

	//  Get the number of milliseconds since glutInit called
	//  (or first call to glutGet(GLUT ELAPSED TIME)).
	currentTime = glutGet(GLUT_ELAPSED_TIME);

	//  Calculate time passed
	int timeInterval = currentTime - previousTime;

	if (timeInterval > 1000)
	{
		//  calculate the number of frames per second
		fps = frameCount / (timeInterval / 1000.0f);

		//  Set time
		previousTime = currentTime;

		//  Reset frame count
		frameCount = 0;
	}
}

void mouse(int button, int state, int x, int y)
{
	// process your mouse control here
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		printf("push left mouse button.\n");
}


void timer(int v)
{
	calculateFPS();

	glutPostRedisplay(); // trigger display function by sending redraw into message queue
	glutTimerFunc(1000 / nFPS, timer, v); // restart timer again
}



// Entry point - GLUT setup and initialization
int main(int argc, char** argv)
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("OpenGL Test");

	init();

	// set up the call-back functions 
	glutDisplayFunc(display);  // called when drawing 
	glutReshapeFunc(reshape);  // called when change window size
	glutKeyboardFunc(keyboard); // called when received keyboard interaction
	glutMouseFunc(mouse);	    // called when received mouse interaction
	glutTimerFunc(100, timer, nFPS); // a periodic timer. Usually used for updating animation

	glutMainLoop();

	return 0;
}