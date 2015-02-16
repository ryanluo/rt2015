class MacroInstance;


#ifndef MACROINSTANCE_H
#define MACROINSTANCE_H


#include "common.h"
#include "geometry.h"
#include "intersection.h"
#include "shapegroup.h"
#include <map>

class RayFile;
class Macro;
class Group;


/*
* instance of a macro in a shape heirarchy, placed with the 
* #macro_instance directive.  just a place holder for parsing - after
* linking, stores the macro's shape heirarchy and just passes
* gldraw and intersect calls on to it.
*/
class MacroInstance : public ShapeGroup
{
public:

	MacroInstance ();
	virtual			~MacroInstance ();

	double		intersect ( Intersection& info);
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

	Macro*		macro;
	Group*		root;	// base node of the shape heirarchy

	/* inherited from ShapeGroup
	*
	*	char		name[MAX_NAME];
	*/

};


ostream& operator<< (ostream& out, MacroInstance& i);
istream& operator>> (istream& in,  MacroInstance& i);


#endif // MACROINSTANCE_H
