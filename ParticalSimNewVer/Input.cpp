#include "Input.h"
#include <stdio.h>

void mouseCallback(int button, int state, int x, int y) {
	printf("Button: %d, %d, (%d %d)\n", button, state, x, y);
}

void mouseMoveCallback(int x, int y) {
	printf("Move: (%d %d)\n", x, y);
}

void keyboardCallback(unsigned char key, int x, int y) {
	printf("Keyboard: %c\n", key);
}