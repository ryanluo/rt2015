class Cone;


#ifndef CONE_H
#define CONE_H


#include "common.h"
#include "shape.h"
#include "geometry.h"


class Cone : public Shape
{
public:
	Cone ();
	virtual			~Cone ();

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

	bool		inInfCone(const Point3d& pos);

	Point3d		center;
	double		radius;
	double		height;
	bool		closed;

	// Uses the normal and the distance along the height of the cone
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


istream& operator>> (istream& in,  Cone& s);
ostream& operator<< (ostream& out, Cone& s);


#endif // CONE_H
