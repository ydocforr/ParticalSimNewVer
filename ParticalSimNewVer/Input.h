#pragma once

#include <stdio.h>
#include "ExternHeaders\freeglut.h"

extern double zoom;
extern double panx;
extern double pany;
extern bool pause;
extern bool frameskip;
extern int color_mode;
extern int calc_mode;
extern bool interop_mode;
extern int speed;

void endSimulation();
void mouseCallback(int button, int state, int x, int y);
void mouseMoveCallback(int x, int y);
void keyboardCallback(unsigned char key, int x, int y);