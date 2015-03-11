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
    
    Point3d p = info.theRay.getPos();
    Vector3d v = info.theRay.getDir();
    
    double a = sqr(v[0]) + sqr(v[1]) + sqr(v[2])
    
     + sqr(majorRad) * sqr(v[2]);
    double b = 2 * (p[0]*v[0] + p[1]*v[1] + p[2]*v[2] - v[0]*center[0] - v[1]*center[1] - v[2]*center[2])
    
        + sqr(majorRad) * 2 * (p[2]*v[2] - v[2] * center[2]);
    double c = sqr(p[0]) + sqr(p[1]) + sqr(p[2])
     + sqr(center[0]) + sqr(center[1]) + sqr(center[2])
     - 2 * (p[0]*center[0] + p[1]*center[1] + p[2] * center[2])
     - sqr(sqr(majorRad) + sqr(minorRad))
    
     - sqr(majorRad) * sqr(minorRad)
     + sqr(majorRad) * (sqr(p[2]) + sqr(center[2]) - 2 * p[2] * center[2]);
    double determ = sqr(b) - 4 * a * c;
    double alpha1 = -1;
    double alpha2 = -1;
    double alpha = -1;
    double z1 = 0;
    double z2 = 0;
    if (determ >= 0) {
        alpha1 = (-b + sqrt(determ)) / (2*a);
        alpha2 = (-b - sqrt(determ)) / (2*a);
        z1 = (p + v * alpha1)[2];
        z2 = (p + v * alpha2)[2];
    }
    if (alpha1 < 0) {
        if (alpha2 < 0) return -1;
        alpha1 = alpha2;
    }
    if (alpha2 < 0) {
        alpha2 = alpha1;
    }

    alpha = min(alpha1, alpha2);
    
    info.iCoordinate = p + alpha * v;
    info.material = this->material;
    Point3d torusCenter(info.iCoordinate[0],info.iCoordinate[1],center[2]);
    Vector3d normal(center,info.iCoordinate);
    info.entering = true;
    if (normal.dot(v) > 0) {
        normal *= -1;
        info.entering = false;
    }
    normal.normalize();
    Vector3d n(0,0,-1);
    info.normal = n;
    return alpha;

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
