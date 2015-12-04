#include <stdio.h>
#include "Circle.h"
#include "math.h"
//#define gravity 5;
//#define numCircles (2);
/*
int numCircles;

void createCircles() {
numCircles = 2;
circles = (struct CircleType*)malloc(sizeof(struct CircleType) * numCircles);

circles[0].position.x = 0;
circles[0].position.y = 0;
circles[0].velocity.x = 0;
circles[0].velocity.y = 0;
circles[0].acceleration.x = 0;
circles[0].acceleration.y = 0;

circles[1].position.x = 0;
circles[1].position.y = 0;
circles[1].velocity.x = 0;
circles[1].velocity.y = 0;
circles[1].acceleration.x = 0;
circles[1].acceleration.y = 0;

}

//right now assumes all objects are the same mass
void performGravity(int gravity) {
int x, y;
struct Vector tempVector, tempVector2;
for (x = 0; x < numCircles; x++)
{

struct Vector acceleration;
for (y = 0; y < numCircles; y++)
{
if (x == y)
{
continue;
}
tempVector = subtractVectors(circles[x].position, circles[y].position);
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
*/