class RayFileInstance;


#ifndef RAYFILEINSTANCE_H
#define RAYFILEINSTANCE_H


#include "common.h"
#include "geometry.h"
#include "intersection.h"
#include "shapegroup.h"
#include <map>

class RayFile;
class Group;


/*
 * instance of an included rayfile within another rayfile, signified
 * by the #rayfile_include directive.  contains a name of the rayfile
 * to reference, and rayfile and group pointers setup after parsing.
 * just passes draw and intersect calls on through to the rayfile.
 */
class RayFileInstance : public ShapeGroup
{
 public:

			RayFileInstance ();
virtual			~RayFileInstance ();

	double		intersect (Intersection& info);
	void		glDraw ();
	int		makeLinks (RayFile* rf);

			// read and write .ray directives
	istream&	read (istream& in);
	ostream&	write (ostream& out);

/* inherited from ShapeGroup
 *
 * 	const char* 	getName ();
 */

 protected:

	RayFile*	rayfile;
	Group*		root;	// base node of the shape heirarchy

/* inherited from ShapeGroup
 *
 *	char		name[MAX_NAME];
 */

};


ostream& operator<< (ostream& out, RayFileInstance& i);
istream& operator>> (istream& in,  RayFileInstance& i);


#endif // RAYFILEINSTANCE_H
