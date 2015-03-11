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
    Point3d p = info.theRay.getPos();
    Vector3d v = info.theRay.getDir();
    
    double a = sqr(v[0]) + sqr(v[1]);
    double b = 2 * (p[0]*v[0] + p[1]*v[1] - v[0]*center[0] - v[1]*center[1]);
    double c = sqr(p[0]) + sqr(p[1]) - sqr(radius) + sqr(center[0]) + sqr(center[1]) - 2*p[0]*center[0] - 2*p[1]*center[1];
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
    if (z1 - center[2] < 0 || z1 - center[2] > length) {
        alpha1 = alpha2;
    }
    if (z2 - center[2] < 0 || z2 - center[2] > length) {
        alpha2 = alpha1;
    }
    alpha = min(alpha1, alpha2);
    
    info.iCoordinate = p + alpha * v;

    if (alpha < 0 || info.iCoordinate[2] - center[2]> length || info.iCoordinate[2] - center[2]< 0) return alpha = -1;
    
    info.material = this->material;
    Point3d circleCenter(center[0],center[1],info.iCoordinate[2]);
    Vector3d normal = info.iCoordinate - circleCenter;
    info.entering = true;
    if (normal.dot(v) > 0) {
        normal *= -1;
        info.entering = false;
    }
    normal.normalize();
    
    if (closed) {
        Point3d faceCenter = center;
        Vector3d faceNormal(0,0,-1);
        if (faceNormal.dot(v) != 0) {
            Vector3d u = center - p;
            double alpha3 = faceNormal.dot(u) / faceNormal.dot(v);
            Point3d faceInter = p + alpha3 * v - center;
            if(sqr(faceInter[0]) + sqr(faceInter[1]) <= sqr(radius) && alpha3 > 0) {
                if (alpha3 < alpha) {
                    info.iCoordinate = p + alpha3 * v;
                    normal = faceNormal;
                    info.entering = true;
                    if (normal.dot(v) > 0) {
                        normal *= -1;
                        info.entering = false;
                    }
                }
            }
            faceNormal *= -1;
            Point3d otherFace(0,0,length);
            u = center - p + otherFace;
            alpha3 = faceNormal.dot(u) / faceNormal.dot(v);
            faceInter = p + alpha3 * v;
            if(sqr(faceInter[0]) + sqr(faceInter[1]) <= sqr(radius) && alpha3 > 0) {
                if (alpha3 < alpha) {
                    info.iCoordinate = p + alpha3 * v;
                    normal = faceNormal;
                    info.entering = true;
                    if (normal.dot(v) > 0) {
                        normal *= -1;
                        info.entering = false;
                    }
                }

            }
        }
    }
    info.textured = this->textured;
    info.texCoordinate = getTexCoordinates(info.iCoordinate);
    
    info.normal = normal;

    //bump mapping
    Vector3d yDir(0,1,0);
    Vector3d vectorUp = yDir-normal.dot(yDir)*normal;
    
    if(vectorUp.length() != 0) vectorUp.normalize();
    Vector3d right = -normal.cross(vectorUp);
    material->bumpNormal(normal, vectorUp, right, info, bumpScale);
    return alpha;
    
}

TexCoord2d Cylinder::getTexCoordinates (Point3d i)
{
	Vector3d n(center, i);
    double r = sqrt(sqr(n[0]) + sqr(n[1]));
    double theta = asin(n[1]/r);
    theta = (theta + M_PI/2)/M_PI;
    double h = n[2] / length;

	TexCoord2d tCoord(theta, h);

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
