#include "torus.h"
#include "parse.h"
#include "material.h"
#include "main.h"
#include "roots.h"


Torus::Torus ()
: Shape ()
{
}


Torus::~Torus ()
{
}


double Torus::intersect (Intersection& info)
{
	/*
	* z-axis aligned torus centered at (x0, y0, z0) is of the form 
	* ((x-x0)^2 + (y-y0)^2 + (z-z0)^2 - (majorRad^2 + minorRad^2))^2 = 
	*					 majorRad^2 (minorRad^2 - (z-z0)^2)
	*
	* ray is of the form ray.pos + alpha * ray.dir
	*
	* plug ray equation into that for the torus, solve for alpha, and find
	* the corresponding z-values by solving the resulting quartic.
	*/


		return -1;

}


bool Torus::inside (const Point3d& pos, bool surfaceOpen)
{
	return true;
}


TexCoord2d Torus::getTexCoordinates (Point3d i)
{
	
	TexCoord2d tCoord;
	tCoord[0]=0;
	tCoord[1]=0;
	return tCoord;
}





// from GLUT source code, called doughnut
void Torus::glutTexturedTorus (GLfloat r, GLfloat R, 
							   GLint nsides, GLint rings)
{
	int i, j;
	GLfloat theta, phi, theta1;
	GLfloat cosTheta, sinTheta;
	GLfloat cosTheta1, sinTheta1;
	GLfloat ringDelta, sideDelta;
	GLfloat sCoord, sCoord1, tCoord;
	GLfloat sDelta, tDelta;

	ringDelta = 2.0 * M_PI / rings;
	sideDelta = 2.0 * M_PI / nsides;
	sDelta = 1.0 / rings;
	tDelta = 1.0 / nsides;

	theta = 0.0;
	cosTheta = 1.0;
	sinTheta = 0.0;
	sCoord = 1.0;
	for (i = rings - 1; i >= 0; i--) {
		theta1 = theta + ringDelta;
		cosTheta1 = cos(theta1);
		sinTheta1 = sin(theta1);
		sCoord1 = sCoord - sDelta;
		glBegin(GL_QUAD_STRIP);
		phi = M_PI;
		tCoord = 0.0;
		for (j = nsides; j >= 0; j--) {
			GLfloat cosPhi, sinPhi, dist;

			phi += sideDelta;
			cosPhi = cos(phi);
			sinPhi = sin(phi);
			tCoord += tDelta;
			dist = R + r * cosPhi;

			glTexCoord2f(sCoord1, tCoord);
			glNormal3f(cosTheta1 * cosPhi, -sinTheta1 * cosPhi, sinPhi);
			glVertex3f(cosTheta1 * dist, -sinTheta1 * dist, r * sinPhi);
			glTexCoord2f(sCoord, tCoord);
			glNormal3f(cosTheta * cosPhi, -sinTheta * cosPhi, sinPhi);
			glVertex3f(cosTheta * dist, -sinTheta * dist,  r * sinPhi);
		}
		glEnd();
		theta = theta1;
		cosTheta = cosTheta1;
		sinTheta = sinTheta1;
		sCoord = sCoord1;
	}
}


void Torus::glDraw ()
{
	material->glLoad();

	glPushMatrix();
	// move to the origin of the torus
	glTranslatef(center[0], center[1], center[2]);

	if (options->textures && textured && material->textured())
	{ 
		glEnable(GL_TEXTURE_2D);
	}

	// draw the torus
	glutTexturedTorus(minorRad, majorRad, 20, 20);
	glPopMatrix();

	material->glUnload();
	glDisable(GL_TEXTURE_2D);
}


inline istream& operator>> (istream& in, Torus& s)
{
	return s.read(in);
}


istream& Torus::read (istream& in)
{
	int numFlags = 4;
	Flag flags[4] = { { (char*)"-n", STRING, false, MAX_NAME,       name      },
	{ (char*)"-m", STRING, true,  MAX_NAME,       matname   },
	{ (char*)(char*)"-t", BOOL,   false, sizeof(bool),   &textured },
	{ (char*)"-u", DOUBLE, false, sizeof(double), &bumpScale }
	};

	if (parseFlags(in, flags, numFlags) == -1)
	{
		cerr << "ERROR: Torus: flag parsing failed!" << endl;
		return in;
	}

	if (bumpScale != 0)
		bumpMapped = true;

	in >> center;

	majorRad = nextDouble(in);
	minorRad = nextDouble(in);

	if (in.fail())
	{
		cerr << "ERROR: Torus: unknown stream error" << endl;
		return in;
	}
	return in;
}


inline ostream& operator<< (ostream& out, Torus& s)
{
	return s.write(out);
}


ostream& Torus::write (ostream& out)
{
	out << "#torus -m " << matname << flush;
	if (name[0] != '\0')
		out << " -n " << name << flush;
	if (textured)
		out << " -t" << flush;

	if (bumpMapped)
		out << " -u" << flush;

	out << " --" << endl;

	out << "  " << center << endl;
	out << "  " << majorRad << endl;
	out << "  " << minorRad << endl;

	return out;
}
