#include "sphere.h"
#include "parse.h"
#include "material.h"
#include "main.h"
#include "common.h"

#define PI 3.14159
// single glu quadric object to do opengl sphere drawing
static GLUquadricObj* sphereQuadric = gluNewQuadric();


Sphere::Sphere ()
: Shape ()
{
}


Sphere::~Sphere ()
{
}


double Sphere::intersect (Intersection& intersectionInfo)
{
	/*
	* This method determines if intersectionInfo.theRay visibly intersects
	* the sphere from the position of the start of intersectionInfo.theRay
    *
    * The first visible intersection point is at alpha along
    * the ray where
	*	alpha is the smallest non-negative real root of
	*	||v||^2 alpha^2 - 2 (u dot v)alpha + ||u||^2 - r^2
	*	where 
	*	v is the unit direction vector of intersectionInfo.theRay
	*   u is the vector for the start of intersectionInfo.theRay to the
	*		center of the sphere
	*	r is the radius of the sphere
	*/

    double alpha=-1;
    
    // RAY_CASTING TODO (sphere intersection)
    // Determine if intersectionInfo.theRay intersects the sphere in front of the camera
    // if so, store intersectionInfo on intersection point in intersectionInfo
    // RAY_CASTING TODO (sphere intersection)
    
    Vector3d v = intersectionInfo.theRay.getDir();
    
    Vector3d u = this->center - intersectionInfo.theRay.getPos();
    
    double determ = 4*sqr(u.dot(v)) - 4*v.dot(v)*(u.dot(u) - sqr(this->radius));
    double alpha1 = -1;
    double alpha2 = -1;
    if (determ >= 0) {
        alpha1 = (2 * (u.dot(v)) + sqrt(determ))/(2*v.dot(v));
        alpha2 = (2 * (u.dot(v)) - sqrt(determ))/(2*v.dot(v));
    }
    if (alpha1 < 0) {
        if (alpha2 < 0) return -1;
        alpha1 = alpha2;
    }
    if (alpha2 < 0) {
        alpha2 = alpha1;
    }
    alpha = min(alpha1, alpha2);
    if (alpha < 0) return alpha = -1;
    intersectionInfo.material = this->material;
    intersectionInfo.iCoordinate = intersectionInfo.theRay.getPos() + alpha * v;
    Vector3d normal = ((Vector3d) intersectionInfo.iCoordinate - this->center);
    normal.normalize();
    intersectionInfo.entering = true;
    if (normal.dot(v)>0) {
        normal *= -1;
        intersectionInfo.entering = false;
    }
    intersectionInfo.normal = normal;
    intersectionInfo.textured = this->textured;
    
    
    //begin textured stuff
    if (alpha > 0){
        Point3d localPoint = intersectionInfo.iCoordinate-this->center;
        double r = ((Vector3d)localPoint).length();
        double theta = asin(localPoint[2]/r);
        double phi = atan2(localPoint[1],localPoint[0]);
        
        theta = (theta+(M_PI/2))/(M_PI);
        phi = (phi+M_PI)/(2* M_PI);
        intersectionInfo.texCoordinate[0] = phi;
        intersectionInfo.texCoordinate[1] = theta;
        //bump mapping
        
        Vector3d yDir(0,1,0);
        Vector3d vectorUp = yDir-normal.dot(yDir)*normal;
        if (vectorUp.length() != 0) vectorUp.normalize();
        Vector3d right = -normal.cross(vectorUp);
        material->bumpNormal(normal, vectorUp, right, intersectionInfo, bumpScale);
    }
    return alpha;
}


void Sphere::glDraw ()
{
	/*
	* draw the sphere with the appropriate material and textured status
	* at the desired position and radius, using a glu quadric (for
	* easy texture coordinate generation)
	*/

	material->glLoad();

	glPushMatrix();
	// move to the origin of the sphere
	glTranslatef(center[0], center[1], center[2]);

	// set up the glu object's parameters - smooth filled polys with normals
	gluQuadricDrawStyle(sphereQuadric, GLU_FILL);
	gluQuadricOrientation(sphereQuadric, GLU_OUTSIDE);
	gluQuadricNormals(sphereQuadric, GLU_SMOOTH);

	// only calculate tex coords if we need to
	if (options->textures && textured && material->textured())
	{
		gluQuadricTexture(sphereQuadric, GLU_TRUE);
		glEnable(GL_TEXTURE_2D);
	}

	// draw the sphere
	gluSphere(sphereQuadric, radius, 50, 50);
	glPopMatrix();

	material->glUnload();

	glDisable(GL_TEXTURE_2D);
}


inline istream& operator>> (istream& in, Sphere& s)
{
	return s.read(in);
}


istream& Sphere::read (istream& in)
{
	int numFlags = 4;
	Flag flags[4] = { { (char*)"-n", STRING, false, MAX_NAME,     name      },
	{ (char*)"-m", STRING, true,  MAX_NAME,     matname   },
	{ (char*)"-t", BOOL,   false, sizeof(bool), &textured },
	{ (char*)"-u", DOUBLE, false, sizeof(double), &bumpScale }
	};

	if (parseFlags(in, flags, numFlags) == -1)
	{
		cerr << "ERROR: Sphere: flag parsing failed!" << endl;
		return in;
	}

	if (bumpScale != 0)
		bumpMapped = true;


	in >> center;

	radius = nextDouble(in);

	if (in.fail())
	{
		cerr << "ERROR: Sphere: unknown stream error" << endl;
		return in;
	}

	return in;
}


inline ostream& operator<< (ostream& out, Sphere& s)
{
	return s.write(out);
}


ostream& Sphere::write (ostream& out)
{
	out << "#sphere -m " << matname << flush;
	if (name[0] != '\0')
		out << " -n " << name << flush;
	if (textured)
		out << " -t" << flush;
	out << " --" << endl;

	out << "  " << center << endl;
	out << "  " << radius << endl;

	return out;
}
