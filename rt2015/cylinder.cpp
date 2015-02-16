#include "cylinder.h"
#include "parse.h"
#include "material.h"
#include "main.h"
#include "roots.h"


// single glu quadric object to draw cylinders with
static GLUquadricObj* cylinderQuadric = gluNewQuadric();


Cylinder::Cylinder ()
: Shape ()
{
	closed = false;
}


Cylinder::~Cylinder ()
{
}


double Cylinder::intersect (Intersection& info)
{
	/*
	* z-axis aligned cylinder is of the form x^2 + y^2 = radius^2 with 
	* 0 <= z <= length
	*
	* ray is of the form ray.pos + alpha * ray.dir
	*
	* plug ray equation into that for the cylinder, solve for alpha, and find
	* the corresponding z-values
	*/

	/* set up quadratic: a * alpha^2 + b * alpha * c = 0 */

	return -1;
}

TexCoord2d Cylinder::getTexCoordinates (Point3d i)
{
	Vector3d n(center, i);
	

	TexCoord2d tCoord(0.0,0.0);

	// This function calculates the texture coordinates for the cylinder
	// intersection.  It uses the normal at the point of intersection
	// to calculate the polar coordinates of the intersection on that
	// circle of the cylinder.  Then we linearly map these coordinates as:
	//     Theta: [0, 2*PI] -> [0, 1]
	// This remapped theta gives us tCoord[0].  For tCoord[1], we take
	// the distance along the height of the cylinder, using the top as
	// the zero point.


	return tCoord;
}




void Cylinder::glDraw ()
{
	material->glLoad();

	glPushMatrix();
	// translate to the cylinder's origin
	glTranslatef(center[0], center[1], center[2]);

	// set up the quadric - smooth filled polys, calculate normals
	gluQuadricDrawStyle(cylinderQuadric, GLU_FILL);
	gluQuadricOrientation(cylinderQuadric, GLU_OUTSIDE);
	gluQuadricNormals(cylinderQuadric, GLU_SMOOTH);

	// calculate tex coords if we need to
	if (options->textures && textured && material->textured())
	{
		gluQuadricTexture(cylinderQuadric, GLU_TRUE);
		glEnable(GL_TEXTURE_2D);
	}

	// draw the cylinder
	gluCylinder(cylinderQuadric, radius, radius, length, 20, 1);

	// if closed, cap off the ends with disks
	if (closed)
	{
		glPushMatrix();
		glTranslatef(0.0, 0.0, length);
		gluDisk(cylinderQuadric, 0, radius, 20, 1);
		glPopMatrix();

		gluQuadricOrientation(cylinderQuadric, GLU_INSIDE);
		gluDisk(cylinderQuadric, 0, radius, 20, 1);
	}
	glPopMatrix();

	material->glUnload();
	glDisable(GL_TEXTURE_2D);
}


inline istream& operator>> (istream& in, Cylinder& s)
{
	return s.read(in);
}


istream& Cylinder::read (istream& in)
{
	int numFlags = 5;
	Flag flags[5] = { { (char*)"-n", STRING, false, MAX_NAME,       name      },
	{ (char*)"-m", STRING, true,  MAX_NAME,       matname   },
	{ (char*)"-t", BOOL,   false, sizeof(bool),   &textured },
	{ (char*)"-u", DOUBLE, false, sizeof(double), &bumpScale },
	{ (char*)"-c", BOOL,   false, sizeof(bool),   &closed   }
	};

	if (parseFlags(in, flags, numFlags) == -1)
	{
		cerr << "ERROR: Cylinder: flag parsing failed!" << endl;
		return in;
	}

	if (bumpScale != 0)
		bumpMapped = true;

	in >> center;

	radius = nextDouble(in);
	length = nextDouble(in);

	if (in.fail())
	{
		cerr << "ERROR: Cylinder: unknown stream error" << endl;
		return in;
	}

	return in;
}


inline ostream& operator<< (ostream& out, Cylinder& s)
{
	return s.write(out);
}


ostream& Cylinder::write (ostream& out)
{
	out << "#cylinder -m " << matname << flush;
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
	out << "  " << length << endl;

	return out;
}
