class ShapeGroup;


#ifndef SHAPEGROUP_H
#define SHAPEGROUP_H


#include "common.h"
#include "geometry.h"
#include "intersection.h"

class RayFile;


/*
 * abstract class that represents shapes and groups, since it's useful
 * to store them both together for our scene heirarchy.
 *
 * looking for parseShapeGroup() ?  it's in parse.h/.cpp
 */
class ShapeGroup
{
 public:

			ShapeGroup ();
virtual			~ShapeGroup ();

virtual	double		intersect (Intersection& info) = 0;
virtual	void		glDraw () = 0;
virtual	int		makeLinks (RayFile* rf) = 0;

			// accessor
virtual const char*	getName () { return name; }

			// read and write .ray directives
virtual	istream&	read (istream& in) = 0;
virtual	ostream&	write (ostream& out) = 0;

 protected:

	char		name[MAX_NAME];

};


ostream& operator<< (ostream& out, ShapeGroup& sg);
istream& operator>> (istream& in,  ShapeGroup& sg);


#endif // SHAPEGROUP_H
