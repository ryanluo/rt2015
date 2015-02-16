typedef struct _Intersection_ Intersection;


#ifndef INTERSECTION_H
#define INTERSECTION_H


#include "common.h"
#include "geometry.h"

class Material;


/*
* this struct encapsulates all the useful information about a
* ray intersection, needed for calculating the color and recursive
* rays at the intersection point.
*/
struct _Intersection_
{
	Rayd				theRay;		// intersection ray
	Point3d       	iCoordinate;// intersection coordinates
	Vector3d      	normal;		// normal to surface at intersection
	Material*	    	material;	// material of surface
	bool          	textured;	// if the material is textured
	TexCoord2d    	texCoordinate;	// tex coords at intersection
	bool				entering;	//
	int				primitive;	
};


#endif // INTERSECTION_H
