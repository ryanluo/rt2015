#include "cone.h"
#include "parse.h"
#include "material.h"
#include "roots.h"
#include "main.h"



// single glu quadric object to draw all the cones with
static GLUquadricObj* coneQuadric = gluNewQuadric();


Cone::Cone ()
: Shape ()
{
	closed = false;
}


Cone::~Cone ()
{
}


double Cone::intersect (Intersection& info)
{
	/*
	* z-axis aligned cone is of the form 
	* (x - x_0)^2 + (y - y_0)^2 = radius^2 / height^2 ((z - z_0) - height)^2 
	* with 0 <= z <= height
	*
	* ray is of the form ray.pos + alpha * ray.dir
	*
	* plug ray equation into that for the cone, solve for alpha, and find
	* the corresponding z-values
	*/

	/* set up quadratic: a * alpha^2 + b * alpha * c = 0 */

	

	return -1;
}

bool Cone::inside (const Point3d& pos, bool surfaceOpen)
{

	return true;
}

bool Cone::inInfCone (const Point3d& pos)
{


	return true;
}

TexCoord2d Cone::getTexCoordinates (Point3d i)
{
	Vector3d n(center, i);

	TexCoord2d tCoord(0.0,0.0);

	// This function calculates the texture coordinates for the cone
	// intersection.  It uses the normal at the point of intersection
	// to calculate the polar coordinates of the intersection on that
	// circle of the cone.  Then we linearly map these coordinates as:
	//     Theta: [0, 2*PI] -> [0, 1]
	// This remapped theta gives us tCoord[0].  For tCoord[1], we take
	// the distance along the height of the cone, using the top as the
	// zero point.


	
	return tCoord;
}




void Cone::glDraw ()
{
	material->glLoad();

	glPushMatrix();
	// move to the cone's origin
	glTranslatef(center[0], center[1], center[2]);

	// set glu quadric options - smooth filled polys with normals
	gluQuadricDrawStyle(coneQuadric, GLU_FILL);
	gluQuadricOrientation(coneQuadric, GLU_OUTSIDE);
	gluQuadricNormals(coneQuadric, GLU_SMOOTH);

	// only calc tex coords if textures are turned on
	if (options->textures && textured && material->textured())
	{
		gluQuadricTexture(coneQuadric, GLU_TRUE);
		glEnable(GL_TEXTURE_2D);
	}

	// draw the cone
	gluCylinder(coneQuadric, radius, 0.0, height, 20, 5);

	// if it's closed, cap it off with a disk
	if (closed)
	{
		gluQuadricOrientation(coneQuadric, GLU_INSIDE);
		gluDisk(coneQuadric, 0, radius, 20, 1);
	}
	glPopMatrix();

	material->glUnload();
	glDisable(GL_TEXTURE_2D);
}


inline istream& operator>> (istream& in, Cone& s)
{
	return s.read(in);
}


istream& Cone::read (istream& in)
{
	int numFlags = 5;
	Flag flags[5] = { { (char*)"-n", STRING, false, MAX_NAME,       name      },
	{ (char*)"-m", STRING, true,  MAX_NAME,       matname   },
	{ (char*)"-t", BOOL,   false, sizeof(bool),   &textured },
	{ (char*)"-c", BOOL,   false, sizeof(bool),   &closed   },
	{ (char*)"-u", DOUBLE, false, sizeof(double), &bumpScale }
	};

	if (parseFlags(in, flags, numFlags) == -1)
	{
		cerr << "ERROR: Cone: flag parsing failed!" << endl;
		return in;
	}

	if (bumpScale != 0)
		bumpMapped = true;

	in >> center;

	radius = nextDouble(in);
	height = nextDouble(in);

	if (in.fail())
	{
		cerr << "ERROR: Cone: unknown stream error" << endl;
		return in;
	}

	return in;
}


inline ostream& operator<< (ostream& out, Cone& s)
{
	return s.write(out);
}


ostream& Cone::write (ostream& out)
{
	out << "#cone -m " << matname << flush;
	if (name[0] != '\0')
		out << " -n " << name << flush;
	if (textured)
		out << " -t" << flush;
	if (bumpMapped)
		out << " -u" << flush;  
	if (closed)
		out << " -c" << flush;
	out << " --" << endl;

	out << "  " << center << endl;
	out << "  " << radius << endl;
	out << "  " << height << endl;

	return out;
}
