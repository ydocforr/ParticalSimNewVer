#include <windows.h>
#include "ExternHeaders\gl.h"
#include <stdio.h>
#include <math.h>
#include "Circle.h"
//#include "ExternHeaders\glut.h"
#include "ExternHeaders\freeglut.h"
CircleType *circles;
int numCircles;
void createCircles() {
	int x;
	numCircles = 150;
	circles = (struct CircleType*)malloc(sizeof(struct CircleType) * numCircles);
	for (x = 0; x < numCircles; x++)
	{
		circles[x].radius = .05;
		circles[x].position.x = (((double)rand() / (double)RAND_MAX) * 5 - 3);
		circles[x].position.y = (((double)rand() / (double)RAND_MAX) * 5 - 3);
		circles[x].velocity.x = 0;
		circles[x].velocity.y = 0;
		circles[x].acceleration.x = 0;
		circles[x].acceleration.y = 0;
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
			//prevent abs of distance from being than 1

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
void glDrawParticles(void) {
	int x;
	for (x = 0; x < numCircles; x++)
	{
		glPushMatrix();
		//glTranslatef(3, 5, 0);
		glTranslatef(circles[x].position.x, circles[x].position.y, 0);
		glRotatef(0 - 90, 0, 0, 1);
		glScalef(circles[x].radius, circles[x].radius, circles[x].radius);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		glBindTexture(GL_TEXTURE_2D, GL_TEXTURE1);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3f(-1, -1, 0);
		glTexCoord2d(1, 0);
		glVertex3f(1, -1, 0);
		glTexCoord2d(1, 1);
		glVertex3f(1, 1, 0);
		glTexCoord2d(0, 1);
		glVertex3f(-1, 1, 0);
		glEnd();
		glBlendFunc(GL_ONE, GL_ONE);
		glBindTexture(GL_TEXTURE_2D, GL_TEXTURE1);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3f(-1, -1, 0);
		glTexCoord2d(1, 0);
		glVertex3f(1, -1, 0);
		glTexCoord2d(1, 1);
		glVertex3f(1, 1, 0);
		glTexCoord2d(0, 1);
		glVertex3f(-1, 1, 0);
		glEnd();
		glEnable(GL_DEPTH_TEST);
		glPopMatrix();
	}
}

void display(void) {
	glClearDepth(1);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0, 0, -10);
	performGravity(-.0001f);
	//glUpdateParticles();
	glDrawParticles();
	glutSwapBuffers();
}

void init(void) {
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	createCircles();
	//glCreateParticles();
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
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(1000, 700);
	glutCreateWindow("Particle Simulation");
	init();
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}
