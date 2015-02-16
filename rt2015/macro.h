class Macro;


#ifndef MACRO_H
#define MACRO_H


#include "common.h"

class Group;
class MacroInstance;
class RayFile;

using namespace std;

/*
* class that represents a small shape heirarchy that can be pasted
* into the rayfile's shape heirarchy multiple times to simplify
* complex scenes.  does not store it's own materials, uses global
* materials.  exists only for parsing - after linking, macro is placed
* in the scene's shape heirarchy
*/
class Macro
{
public:
	Macro (RayFile *file);
	virtual			~Macro ();

	// accessor
	const char*	getName () { return name; }

	// read and write .ray directives
	virtual	istream&	read  (istream& in);
	virtual	ostream&	write (ostream& out);

protected:

	char		name[MAX_NAME];
	Group*		root;	// base node of the shape heirarchy

	RayFile *file;

	friend	MacroInstance;

};


istream& operator>> (istream& in,  Macro& m);
ostream& operator<< (ostream& out, Macro& m);


#endif // MACRO_H
