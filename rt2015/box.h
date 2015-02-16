class Box;


#ifndef BOX_H
#define BOX_H


#include "common.h"
#include "shape.h"
#include "geometry.h"


/*
* Box shape class for drawing and raytracing
*/
class Box : public Shape
{
public:
	Box ();
	virtual			~Box ();

	double		intersect (Intersection& info);
	void		glDraw ();

	istream&	read (istream& in);
	ostream&	write (ostream& out);

	bool		inside (const Point3d& pos, bool surfaceOpen);

	/* inherited from ShapeGroup
	*
	*	const char*	getName ();
	*	int 		makeLinks (RayFile* rf);
	*/

private:

	Point3d		center;
	Vector3d	size;

	// Uses the normal of the Box at a point to calculate the appropriate
	// ..texture coordinates.  Called by intersect.
	TexCoord2d	getTexCoordinates (Point3d i);

	double		planeIntersect (Rayd& ray, Point3d& p0, Vector3d& n);
	void		glutTexturedCube (GLfloat size);

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


istream& operator>> (istream& in,  Box& s);
ostream& operator<< (ostream& out, Box& s);


#endif // BOX_H
