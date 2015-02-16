class Shape;


#ifndef SHAPE_H
#define SHAPE_H


#include "common.h"
#include "shapegroup.h"
#include "intersection.h"

class RayFile;
class Material;


/* 
 * Abstract Shape class that includes material properties
 */
class Shape : public ShapeGroup
{
 public:
			Shape ();
virtual			~Shape ();

virtual	double		intersect (Intersection& info) = 0;
virtual	void		glDraw () = 0;
virtual	int		makeLinks (RayFile* rf);

			// read and write .ray directives
virtual	istream&	read (istream& in) = 0;
virtual	ostream&	write (ostream& out) = 0;

/* inherited from ShapeGroup
 *
 * 	const char* 	getName ();
 */

 protected:

	char		matname[MAX_NAME];
	Material*	material;	// shape's surface material
	bool		textured;	// should shape be textured or not?
	bool		bumpMapped;  // is shape bumpmapped?
	double		bumpScale;

/* inherited from ShapeGroup
 *
 *	char		name[MAX_NAME];
 */

};


ostream& operator<< (ostream& out, Shape& s);
istream& operator>> (istream& in,  Shape& s);


#endif // SHAPE_H
