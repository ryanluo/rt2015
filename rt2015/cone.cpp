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
    Point3d p = info.theRay.getPos();
    Vector3d v = info.theRay.getDir();
    
    double a = sqr(v[0]) + sqr(v[1]) - sqr(radius / height) * sqr(v[2]);
    double b = 2 * (p[0]*v[0] + p[1]*v[1] - v[0]*center[0] - v[1]*center[1]) - sqr(radius/height) * (2 * (p[2]*v[2] - center[2] * v[2] - height * v[2]));
    double c = sqr(p[0]) + sqr(p[1]) - sqr(radius / height) * (sqr(p[2]) - 2 * p[2] * height - 2 * p[2] * center[2] + sqr(center[2]) + 2 * center[2] * height + sqr(height)) + sqr(center[0]) + sqr(center[1]) - 2*p[0]*center[0] - 2*p[1]*center[1];
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
    if (z1 - center[2] < 0 || z1 - center[2] > height) {
        alpha1 = alpha2;
    }
    if (z2 - center[2] < 0 || z2 - center[2] > height) {
        alpha2 = alpha1;
    }
    alpha = min(alpha1, alpha2);
    
    info.iCoordinate = p + alpha * v;
    
    if (alpha < 0 || info.iCoordinate[2] - center[2] > height || info.iCoordinate[2] - center[2] < 0) return alpha = -1;
    
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
            Point3d otherFace(0,0,height
                              );
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
    double r = sqrt(sqr(n[0]) + sqr(n[1]));
    double theta = asin(n[1]/r);
    theta = (theta + M_PI/2)/M_PI;
    double h = n[2]/ height;
    
    TexCoord2d tCoord(theta, h);

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
