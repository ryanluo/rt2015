class Cylinder;


#ifndef CYLINDER_H
#define CYLINDER_H


#include "common.h"
#include "shape.h"
#include "geometry.h"


class Cylinder : public Shape
{
public:
	Cylinder ();
	virtual			~Cylinder ();

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

	bool		inInfCyl(const Point3d& pos);


	Point3d		center;
	double		radius;
	double		length;
	bool		closed;

	// Uses the normal and the distance along the height of the cylinder
	// ..to calculate the appropriate texture coordinates.
	// Called by intersect.
	TexCoord2d	getTexCoordinates(Point3d i);

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
	*	BoundingBox*	bBox;
	*	char		name[MAX_NAME];
	*/

};


istream& operator>> (istream& in,  Cylinder& s);
ostream& operator<< (ostream& out, Cylinder& s);


#endif // CYLINDER_H
