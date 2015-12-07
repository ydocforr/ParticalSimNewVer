#pragma once

int mouseState = -1;
int mouseButton = -1;

void mouseCallback(int button, int state, int x, int y);
void mouseMoveCallback(int x, int y);
void keyboardCallback(unsigned char key, int x, int y);