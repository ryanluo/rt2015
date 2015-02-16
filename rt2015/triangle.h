class Triangle;


#ifndef TRIANGLE_H
#define TRIANGLE_H


#include "common.h"
#include "shape.h"
#include "geometry.h"


/*
 * triangle class specified by three vertices and possibly three
 * texture coordinates.  stores a face normal.
 */
class Triangle : public Shape
{
 public:
			Triangle ();
virtual			~Triangle ();

	double		intersect ( Intersection& info);
	void		glDraw ();

			// read and write .ray directives
	istream&	read (istream& in);
	ostream&	write (ostream& out);

/* inherited from ShapeGroup
 *
 *	const char*	getName ();
 *	int		makeLinks (RayFile* rf);
 */

 private:

	Point3d		v[3];	// vertex positions
	TexCoord2d	t[3];	// texture coordinates
	Vector3d	n;	// face normal

/*
 * inherited from Shape
 *
 *	char		matname[MAX_NAME];
 *	Material*	material;
 *	bool		textured;
 *
 * inherited from ShapeGroup
 *
 *	char		name[MAX_NAME];
 */

};

istream& operator>> (istream& in,  Triangle& t);
ostream& operator<< (ostream& out, Triangle& t);


#endif // TRIANGLE_H
