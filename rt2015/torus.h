class Torus;


#ifndef TORUS_H
#define TORUS_H


#include "common.h"
#include "shape.h"
#include "geometry.h"


class Torus : public Shape
{
 public:
			Torus ();
virtual			~Torus ();

	double		intersect (Intersection& info);
	void		glDraw ();

	istream&	read (istream& in);
	ostream&	write (ostream& out);
  
 	bool		inside(const Point3d& pos, bool surfaceOpen);

/* inherited from ShapeGroup
 *
 *	const char*	getName ();
 *	int 		makeLinks (RayFile* rf);
 */

 private:

/*
 * the torus is a circle of radius minorRad swept in a circle of radius
 * majorRad along the z-axis aligned vector through center
 */
	Point3d		center;
	double		majorRad;
	double		minorRad;

  	// Uses the normal and the distance along the height of the torus
	// ..to calculate the appropriate texture coordinates.
	// Called by intersect.
	TexCoord2d	getTexCoordinates(Point3d i);

	void		glutTexturedTorus (GLfloat r, GLfloat R,
				GLint nsides, GLint rings);

/*
 * inherited from Shape
 *
 *	char		matname[MAX_NAME];
 *	Material*	material;
 *	bool		textured;
 *	bool		transMapped;
 *	bool		bumpMapped;
 *	double		bumpScale;
 *	bool		cutMapped;
 *
 * inherited from ShapeGroup
 *

 *	char		name[MAX_NAME];
 */

};


istream& operator>> (istream& in,  Torus& s);
ostream& operator<< (ostream& out, Torus& s);


#endif // TORUS_H
