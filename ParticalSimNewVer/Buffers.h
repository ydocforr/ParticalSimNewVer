#pragma once

#include <stdio.h>
#include <windows.h>
#include "ExternHeaders\glew.h"
#include "ExternHeaders\gl.h"
#include "ExternHeaders\glext.h"
#include "ExternHeaders\freeglut.h"

GLuint createVBO(const void* data, int size, GLenum target, GLenum usage);
void deleteVBO(const GLuint vboId);