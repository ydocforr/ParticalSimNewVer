#include <windows.h>
#include "ExternHeaders\gl.h"
#include <stdio.h>
#include <math.h>
#include "Circle.h"
#include "Input.h"
//#include "ExternHeaders\glut.h"
#include "ExternHeaders\glext.h"
#include "ExternHeaders\freeglut.h"
CircleType *circles;
int numCircles;
double fps = 0;
#define N_PARTICLES 150
void createCircles() {
	int x;
	numCircles = N_PARTICLES;
	circles = (struct CircleType*)malloc(sizeof(struct CircleType) * numCircles);
	for (x = 0; x < numCircles; x++)
	{
		circles[x].radius = 0.5;
		circles[x].position.x = (((double)rand() / (double)RAND_MAX) * 5 - 3);
		circles[x].position.y = (((double)rand() / (double)RAND_MAX) * 5 - 3);
		circles[x].velocity.x = 0;
		circles[x].velocity.y = 0;
		circles[x].acceleration.x = 0;
		circles[x].acceleration.y = 0;
		circles[x].mass = (x * 1000) / 150;
	}
}
//right now assumes all objects are the same mass
void performGravity(float gravity) {
	int x, y;
	double con = 1;
	struct Vector tempVector, tempVector2;
	for (x = 0; x < numCircles; x++)
	{

		struct Vector acceleration;
		tempVector.x = 0;
		tempVector.y = 0;
		tempVector2.x = 0;
		tempVector2.y = 0;
		acceleration.x = 0;
		acceleration.y = 0;
		for (y = 0; y < numCircles; y++)
		{
			if (x == y)
			{
				continue;
			}
			tempVector = subtractVectors(circles[x].position, circles[y].position);
			//prevent abs of distance from being less than 1

			if (fabs(tempVector.x) < con)
			{
				if (tempVector.x >= 0)
				{
					tempVector.x = con;
				}
				else
				{
					tempVector.x = -con;
				}
			}
			if (fabs(tempVector.y) < con)
			{
				if (tempVector.y >= 0)
				{
					tempVector.y = con;
				}
				else
				{
					tempVector.y = -con;
				}
			}
			tempVector2.x = tempVector.x / pow(magnitude(tempVector), 3);
			tempVector2.y = tempVector.y / pow(magnitude(tempVector), 3);
			acceleration = addVectors(acceleration, tempVector2);
		}
		acceleration.x *= gravity;
		acceleration.y *= gravity;
		circles[x].acceleration = acceleration;
	}
	for (x = 0; x < numCircles; x++)
	{
		circles[x].velocity = addVectors(circles[x].velocity, circles[x].acceleration);
		circles[x].position = addVectors(circles[x].position, circles[x].velocity);
		if (circles[x].position.x < -3)
		{
			circles[x].velocity.x = circles[x].velocity.x * -0.5;
			circles[x].position.x = -2.97;
		}
		if (circles[x].position.x > 3)
		{
			circles[x].velocity.x = circles[x].velocity.x * -0.5;
			circles[x].position.x = 2.97;
		}
		if (circles[x].position.y < -3)
		{
			circles[x].velocity.y = circles[x].velocity.y * -0.5;
			circles[x].position.y = -2.97;
		}
		if (circles[x].position.y > 3)
		{
			circles[x].velocity.y = circles[x].velocity.y * -0.5;
			circles[x].position.y = 2.97;
		}
	}
}
struct Vector subtractVectors(struct Vector vA, struct Vector vB) {
	struct Vector v;
	v.x = vA.x - vB.x;
	v.y = vA.y - vB.y;
	return v;
}
struct Vector addVectors(struct Vector vA, struct Vector vB) {
	struct Vector v;
	v.x = vA.x + vB.x;
	v.y = vA.y + vB.y;
	return v;
}

double magnitude(struct Vector v)
{
	return sqrt(v.x * v.x + v.y * v.y);
}

double distance(struct Vector v, struct Vector v2) {
	return sqrt(pow(v2.x - v.x, 2) + pow(v2.y - v.y, 2));
}

void chooseColor(CircleType point) {
	switch (color_mode) {
	case 0:
		glColor3f(((point.mass * 1.0)) / 1000.0, 0.0, (1.0 - ((point.mass) / 1000.0)));
		break;
	case 1:
		struct Vector center;
		center.x = 0.0;
		center.y = 0.0;
		glColor3f(1.0 - distance(point.position, center) / 3.0, 0.0, distance(point.position, center) / 3.0);
		break;
	case 2:
		glColor3f(magnitude(point.velocity) * 10.0, 0.0, 1.0 - magnitude(point.velocity) * 10.0);
		break;
	}
}

void glDrawFPS(void) {
	glColor3f(1.0, 1.0, 1.0);
	char buf[100];
	sprintf_s(buf, "FPS: %f\n", fps);
	glRasterPos2f(-4.0, 3.9);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)buf);
}

void glDrawInterface(void) {
	char buf[100];
	sprintf_s(buf, "N=%d", N_PARTICLES);
	glRasterPos2f(0.0, 3.9);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)buf);
	glRasterPos2f(3.0f, 3.9);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"[P] to Pause\n");
	if (pause) {
		glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"[F] to Fullscreen\n[Q] to Quit\n[T] to Throttle\n[A] to Frameskip");
		char* mode = "";
		switch (color_mode) {
		case 0:
			mode = "Mass";
			break;
		case 1:
			mode = "Position";
			break;
		case 2:
			mode = "Velocity";
			break;
		}
		sprintf_s(buf, "[C] to change color mapping (%s)", mode);
		glRasterPos2f(-4.0f, -3.9f);
		glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)buf);
	}
}

void glDrawBounds(void) {
	glPushMatrix();
	glTranslatef(panx, -pany, zoom);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_LINE_LOOP);
	glVertex2i(3, 3);
	glVertex2i(3, -3);
	glVertex2i(-3, -3);
	glVertex2i(-3, 3);
	glEnd();
	glPopMatrix();
}

void glDrawParticles(void) {
	glPointSize(zoom + 5.0);
	glBegin(GL_POINTS);
	for (int x = 0; x < numCircles; x++)
	{
		chooseColor(circles[x]);
		glVertex3f(circles[x].position.x + panx, circles[x].position.y - pany, zoom);
	}
	glEnd();
}

void display(void) {
	glClearDepth(1);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0, 0, -10);
	if (!pause)
		performGravity(-.0001f);
	glDrawBounds();
	glDrawParticles();
	glDrawFPS();
	glDrawInterface();
	glutSwapBuffers();
}

// FPS calculation specific stuff
int frameCount = 0;
int currentTime = 0;
int previousTime = 0;

bool calculateFPS() {
	frameCount++;
	currentTime = glutGet(GLUT_ELAPSED_TIME);
	int timeInterval = currentTime - previousTime;
	if (frameCount == 60) {
		fps = frameCount / (timeInterval / 1000.0f);
		previousTime = currentTime;
		frameCount = 0;
		return true;
	}
	return false;
}

void idle(void) {
	if (calculateFPS())
		return;
	glutPostRedisplay();
}

void init(void) {
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(3);
	createCircles();
}

void reshape(int w, int h) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(45, (w / h), 1.0, 1000.0);
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(1000, 700);
	glutCreateWindow("Particle Simulation");
	// init
	init();
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	// setup input
	glutMouseFunc(mouseCallback);
	glutMotionFunc(mouseMoveCallback);
	glutKeyboardFunc(keyboardCallback);
	glutMainLoop();
	return 0;
}
