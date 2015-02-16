class Light;


#ifndef LIGHT_H
#define LIGHT_H


#include "common.h"
#include "geometry.h"
#include "intersection.h"

class ShapeGroup;


/*
* Abstract class for lights.
* All lights have color, can be input/output, can calculate diffuse/specular
* contributions, and determine if a point is shadowed (wrt that light)
*/
class Light
{
public:
	Light ();
	virtual			~Light ();

	virtual	Color3d		getDiffuse (Intersection& iInfo) = 0;
	virtual	Color3d		getSpecular (Intersection& iInfo) = 0;
	virtual	bool		getShadow (Intersection& iInfo,
		ShapeGroup* root) = 0;

	const char*	getName ();

	virtual	istream&	read  (istream& in) = 0;
	virtual	ostream&	write (ostream& out) = 0;

	virtual	void		glLoad (GLenum light) = 0;
	virtual	void		glDraw () = 0;

protected:

	Color3d		color;
	char		name[MAX_NAME];

};


istream& operator>> (istream& in,  Light& l);
ostream& operator<< (ostream& out, Light& l);


#endif // LIGHT_H
