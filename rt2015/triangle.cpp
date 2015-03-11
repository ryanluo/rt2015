#include "triangle.h"
#include "parse.h"
#include "material.h"
#include "main.h"


Triangle::Triangle ()
: Shape ()
{
}


Triangle::~Triangle ()
{
}


double Triangle::intersect (Intersection& intersectionInfo)
{
	/*
	* This method solves for inersection between intersectionInfo.theRay and
	* the triangle. 
	*   If there is no intersection OR if the ray lies in the
	*	plane containing the triangle we return -1;
	*	Otherwise we return alpha the distance from the
	*	starting point of intersectionInfo.theRay to the intersection point
	
	*
	* Then, decompose the vector p - v0 into a linear combination of
	*/
    
    // RAY_CASTING TODO (intersection)
    // (1) determine intersection with plane (if any)
    // (2) test if intersection point is in triangle
    // (3) compute distance from start of ray to intersection point & normal in direction of incoming ray

    Vector3d v = intersectionInfo.theRay.getDir();
    Vector3d u = this->v[0] - intersectionInfo.theRay.getPos();
    

    if (n.dot(v) == 0) return -1;
    
    double alpha = n.dot(u) / n.dot(v);
    
    Vector3d w1 = this->v[1] - this->v[0];
    Vector3d w2 = this->v[2] - this->v[0];
    if (n.dot(v) > 0) {
        n = -1. * n;
    }
    double c = (alpha * v - u).dot(w1);
    double e = (alpha * v - u).dot(w2);
    double a = w1.dot(w1);
    double b = w1.dot(w2);
    double d = w2.dot(w2);
    
    double gamma = (c*b-a*e)/(-a*d+sqr(b));
    double beta = (c - gamma * b) / a;
    
    if (alpha>=0 && beta >= 0 && gamma >= 0 && beta + gamma <= 1) {
        intersectionInfo.iCoordinate = intersectionInfo.theRay.getPos() + alpha * v;
        intersectionInfo.material = this->material;
        intersectionInfo.normal = n.normalize();
        intersectionInfo.textured = this->textured;
        intersectionInfo.texCoordinate[0] = beta;
        intersectionInfo.texCoordinate[1] = gamma;
            Vector3d yDir(0,1,0);
            Vector3d vectorUp = yDir-n.dot(yDir)*n;
            if (vectorUp.length() != 0) vectorUp.normalize();
            Vector3d right = -n.cross(vectorUp);
            
            material->bumpNormal(n, vectorUp, right, intersectionInfo, bumpScale);
        
        return alpha;
    }
	return -1;
}




void Triangle::glDraw ()
{
	/*
	* draw the sphere with the appropriate material and textured status
	* at the desired position and radius
	*/

	material->glLoad();

	if (options->textures && textured && material->textured())
	{
		glEnable(GL_TEXTURE_2D);
		// draw the triangle with normal and tex coords
		glBegin(GL_TRIANGLES);
		n.glLoad();
		t[0].glLoad();
		v[0].glLoad();
		t[1].glLoad();
		v[1].glLoad();
		t[2].glLoad();
		v[2].glLoad();
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}
	else
	{
		// draw the triangle with normal but no tex coords
		glBegin(GL_TRIANGLES);
		n.glLoad();
		v[0].glLoad();
		v[1].glLoad();
		v[2].glLoad();
		glEnd();
	}

	material->glUnload();
}


inline istream& operator>> (istream& in, Triangle& t)
{
	return t.read(in);
}


istream& Triangle::read (istream& in)
{
	int numFlags = 4;
	Flag flags[4] = { { (char*)"-n", STRING, false, MAX_NAME,     name      },
	{ (char*)"-m", STRING, true,  MAX_NAME,     matname   },
	{ (char*)"-t", BOOL,   false, sizeof(bool), &textured },
	{ (char*)"-u", DOUBLE, false, sizeof(double), &bumpScale }

	};

	if (parseFlags(in, flags, numFlags) == -1)
	{
		cerr << "ERROR: Triangle: flag parsing failed!" << endl;
		return in;
	}
	if (bumpScale != 0)
		bumpMapped = true;

	if (textured || bumpMapped)
		in >> v[0] >> t[0] >> v[1] >> t[1] >> v[2] >> t[2];
	else
		in >> v[0] >> v[1] >> v[2];

	if (in.fail())
	{
		cerr << "ERROR: Triangle: unknown stream error" << endl;
		return in;
	}

	// Vectors from vertex 0 to the other vertices
	Vector3d v1 = v[1]-v[0];
	Vector3d v2 = v[2]-v[0];  

	// Calculate this triangle's normal
	n = (v1).cross(v2);
	n.normalize();

	return in;
}


inline ostream& operator<< (ostream& out, Triangle& t)
{
	return t.write(out);
}


ostream& Triangle::write (ostream& out)
{
	out << "#triangle -m " << matname << flush;
	if (name[0] != '\0')
		out << " -n " << name << flush;
	if (textured)
		out << " -t" << flush;
	out << " --" << endl;

	if (textured)
	{
		out << "  " << v[0] << "   " << t[0] << endl
			<< "  " << v[1] << "   " << t[1] << endl
			<< "  " << v[2] << "   " << t[2] << endl;
	}
	else
	{
		out << "  " << v[0] << endl
			<< "  " << v[1] << endl
			<< "  " << v[2] << endl;
	}

	return out;
}
