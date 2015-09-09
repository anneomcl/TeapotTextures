// TeapotTextures.cpp : Defines the entry point for the console application.
//
#define GLEW_STATIC
#include "stdafx.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <glm\glm.hpp>
#include <sstream>
#include <string>

#include "glew.h"
#include <gl\glut.h>
using namespace std;

float fRotateAngle = 0.f;

std::vector< glm::vec4 > vertices;
std::vector< glm::vec3 > normals; // Won't be used at the moment.
std::vector< GLushort > faces;

int nFPS = 30;
int frameCount = 0;
int currentTime;
int previousTime;
int fps;

GLuint tex;
GLuint envi;

int globalWidth;
int globalHeight;

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}


void load_obj(const char* filename, std::vector<glm::vec4> &vertices, std::vector<glm::vec3> &normals, std::vector<GLushort> &faces) {
	std::ifstream in(filename, std::ios::in);

	std::string line;
	while (getline(in, line)) {
		if (line.substr(0, 2) == "v ") 
		{
			std::string inputs = line.substr(2);
			char * delim = " ";
			glm::vec4 v;
			vector < std::string> inputsList = split(inputs, *delim);
			v.x = atof(inputsList[0].c_str());
			v.y = atof(inputsList[1].c_str());
			v.z = atof(inputsList[2].c_str());
			v.w = 1.f;
			vertices.push_back(v);
		}
		else if (line.substr(0, 2) == "f ") 
		{
			std::string inputs = line.substr(2);
			char * delim = " ";
			GLushort x, y, z;
			vector<std::string>inputsList = split(inputs, *delim);
			x = (GLushort) atoi(inputsList[1].c_str()) - 1;
			y = (GLushort) atoi(inputsList[2].c_str()) - 1;
			z = (GLushort) atoi(inputsList[3].c_str()) - 1;
			faces.push_back(x); 
			faces.push_back(y); 
			faces.push_back(z);
		}
		else if (line[0] == '#') {}
		else {}
	}

	normals.resize(vertices.size(), glm::vec3(0.0, 0.0, 0.0));
	for (int i = 0; i < faces.size(); i += 3) {
		GLushort ia = faces[i];
		GLushort ib = faces[i + 1];
		GLushort ic = faces[i + 2];
		glm::vec3 normal = glm::normalize(glm::cross(
			glm::vec3(vertices[ib]) - glm::vec3(vertices[ia]),
			glm::vec3(vertices[ic]) - glm::vec3(vertices[ia])));
		normals[ia] = normals[ib] = normals[ic] = normal;
	}
}

void init(void)
{
	// init your data, setup OpenGL environment here
	

	load_obj("teapot_0.obj", vertices, normals, faces);

	glClearColor(.5f, 0.f, 0.f, 1.0f);

	glMatrixMode(GL_PROJECTION);
	gluPerspective(50.0, 1.0, 1.0, 50.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_DEPTH_TEST);
}

void texturesAndLighting()
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat light[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat position[] = { 0.0, 10.0, 10.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light);

}

void makeTriangles()
{
	texturesAndLighting();

	float PI = 3.14;

	glBegin(GL_TRIANGLES);
	for (unsigned int i = 0; i < faces.size(); i+=3)
	{
		//float theta = atan2(normals[faces[i]].z, normals[faces[i]].x);
		//glMultiTexCoord2dvARB(GL_TEXTURE0_ARB, (1.0f, 2.0f));

		glNormal3f(normals[faces[i]].x, normals[faces[i]].y, normals[faces[i]].z);
		glVertex3f(vertices[faces[i]].x, vertices[faces[i]].y, vertices[faces[i]].z);
		glNormal3f(normals[faces[i + 1]].x, normals[faces[i + 1]].y, normals[faces[i + 1]].z);
		glVertex3f(vertices[faces[i + 1]].x, vertices[faces[i + 1]].y, vertices[faces[i + 1]].z);
		glNormal3f(normals[faces[i + 2]].x, normals[faces[i + 2]].y, normals[faces[i + 2]].z);
		glVertex3f(vertices[faces[i + 2]].x, vertices[faces[i + 2]].y, vertices[faces[i + 2]].z);
	}
	glEnd();

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
}

//spots.ppm is from https://www.cs.oberlin.edu/~bob/cs357.08/TextureExamples/TextureExamples.html
//method adapted from http://www.cs.rit.edu/usr/local/pub/wrc/courses/cg/textures/ppm/"
// discussed high level implementation and resources to look at 
// with Revanth Rameshkumar (in case there is any discrepency/issue)
unsigned char * ParseTex(const char * path)
{
	FILE * file;
	file = fopen(path, "rb");
	
	char buffer[100];
	int width, height, dim;

	fgets(buffer, 100, file);
	if (strncmp(buffer, "P6", 2)) return NULL;

	for (int i = 0; i < 3; i++)
	{
		fgets(buffer, 100, file);
		if (buffer[0] == '#') continue;
		if (i == 0) i += sscanf(buffer, "%d %d %d", &width, &height, &dim);
		if (i == 1) i += sscanf(buffer, "%d %d", &height, &dim);
		if (i == 2) i += sscanf(buffer, "%d", &dim);
	}

	unsigned char * image = (unsigned char *)malloc(sizeof(unsigned char)*width*height * 3);
	fread(image, sizeof(unsigned char), sizeof(unsigned char)*width*height*3, file);
	fclose(file);

	globalWidth = width;
	globalHeight = height;
	return image;
}

void setTextures()
{
	unsigned char * texFile = ParseTex("spots.ppm");

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, globalWidth, globalHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texFile);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

}

void display(void)
{
	// put your OpenGL display commands here
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// reset OpenGL transformation matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); // reset transformation matrix to identity

	glRotatef(fRotateAngle, 0.f, 0.f, 1.f);
	gluLookAt(1.f, 5.f, 5.f, 0.f, 1.25f, 0.f, 0.f, 1.f, 0.f);

	makeTriangles();

	glutSwapBuffers();	// swap front/back framebuffer to avoid flickering 
	glFlush();
	glutPostRedisplay();
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

void SetEnviron()
{
	unsigned char * env = ParseTex("environment.ppm");

	glGenTextures(1, &envi);
	glBindTexture(GL_TEXTURE_2D, envi);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, globalWidth, globalHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, env);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, globalWidth, globalHeight, GL_RGB, GL_UNSIGNED_BYTE, env);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

}

void setMulti()
{
	unsigned char * texFile = ParseTex("spots.ppm");

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, globalWidth, globalHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texFile);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	unsigned char * env = ParseTex("environment.ppm");

	glGenTextures(1, &envi);
	glBindTexture(GL_TEXTURE_2D, envi);
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, globalWidth, globalHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, env);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, globalWidth, globalHeight, GL_RGB, GL_UNSIGNED_BYTE, env);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
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

	if (key == 't')
	{
		setTextures();
	}
	if (key == 'e')
	{
		SetEnviron();
	}
	if (key == 'm')
	{
		setMulti();
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
	fRotateAngle += 3.1415 / 10;
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
	GLenum err = glewInit();
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