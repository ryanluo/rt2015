class Sphere;


#ifndef SPHERE_H
#define SPHERE_H


#include "common.h"
#include "shape.h"
#include "geometry.h"


/*
 * sphere shape class for drawing and raytracing
 */
class Sphere : public Shape
{
 public:
				Sphere ();
	virtual			~Sphere ();

	double		intersect (Intersection& info);
	void		glDraw ();

			// read and write .ray directives
	istream&	read  (istream& in);
	ostream&	write (ostream& out);

/* inherited from ShapeGroup
 *
 *	const char*	getName ();
 *	int 		makeLinks (RayFile* rf);
 */

 private:

	Point3d		center;
	double		radius;

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


istream& operator>> (istream& in,  Sphere& s);
ostream& operator<< (ostream& out, Sphere& s);


#endif // SPHERE_H
