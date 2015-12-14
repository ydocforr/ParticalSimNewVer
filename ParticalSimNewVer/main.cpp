#include <windows.h>
#include "ExternHeaders\gl.h"
#include <stdio.h>
#include <math.h>
#include "Input.h"
#include "ExternHeaders\glext.h"
#include "ExternHeaders\freeglut.h"
//DIMS FOR PARTICAL CONTAINER
#define  yDim  6
//GRAVITY CONSTANTS
#define  con 1
#define  gravity -.0001f
//2048 MAX BLOCKS
#define numBlocks 2
//1024 MAX THREADS
#define numThreads 1024

//NUMBER OF PARTICALS (for /4 reduction method)
//#define  numCircles (numBlocks * numThreads / 4)

//NUMBER OF PARTICALS (for / 2 reduction method)
//#define  numCircles (numBlocks * numThreads / 2)

////NUMBER OF PARTICAL for no reduction && cpu partical updates
//#define numCircles (numBlocks * numThreads)
#define numCircles 500


// FPS calculation specific stuff
int frameCount = 0;
int currentTime = 0;
int previousTime = 0;
float fps = 0;
//Cuda Device Pointers
float *deviceParticleInput = NULL;
float *deviceParticleOutput = NULL;
//Host Partial Container
float *particleContainer = NULL;
//Gravity calculation vector
float vectorArray[6];
//Main Headers
void performGravity();
void initContainer();
void createCircles();
void UpdateParticlesCPU();
//GUI Headers
void chooseColor(int startIdx);
void idle(void);
bool calculateFPS();
void glDrawParticles(void);
void glDrawBounds(void);
void glDrawInterface(void);
void glDrawFPS(void);
float distance(float x1, float y1, float x2, float y2);
float magnitude(float x, float y);
/*
//CUDA KERNEL  DIVIDE FOUR REDUCTION METHOD 
__global__ void UpdateParticlesDivideFourReduction(float*particalInContainer, float*particalOutContainer)
{
	int threadID = (threadIdx.x + blockDim.x * blockIdx.x) * yDim;
	int i = ((threadID / yDim) / 4) * yDim;
	int startingIdx = (threadID / yDim % 4) * numCircles * yDim / 4;
	float accelerationX = 0;
	float accelerationY = 0;
	float vectorArray[6];
	int x, y;
	for (y = 0; y < 6; y++)
	{
		vectorArray[y] = 0;
	}
	for (y = startingIdx; y < startingIdx + (numCircles * yDim) / 4; y += yDim)
	{
		if (i == y)
		{
			continue;
		}
		//store x position difference
		vectorArray[0] = particalInContainer[i + 0] - particalInContainer[y + 0];
		//store y position difference
		vectorArray[1] = particalInContainer[i + 1] - particalInContainer[y + 0];
		//prevent abs of distance from being than 1
		if (fabs(vectorArray[0]) < con)
		{
			if (vectorArray[0] >= 0)
			{
				vectorArray[0] = con;
			}
			else
			{
				vectorArray[0] = -con;
			}
		}
		if (fabs(vectorArray[1]) < con)
		{
			if (vectorArray[1] >= 0)
			{
				vectorArray[1] = con;
			}
			else
			{
				vectorArray[1] = -con;
			}
		}
		vectorArray[2] = vectorArray[0] / pow(sqrt(vectorArray[0] * vectorArray[0] + vectorArray[1] * vectorArray[1]), 3);
		vectorArray[3] = vectorArray[1] / pow(sqrt(vectorArray[0] * vectorArray[0] + vectorArray[1] * vectorArray[1]), 3);
		vectorArray[4] = vectorArray[4] + vectorArray[2];
		vectorArray[5] = vectorArray[5] + vectorArray[3];
	}
	vectorArray[4] *= gravity;
	vectorArray[5] *= gravity;
	accelerationX = vectorArray[4];
	accelerationY = vectorArray[5];
	//VX
	if (threadID / yDim % 4 == 3 || threadID / yDim % 4 == 2 || threadID / yDim % 4 == 1)
	{
		particalOutContainer[i + 2] = accelerationX;
		particalOutContainer[i + 3] = accelerationY;

	}
	__syncthreads();
	//VY
	if (threadID / yDim % 4 == 0)
	{
		//Update Partical
		particalOutContainer[i + 2] += particalInContainer[i + 2] + accelerationX;
		particalOutContainer[i + 3] += particalInContainer[i + 3] + accelerationY;
		particalOutContainer[i + 0] = particalInContainer[i + 0] + particalOutContainer[i + 2];
		particalOutContainer[i + 1] = particalInContainer[i + 1] + particalOutContainer[i + 3];
		particalOutContainer[i + 4] = particalInContainer[i + 4];
		particalOutContainer[i + 5] = particalInContainer[i + 5];
		//Bounds Checking for window
		if (particalOutContainer[i + 0] < -3)
		{
			particalOutContainer[i + 2] = particalOutContainer[i + 2] * -0.5;
			particalOutContainer[i + 0] = -3;
		}
		if (particalOutContainer[i + 0] > 3)
		{
			particalOutContainer[i + 2] = particalOutContainer[i + 2] * -0.5;
			particalOutContainer[i + 0] = 3;
		}
		if (particalOutContainer[i + 1] < -3)
		{
			particalOutContainer[i + 3] = particalOutContainer[i + 3] * -0.5;
			particalOutContainer[i + 1] = -3;
		}
		if (particalOutContainer[i + 1] > 3)
		{
			particalOutContainer[i + 3] = particalOutContainer[i + 3] * -0.5;
			particalOutContainer[i + 1] = 3;
		}
	}
}
//CUDA KERNEL  DIVIDE TWO REDUCTION METHOD 
__global__ void UpdateParticlesDivideTwoReduction(float*particalInContainer, float*particalOutContainer)
{
	int threadID = (threadIdx.x + blockDim.x * blockIdx.x) * yDim;
	int i = ((threadID / yDim) / 2) * yDim;
	int startingIdx = (threadID / yDim % 2) * numCircles * yDim / 2;
	float accelerationX = 0;
	float accelerationY = 0;
	float vectorArray[6];
	int x, y;
	for (y = 0; y < 6; y++)
	{
		vectorArray[y] = 0;
	}
	for (y = startingIdx; y < startingIdx + (numCircles * yDim) / 2; y += yDim)
	{
		if (i == y)
		{
			continue;
		}
		//store x position difference
		vectorArray[0] = particalInContainer[i + 0] - particalInContainer[y + 0];
		//store y position difference
		vectorArray[1] = particalInContainer[i + 1] - particalInContainer[y + 0];
		//prevent abs of distance from being than 1
		if (fabs(vectorArray[0]) < con)
		{
			if (vectorArray[0] >= 0)
			{
				vectorArray[0] = con;
			}
			else
			{
				vectorArray[0] = -con;
			}
		}
		if (fabs(vectorArray[1]) < con)
		{
			if (vectorArray[1] >= 0)
			{
				vectorArray[1] = con;
			}
			else
			{
				vectorArray[1] = -con;
			}
		}
		vectorArray[2] = vectorArray[0] / pow(sqrt(vectorArray[0] * vectorArray[0] + vectorArray[1] * vectorArray[1]), 3);
		vectorArray[3] = vectorArray[1] / pow(sqrt(vectorArray[0] * vectorArray[0] + vectorArray[1] * vectorArray[1]), 3);
		vectorArray[4] = vectorArray[4] + vectorArray[2];
		vectorArray[5] = vectorArray[5] + vectorArray[3];
	}
	vectorArray[4] *= gravity;
	vectorArray[5] *= gravity;
	accelerationX = vectorArray[4];
	accelerationY = vectorArray[5];
	//VX
	if (threadID / yDim % 2 == 0)
	{
		particalOutContainer[i + 2] = accelerationX;
		particalOutContainer[i + 3] = accelerationY;

	}
	__syncthreads();
	//VY
	if (threadID / yDim % 2 == 1)
	{
		//Update Partical
		particalOutContainer[i + 2] += particalInContainer[i + 2] + accelerationX;
		particalOutContainer[i + 3] += particalInContainer[i + 3] + accelerationY;
		particalOutContainer[i + 0] = particalInContainer[i + 0] + particalOutContainer[i + 2];
		particalOutContainer[i + 1] = particalInContainer[i + 1] + particalOutContainer[i + 3];
		particalOutContainer[i + 4] = particalInContainer[i + 4];
		particalOutContainer[i + 5] = particalInContainer[i + 5];
		//Bounds Checking for window
		if (particalOutContainer[i + 0] < -3)
		{
			particalOutContainer[i + 2] = particalOutContainer[i + 2] * -0.5;
			particalOutContainer[i + 0] = -3;
		}
		if (particalOutContainer[i + 0] > 3)
		{
			particalOutContainer[i + 2] = particalOutContainer[i + 2] * -0.5;
			particalOutContainer[i + 0] = 3;
		}
		if (particalOutContainer[i + 1] < -3)
		{
			particalOutContainer[i + 3] = particalOutContainer[i + 3] * -0.5;
			particalOutContainer[i + 1] = -3;
		}
		if (particalOutContainer[i + 1] > 3)
		{
			particalOutContainer[i + 3] = particalOutContainer[i + 3] * -0.5;
			particalOutContainer[i + 1] = 3;
		}
	}
}
//CUDA KERNEL SINGLE THREAD PER PARTICAL WITH SHARED MEMORY
__global__ void UpdateParticlesNoReductionWithSharedMem(float*particalInContainer, float*particalOutContainer)
{
	int i = (threadIdx.x + blockDim.x * blockIdx.x) * yDim;
	__shared__ float sharedParticalMem[numCircles * yDim];

	float accelerationX = 0;
	float accelerationY = 0;
	float vectorArray[6];
	int x, y;
	for (y = 0; y < 6; y++)
	{
		vectorArray[y] = 0;
	}
	for (y = 0; y < (numCircles * yDim); y += yDim)
	{
		if (i == y)
		{
			continue;
		}
		//store x position difference
		vectorArray[0] = particalInContainer[i + 0] - particalInContainer[y + 0];
		//store y position difference
		vectorArray[1] = particalInContainer[i + 1] - particalInContainer[y + 0];
		//prevent abs of distance from being than 1
		if (fabs(vectorArray[0]) < con)
		{
			if (vectorArray[0] >= 0)
			{
				vectorArray[0] = con;
			}
			else
			{
				vectorArray[0] = -con;
			}
		}
		if (fabs(vectorArray[1]) < con)
		{
			if (vectorArray[1] >= 0)
			{
				vectorArray[1] = con;
			}
			else
			{
				vectorArray[1] = -con;
			}
		}
		vectorArray[2] = vectorArray[0] / pow(sqrt(vectorArray[0] * vectorArray[0] + vectorArray[1] * vectorArray[1]), 3);
		vectorArray[3] = vectorArray[1] / pow(sqrt(vectorArray[0] * vectorArray[0] + vectorArray[1] * vectorArray[1]), 3);
		vectorArray[4] = vectorArray[4] + vectorArray[2];
		vectorArray[5] = vectorArray[5] + vectorArray[3];
	}
	vectorArray[4] *= gravity;
	vectorArray[5] *= gravity;
	accelerationX = vectorArray[4];
	accelerationY = vectorArray[5];
	particalOutContainer[i + 2] = accelerationX;
	particalOutContainer[i + 3] = accelerationY;
	//Update Partical
	particalOutContainer[i + 2] += particalInContainer[i + 2] + accelerationX;
	particalOutContainer[i + 3] += particalInContainer[i + 3] + accelerationY;
	particalOutContainer[i + 0] = particalInContainer[i + 0] + particalOutContainer[i + 2];
	particalOutContainer[i + 1] = particalInContainer[i + 1] + particalOutContainer[i + 3];
	particalOutContainer[i + 4] = particalInContainer[i + 4];
	particalOutContainer[i + 5] = particalInContainer[i + 5];
	//Bounds Checking for window
	if (particalOutContainer[i + 0] < -3)
	{
		particalOutContainer[i + 2] = particalOutContainer[i + 2] * -0.5;
		particalOutContainer[i + 0] = -3;
	}
	if (particalOutContainer[i + 0] > 3)
	{
		particalOutContainer[i + 2] = particalOutContainer[i + 2] * -0.5;
		particalOutContainer[i + 0] = 3;
	}
	if (particalOutContainer[i + 1] < -3)
	{
		particalOutContainer[i + 3] = particalOutContainer[i + 3] * -0.5;
		particalOutContainer[i + 1] = -3;
	}
	if (particalOutContainer[i + 1] > 3)
	{
		particalOutContainer[i + 3] = particalOutContainer[i + 3] * -0.5;
		particalOutContainer[i + 1] = 3;
	}

}

//CUDA KERNEL SINGLE THREAD PER PARTICAL
__global__ void UpdateParticlesNoReduction(float*particalInContainer, float*particalOutContainer)
{
	int i = (threadIdx.x + blockDim.x * blockIdx.x) * yDim;

	float accelerationX = 0;
	float accelerationY = 0;
	float vectorArray[6];
	int x, y;
	for (y = 0; y < 6; y++)
	{
		vectorArray[y] = 0;
	}
	for (y = 0; y < (numCircles * yDim); y += yDim)
	{
		if (i == y)
		{
			continue;
		}
		//store x position difference
		vectorArray[0] = particalInContainer[i + 0] - particalInContainer[y + 0];
		//store y position difference
		vectorArray[1] = particalInContainer[i + 1] - particalInContainer[y + 0];
		//prevent abs of distance from being than 1
		if (fabs(vectorArray[0]) < con)
		{
			if (vectorArray[0] >= 0)
			{
				vectorArray[0] = con;
			}
			else
			{
				vectorArray[0] = -con;
			}
		}
		if (fabs(vectorArray[1]) < con)
		{
			if (vectorArray[1] >= 0)
			{
				vectorArray[1] = con;
			}
			else
			{
				vectorArray[1] = -con;
			}
		}
		vectorArray[2] = vectorArray[0] / pow(sqrt(vectorArray[0] * vectorArray[0] + vectorArray[1] * vectorArray[1]), 3);
		vectorArray[3] = vectorArray[1] / pow(sqrt(vectorArray[0] * vectorArray[0] + vectorArray[1] * vectorArray[1]), 3);
		vectorArray[4] = vectorArray[4] + vectorArray[2];
		vectorArray[5] = vectorArray[5] + vectorArray[3];
	}
	vectorArray[4] *= gravity;
	vectorArray[5] *= gravity;
	accelerationX = vectorArray[4];
	accelerationY = vectorArray[5];
	particalOutContainer[i + 2] = accelerationX;
	particalOutContainer[i + 3] = accelerationY;
	//Update Partical
	particalOutContainer[i + 2] += particalInContainer[i + 2] + accelerationX;
	particalOutContainer[i + 3] += particalInContainer[i + 3] + accelerationY;
	particalOutContainer[i + 0] = particalInContainer[i + 0] + particalOutContainer[i + 2];
	particalOutContainer[i + 1] = particalInContainer[i + 1] + particalOutContainer[i + 3];
	particalOutContainer[i + 4] = particalInContainer[i + 4];
	particalOutContainer[i + 5] = particalInContainer[i + 5];
	//Bounds Checking for window
	if (particalOutContainer[i + 0] < -3)
	{
		particalOutContainer[i + 2] = particalOutContainer[i + 2] * -0.5;
		particalOutContainer[i + 0] = -3;
	}
	if (particalOutContainer[i + 0] > 3)
	{
		particalOutContainer[i + 2] = particalOutContainer[i + 2] * -0.5;
		particalOutContainer[i + 0] = 3;
	}
	if (particalOutContainer[i + 1] < -3)
	{
		particalOutContainer[i + 3] = particalOutContainer[i + 3] * -0.5;
		particalOutContainer[i + 1] = -3;
	}
	if (particalOutContainer[i + 1] > 3)
	{
		particalOutContainer[i + 3] = particalOutContainer[i + 3] * -0.5;
		particalOutContainer[i + 1] = 3;
	}

}
*/
//CPU Partical Update Function Call
void UpdateParticlesCPU()
{
	float accelerationX;
	float accelerationY;
	float vectorArray[6];
	int i, x, y;
	for (i = 0; i < numCircles*yDim; i += yDim)
	{
		for (y = 0; y < 6; y++)
		{
			vectorArray[y] = 0;
		}
		for (y = 0; y < numCircles * yDim; (y += yDim))
		{
			if (i == y)
			{
				continue;
			}
			//store x position difference
			vectorArray[0] = particleContainer[i + 0] - particleContainer[y + 0];
			//store y position difference
			vectorArray[1] = particleContainer[i + 1] - particleContainer[y + 0];
			//tempVector = subtractVectors(circles[x].position, circles[y].position);
			//prevent abs of distance from being than 1
			if (fabs(vectorArray[0]) < con)
			{
				if (vectorArray[0] >= 0)
				{
					vectorArray[0] = con;
				}
				else
				{
					vectorArray[0] = -con;
				}
			}
			if (fabs(vectorArray[1]) < con)
			{
				if (vectorArray[1] >= 0)
				{
					vectorArray[1] = con;
				}
				else
				{
					vectorArray[1] = -con;
				}
			}
			vectorArray[2] = vectorArray[0] / pow(sqrt(vectorArray[0] * vectorArray[0] + vectorArray[1] * vectorArray[1]), 3);
			//tempVector2.x = tempVector.x / pow(magnitude(tempVector), 3);
			vectorArray[3] = vectorArray[1] / pow(sqrt(vectorArray[0] * vectorArray[0] + vectorArray[1] * vectorArray[1]), 3);
			vectorArray[4] = vectorArray[4] + vectorArray[2];
			vectorArray[5] = vectorArray[5] + vectorArray[3];
			//acceleration = addVectors(acceleration, tempVector2);
		}
		vectorArray[4] *= gravity;
		vectorArray[5] *= gravity;
		accelerationX = vectorArray[4];
		accelerationY = vectorArray[5];
		//VX
		particleContainer[i + 2] = particleContainer[i + 2] + accelerationX;
		particleContainer[i + 3] = particleContainer[i + 3] + accelerationY;
	}
	for (i = 0; i < numCircles*yDim; i += yDim)
	{
		//VY
		particleContainer[i + 0] = particleContainer[i + 0] + particleContainer[i + 2];
		particleContainer[i + 1] = particleContainer[i + 1] + particleContainer[i + 3];
		if (particleContainer[i + 0] < -3)
		{
			particleContainer[i + 2] = particleContainer[i + 2] * -0.5;
			//circles[x].velocity.x = circles[x].velocity.x * -0.5;
			particleContainer[i + 0] = -3;
			//circles[x].position.x = -2.97;
		}
		if (particleContainer[i + 0] > 3)
		{
			particleContainer[i + 2] = particleContainer[i + 2] * -0.5;
			//circles[x].velocity.x = circles[x].velocity.x * -0.5;
			particleContainer[i + 0] = 3;
		}
		if (particleContainer[i + 1] < -3)
		{
			particleContainer[i + 3] = particleContainer[i + 3] * -0.5;
			//circles[x].velocity.x = circles[x].velocity.x * -0.5;
			particleContainer[i + 1] = -3;
		}
		if (particleContainer[i + 1] > 3)
		{
			particleContainer[i + 3] = particleContainer[i + 3] * -0.5;
			//circles[x].velocity.x = circles[x].velocity.x * -0.5;
			particleContainer[i + 1] = 3;
		}
		particleContainer[i + 4] = particleContainer[i + 4];
		particleContainer[i + 5] = particleContainer[i + 5];
	}
}
//When the user ends the simulation, the Input class calls this method to free structs
void endSimulation()
{
	/*
	cudaError_t err = cudaSuccess;
	free(particleContainer);
	err = cudaFree(deviceParticleInput);
	if (err != cudaSuccess)
	{
		fprintf(stderr, "Failed to free device vector ParticalIn (error code %s)!\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}
	err = cudaFree(deviceParticleOutput);
	if (err != cudaSuccess)
	{
		fprintf(stderr, "Failed to free device vector ParticalOut(error code %s)!\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}
	*/
}
void initContainer()
{
	/*
	int i;
	cudaError_t err = cudaSuccess;
	//Cuda-Malloc Device
	deviceParticleInput = NULL;
	err = cudaMalloc((void**)&deviceParticleInput, sizeof(float)*(numCircles*yDim));
	if (err != cudaSuccess)
	{
		fprintf(stderr, "CudaMalloc for deviceInput failed (error code %s)!\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}
	deviceParticleOutput = NULL;
	err = cudaMalloc((void**)&deviceParticleOutput, sizeof(float)*(numCircles*yDim));
	if (err != cudaSuccess)
	{
		fprintf(stderr, "CudaMalloc for device AWDWA failed (error code %s)!\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}
	*/
	particleContainer = (float *)malloc(numCircles * yDim * sizeof(float));
	createCircles();
}
void createCircles() {
	int x;
	for (x = 0; x < numCircles*yDim; x += yDim)
	{
		particleContainer[x] = (((float)rand() / (float)RAND_MAX) * 5 - 3);
		particleContainer[x + 1] = (((float)rand() / (float)RAND_MAX) * 5 - 3);
		particleContainer[x + 2] = 0;
		particleContainer[x + 3] = 0;
		particleContainer[x + 4] = .05;
		particleContainer[x + 5] = (x * 1000) / (yDim*numCircles);
		printf("Mass: %f\n", particleContainer[x + 5]);
	}
}
//right now assumes all objects are the same mass
void performGravity() {
	float elapsedTime;
	/*
	cudaEvent_t start;
	cudaEvent_t stop;
	
	cudaError_t err = cudaSuccess;
	//cudaMemcpy(d_arr, h_arr, sizeof(int)*N, cudaMemcpyHostToDevice);
	err = cudaMemcpy(deviceParticleInput, particleContainer, sizeof(float)*(numCircles*yDim), cudaMemcpyHostToDevice);
	if (err != cudaSuccess)
	{
		fprintf(stderr, "CudaMalloc for host to dev failed (error code %s)!\n", cudaGetErrorString(err));
		//exit(EXIT_FAILURE);
	}
	err = cudaEventCreate(&start);
	if (err != cudaSuccess) {
		printf("Failed to create cuda event start (error code %s)!\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}
	err = cudaEventCreate(&stop);
	if (err != cudaSuccess) {
		printf("Failed to create cuda event stop (error code %s)!\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}
	err = cudaEventRecord(start, 0);
	if (err != cudaSuccess) {
		printf("Failed to record event start (error code %s)!\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}
	//Cuda Kernel Call Divide Four Reduction
	//UpdateParticlesDivideFourReduction<<<numBlocks,numThreads>>>(deviceParticleInput,deviceParticleOutput);

	//DOUBLE THREAD:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	//Cuda Kernel Call Divide Two Reduction
	UpdateParticlesDivideTwoReduction << <numBlocks, numThreads >> >(deviceParticleInput, deviceParticleOutput);

	//SINGLE THREAD:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	//Cuda Kernel Call Single Thread Per Partical, No Reduction Shared Memory
	//UpdateParticlesNoReductionWithSharedMem<<<numBlocks,numThreads>>>(deviceParticleInput,deviceParticleOutput);
	//Cuda Kernel Call Single Thread Per Partical, No Reduction
	//UpdateParticlesNoReduction<<<numBlocks,numThreads>>>(deviceParticleInput,deviceParticleOutput);
	//CPU UPDATE PARTICALS CALL, 
	err = cudaEventRecord(stop, 0);
	if (err != cudaSuccess) {
		printf("Failed to record event stop (error code %s)!\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}
	err = cudaEventSynchronize(stop);
	if (err != cudaSuccess) {
		printf("Failed to synchronize (error code %s)!\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}
	err = cudaEventElapsedTime(&elapsedTime, start, stop);
	if (err != cudaSuccess) {
		printf("Failed to calculate elapsed time (error code %s)!\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}
	printf("\nMatrix add for atomic %3.1f ms \n", elapsedTime);

	err = cudaMemcpy(particleContainer, deviceParticleOutput, sizeof(float)*(numCircles*yDim), cudaMemcpyDeviceToHost);
	if (err != cudaSuccess)
	{
		fprintf(stderr, "CudaMalloc for host to dev failed (error code %s)!\n", cudaGetErrorString(err));
		//exit(EXIT_FAILURE);
	}
	*/
	UpdateParticlesCPU();
}
void glDrawParticles(void) {
	glPointSize(zoom + 5.0);
	glBegin(GL_POINTS);
	int x;
	for (x = 0; x < numCircles * yDim; x += yDim)
	{
		chooseColor(x);
		glVertex3f(particleContainer[x + 0] + panx, particleContainer[x + 1] - pany, zoom);
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
		performGravity();
	glDrawBounds();
	glDrawParticles();
	glDrawFPS();
	glDrawInterface();
	glutSwapBuffers();
}
void init(void) {
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(3);
	initContainer();
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
//Nolans GUI Stuff
void chooseColor(int containerStartIDX) {
	switch (color_mode) {
	case 0:
		glColor3f((particleContainer[containerStartIDX + 5]) / 1000, 0.0, (1.0 - ((particleContainer[containerStartIDX + 5]) / 1000.0)));
		break;
	case 1:
		//To find distance function
		glColor3f(1.0 - distance(particleContainer[containerStartIDX + 0], particleContainer[containerStartIDX + 1], 0.0, 0.0) / 3.0, 0.0, distance(particleContainer[containerStartIDX + 0], particleContainer[containerStartIDX + 1], 0.0, 0.0) / 3.0);
		break;
	case 2:
		glColor3f(magnitude(particleContainer[containerStartIDX + 2], particleContainer[containerStartIDX + 3]) * 10.0, 0.0, 1.0 - magnitude(particleContainer[containerStartIDX + 2], particleContainer[containerStartIDX + 3]) * 10.0);
		break;
	}
}
float magnitude(float x, float y)
{
	return sqrt(x * x + y * y);
}
float distance(float x1, float y1, float x2, float y2) {
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
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
	sprintf_s(buf, "N=%d", numCircles);
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
