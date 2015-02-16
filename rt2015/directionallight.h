class DirectionalLight;


#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H


#include "common.h"
#include "light.h"

class ShapeGroup;


/*
* directional light class, just like OpenGL's.  supports diffuse,
* specular and shadow calculations for an intersection
*/
class DirectionalLight : public Light 
{
public:

	DirectionalLight ();
	virtual			~DirectionalLight ();

	// scene lighting functions
	Color3d		getDiffuse  (Intersection& iInfo);
	Color3d		getSpecular (Intersection& iInfo);
	bool		getShadow (Intersection& iInfo,
		ShapeGroup* root);

	// read and write .ray directives
	istream&	read  (istream& in);
	ostream&	write (ostream& out);

	// OpenGL calls
	void		glLoad (GLenum light);
	void		glDraw ();

	/* inherited from Light
	*
	*	const char*	getName ();
	*/

private:

	Vector3d	direction;	// global coordinates

	/* inherited from Light
	*
	*	Color3d		color;
	*	char		name[MAX_NAME];
	*/

};


istream& operator>> (istream& in,  DirectionalLight& l);
ostream& operator<< (ostream& out, DirectionalLight& l);


#endif // DIRECTIONALLIGHT_H
