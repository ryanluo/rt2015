typedef struct Options_ Options;


#ifndef MAIN_H
#define MAIN_H


#include "common.h"

class RayFile;
class Image;

using namespace std;
/*
* User-controllable options to determine various aspects of the ray
* tracing
*/
struct Options_
{
	int 		width;
	int 		height;
	int 		jitter;
	int 		shadow;
	int 		recursiveDepth;
	double		contribution;
	char		source[MAX_PATH];
	char		destination[MAX_PATH];

	bool		progressive;
	bool		lighting;
	bool		transforms;
	bool		textures;
	bool 		bumpMaps;
	bool		transparency;
	bool		backface;
	bool		quiet;
};


// functions relating to the Options object
void     initOptions ();
ostream& operator<<  (ostream& out, Options& o);


extern Options* options;	// runtime rendering options
extern RayFile* scene;		// description of the scene
extern Image*   image;		// image to raytrace to

extern bool     drawPolys;	// draw interactively or not?


// command line interface methods
int  main (int argc, char** argv);
void init (int argc, char** argv);
void parseArgs (int argc, char** argv);
void help ();
void usage ();
int nonInteractiveMode (int argc, char** argv);


// OpenGL callbacks
void display ();
void unreshape (int width, int height);
void reshape (int width, int height);


#endif // MAIN_H
