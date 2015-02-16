class RayFile;


#ifndef RAYFILE_H
#define RAYFILE_H


#include "common.h"
#include "geometry.h"
#include "intersection.h"
#include <vector>
#include <map>
#include <string>

class Image;
class Camera;
class Material;
class Light;
class Group;
class RayFileInstance;
class Macro;
class RayFile;


/*
 * class to represent a .ray file - stores all the objects represented
 * by the ray file's directives, in the same semantic structure.
 * handles highest level parsing of the rayfile and all memory associated
 * with parsed objects.
 */
class RayFile
{
 public:
			RayFile ();
			~RayFile ();

			// raytrace the scene and put it in image
			// ..in rayfile_render.cpp
	void		raytrace (Image* image);

			// polygon render the scene to the framebuffer
	void		glDraw ();

			// accessors
	Camera*		getCamera     () { return camera;     }
	const Color3d&	getAmbient    () { return ambient;    }
	const Color3d&	getBackground () { return background; }
	const char*	getName       () { return name;       }
	const char*	getFile       () { return file;       }

			// more complicated accessors
	Material*	getMaterial (const char* label);
	Macro*		getMacro    (const char* label);
	RayFile*	getRayFile  (const char* rayfile);

			// read and write .ray directives
	istream&	read  (istream& in);
	ostream&	write (ostream& out);

			// reinitialize everything
	void		clear ();

			// set the rayfile's filename
	void		setFile (const char* file_);
	
	// public version
	char version[MAX_VERSION_LENGTH];

 private:

			// in rayfile_render.cpp
    Color3d		getColor(Rayd theTrace, int rDepth);
	/*Color3d		getSurfaceColor (Intersection& info,
				  int rDepth, float contrib);*/

	void		setDir (const char* relativeTo);
	istream&	includeRayFile (istream& in);

	char		name[MAX_NAME];	// object name
	char		file[MAX_PATH];	// base filename
	char		dir[MAX_PATH];	// relative dir to the rayfile

	Color3d		background;
	Color3d		ambient;

	Camera*		camera;
	Group*		root;	// base node of the shape heirarchy

	VECTOR(Light*)		lights;
	MAP(string, Material*)	materials;
	MAP(string, Macro*)	macros;
	MAP(string, RayFile*)	rayfiles;

friend	RayFileInstance;

};


istream& operator>> (istream& in,  RayFile& rf);
ostream& operator<< (ostream& out, RayFile& rf);


#endif // RAYFILE_H
