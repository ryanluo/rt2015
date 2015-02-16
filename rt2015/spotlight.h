#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H


#include "common.h"
#include "light.h"

class ShapeGroup;


/*
 * spot light class that has all the same features as an opengl spot
 * light, along with calls for determining diffuse, specular and shadow
 * values for intersections
 */
class SpotLight : public Light 
{
 public:
			SpotLight ();
virtual			~SpotLight ();

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
	Vector3d	beamCenter;

	double		constAtten;
	double		linearAtten;
	double		quadAtten;

	double		cutOffAngle;	// read in degrees, store in radians
	double		dropOffRate;

/* inherited from Light
 *
 *	Color3d		color;
 *	char		name[MAX_NAME];
 */

};


istream& operator>> (istream& in,  SpotLight& l);
ostream& operator<< (ostream& out, SpotLight& l);  


#endif // SPOTLIGHT_H
