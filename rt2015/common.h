#ifndef COMMON_H
#define COMMON_H


#include <math.h>
#include <iostream>
#include <GLUT/glut.h>
#include <assert.h>

// accepted file versions
static const char VersionTag02[] = "HMCCS155FALL2002";
static const char VersionTag05[] = "HMCCS155FALL2005";
static const int MAX_VERSION_LENGTH = 80;

// constants

#define EPSILON         1e-5
#define MAX_LINE        1024
#define MAX_PATH        1024
#define MAX_NAME		128

#ifndef M_PI            // from math.h
#define M_PI          3.14159265358979323846
#define M_PI_2        1.57079632679489661923
#define M_PI_4        0.78539816339744830962
#define M_1_PI        0.31830988618379067154
#define M_2_PI        0.63661977236758134308
#define M_E           2.7182818284590452354
#endif // M_PI



// macros

#define sqr(x)          ((x)*(x))
#define cube(x)		((x) * (x) * (x))
#define max(a,b)        ((a)>(b)?(a):(b))
#define min(a,b)        ((a)<(b)?(a):(b))

#define deg2rad(x)      ((x)*M_PI/180.0)
#define rad2deg(x)      ((x)*180.0/M_PI)

#define clamp(x,a,b)    (  ((a)<(b))				\
	? ((x)<(a))?(a):(((x)>(b))?(b):(x))	\
	: ((x)<(b))?(b):(((x)>(a))?(a):(x))	\
	)

#define sign(x)         ((x)<0?-1:1)


// stl defs for convenience
#define VECTOR(t)		std::vector < t >
#define MAP(s,t)		std::map < s, t >


#endif // COMMON_H
