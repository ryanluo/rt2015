class Group;


#ifndef GROUP_H
#define GROUP_H


#include "common.h"
#include "geometry.h"
#include "shapegroup.h"
#include <vector>
#include <map>

class RayFile;


/*
* class for storing a number of ShapeGroup objects, to provide some
* structure to the scene heirarchy.  intersect and gldraw calls are 
* passed on to the individual objects contained in the group
*/
class Group : public ShapeGroup
{
public:
	Group (RayFile *rf);
	virtual			~Group ();

	double		intersect (Intersection& info);
	void		glDraw ();
	int		makeLinks (RayFile* rf);

	// list accessors
	ShapeGroup*	operator[] (int i);
	ShapeGroup*	get   (int i);
	int		size  ();
	VECTOR(ShapeGroup*)::iterator begin();
	VECTOR(ShapeGroup*)::iterator end();
	/*
	ShapeGroup**	begin ();
	ShapeGroup**	end   ();
	*/
	void		add   (ShapeGroup* sg);

	// read and write .ray directives
	istream&	read (istream& in);
	ostream&	write (ostream& out);

	/* inherited from ShapeGroup
	*
	*	const char*	getName ();
	*/

private:

	Matrixd			transform;
	Matrixd			invTransform;
	Matrixd			invTransposeTransform;
	VECTOR(ShapeGroup*)	list;

	Matrixd readMatrixTransforms(istream &in);

	Matrixd readTranslate(istream &in);
	Matrixd readRotate(istream &in);
	Matrixd readScale(istream &in);

	RayFile *file;

	/* inherited from ShapeGroup
	*
	*	char		name[MAX_NAME];
	*/

};


istream& operator>> (istream& in,  Group& g);
ostream& operator<< (ostream& out, Group& g);


#endif // GROUP_H
