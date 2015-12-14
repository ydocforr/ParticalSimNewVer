#include "Input.h"

// externs
double zoom = 0.0;
double panx = 0.0;
double pany = 0.0;
bool pause = false;
bool frameskip = false;
int color_mode = 0;
int speed = 60;

double dragging = false;
int startx = 0;
int starty = 0;
bool fullscreen = false;

void mouseCallback(int button, int state, int x, int y) {
	switch (button) {
	case 0:
		if (state == 0) {
			startx = x;
			starty = y;
			glutSetCursor(GLUT_CURSOR_NONE);
			dragging = true;
		}
		else {
			glutSetCursor(GLUT_CURSOR_INHERIT);
			dragging = false;
		}
		break;
	case 3:
		zoom += 0.1;
		break;
	case 4:
		zoom += -0.1;
		break;
	default:
		break;
	}
}

void mouseMoveCallback(int x, int y) {
	panx += (x - startx) / 100.0;
	pany += (y - starty) / 100.0;
	startx = x;
	starty = y;
}

void keyboardCallback(unsigned char key, int x, int y) {
	switch (key) {
	case 'p': // pause
	case 'P':
		pause = !pause;
		break;
	case 'c': // switch color scheme
	case 'C':
		color_mode += 1;
		if (color_mode > 2)
			color_mode = 0;
		break;
	case 'f': // toggle fullscreen
	case 'F':
		if (!fullscreen) {
			glutFullScreen();
			fullscreen = true;
		}
		else {
			glutReshapeWindow(1000, 700);
			glutPositionWindow(0, 0);
			fullscreen = false;
		}
		break;
	case 'q': // exit
	case 'Q':
		endSimulation();
		exit(0);
		break;
	case 't': // throttle
	case 'T':
		speed += 5;
		if (speed > 60)
			speed = 5;
		break;
	case 'a': // frameskip
	case 'A':
		pause = true;
		frameskip = true;
		break;
	}
}