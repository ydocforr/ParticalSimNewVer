#include "Buffers.h"

GLuint createVBO(const void* data, int dataSize, GLenum target, GLenum usage)
{
	GLuint id = 0;
	int bufferSize = 0;

	glGenBuffers(1, &id);
	glBindBuffer(target, id);
	glBufferData(target, dataSize, data, usage);

	glGetBufferParameteriv(target, GL_BUFFER_SIZE, &bufferSize);
	if (dataSize != bufferSize)
	{
		glDeleteBuffers(1, &id);
		id = 0;
		printf("VBO does match input array size\n");
	}

	return id;
}

void deleteVBO(const GLuint vboId)
{
	glDeleteBuffers(1, &vboId);
}