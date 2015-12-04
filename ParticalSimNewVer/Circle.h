#include "stdio.h"
struct Vector {
	double x;
	double y;
};
struct CircleType {
	struct Vector position;
	struct Vector velocity;
	struct Vector acceleration;
	double radius;
	//double area;
	double mass;
};
struct Vector subtractVectors(struct Vector vA, struct Vector vB);
struct Vector addVectors(struct Vector vA, struct Vector vB);
double magnitude(struct Vector v);