class PointLight;


#ifndef POINTLIGHT_H
#define POINTLIGHT_H


#include "common.h"
#include "light.h"

class ShapeGroup;


/*
 * class to represent a point light, along the lines of OpenGL's point
 * light object.  has a location, color and attenuation.  supports
 * diffuse, specular and shadow calculations for raytracing
 */
class PointLight : public Light 
{
 public:
			PointLight ();
virtual			~PointLight ();

			// scene lighting functions
	Color3d		getDiffuse  (Intersection& iInfo);
	Color3d		getSpecular (Intersection& iInfo);
	bool		getShadow   (Intersection& iInfo,
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
  
	Point3d		location;	// global coordinates

	double		constAtten;
	double		linearAtten;
	double		quadAtten;

/* inherited from Light
 *
 * 	Color3d		color;
 *	char		name[MAX_NAME];
 */
  
};


istream& operator>> (istream& in,  PointLight& l);
ostream& operator<< (ostream& out, PointLight& l);


#endif // POINTLIGHT_H
