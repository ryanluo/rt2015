#include "group.h"
#include "parse.h"
#include "main.h"
#include "rayfile.h"

Group::Group (RayFile *file)
: ShapeGroup ()
{
	transform             = Matrixd::IdentityMatrix();
	invTransform          = Matrixd::IdentityMatrix();
	invTransposeTransform = Matrixd::IdentityMatrix();

	this->file = file;
}


Group::~Group ()
{
	for (VECTOR(ShapeGroup*)::iterator sg = list.begin(); 
		sg != list.end(); 
		++sg)
		delete *sg;
	list.clear();
}


double Group::intersect (Intersection& intersectionInfo)
{
	/*
	* Find the closest intersection point in the group to the start of info.theRay
	*/

    
    // make copy of intersection info for local use
    Intersection localInfo;
    localInfo.theRay = intersectionInfo.theRay;
    
    // RAY_CASTING TODO (Transformations)
    // transform localInfo.theRay into local coordinates
    // RAY_CASTING TODO (Transformations)
    
    
    
    
    // alpha is the distance to the closest intersection point we've found
    double alpha=-1;

    // Step through the objects in the group, testing each for an intersection.
    // Update

	for (VECTOR(ShapeGroup*)::iterator sg = list.begin(); sg != list.end(); ++sg)
	{
        // localInfo.theRay keeps the closest intersection info we have found
        // tempInfo.theRay will keep the intersection info of the current
        // intersection test
        Intersection tempInfo;
        tempInfo.theRay = localInfo.theRay;

		// Distance for the current object
		double currDist = (*sg)->intersect(tempInfo);

        // RAY_CASTING TODO (Intersection)
		// In case of a new closest intersection, update alpha a localInfo
        // RAY_CASTING TODO (Intersection)
        if (currDist < alpha || alpha < 0) {
            alpha = currDist;
            localInfo.material = tempInfo.material;
        }
	}

    // RAY_CASTING TODO (sphere/triangle intersection and transformation)
    // If intersection point was found
	// transform localInfo parent's coordinate and copy into intersectinInfo
    // (be sure to renormalize normal vector!)
    // recompute alpha in parent's coordinate system
    // RAY_CASTING TODO (sphere/triangle intersection and transformation)
    intersectionInfo.material = localInfo.material;
    return alpha;
}


void Group::add (ShapeGroup* sg)
{
	list.push_back(sg);
}


ShapeGroup* Group::get (int i)
{
	return list[i];
}


ShapeGroup* Group::operator[] (int i)
{
	return get(i);
}


int Group::size ()
{
	return (int) list.size();
}

VECTOR(ShapeGroup*)::iterator Group::begin()
{
	return list.begin();
}

/*
ShapeGroup** Group::begin ()
{
return list.begin();
}*/

VECTOR(ShapeGroup*)::iterator Group::end()
{
	return list.end();
}


void Group::glDraw ()
{

	glPushMatrix();

	// add in the group transformation to the modelview matrix stack
	if (options->transforms)
		glMultMatrixd(transform.ptr());

	// draw each element in the gorup
	for (VECTOR(ShapeGroup*)::iterator sg = list.begin(); 
		sg != list.end(); 
		++sg)
		(*sg)->glDraw();

	glPopMatrix();
}


int Group::makeLinks (RayFile* rf)
{
	for (VECTOR(ShapeGroup*)::iterator sg = list.begin(); 
		sg != list.end(); 
		++sg)
	{
		if ((*sg)->makeLinks(rf) == -1)
			return -1;
	}

	return 0;
}


inline istream& operator>> (istream& in, Group& g)
{
	return g.read(in);
}


istream& Group::read (istream& in)
{
	int numFlags = 1;
	Flag flags[1] = {{ (char*)"-n", STRING, false,  MAX_NAME, name }};

	if (parseFlags(in, flags, numFlags) == -1)
	{
		cerr << "ERROR: Group: flag parsing failed!" << endl;
		return in;
	}

	//for(int i = 0; i < 80; ++i)
	//	cerr << (int)CURR_FILE_VERSION[i] << " ";
	//cerr << "\n";
	//cerr << VersionTag05 << "\n";

	if(strcmp(file->version, VersionTag02) == 0)
		in >> transform;
	else if(strcmp(file->version, VersionTag05) == 0)
		transform = readMatrixTransforms(in);
	else
	{
		cerr << "ERROR: Group: Unknown file version... Freaking out!" << endl;
		return in;
	}

	char  next[MAX_LINE];
	bool  done = false;

	// parse while we have a good stream and we haven't reached #group_end
	while (in && !done)
	{
		nextToken(in, next, MAX_LINE);

		if (next[0] == '#')
		{
			if (strncmp(next, "#group_end", 10) == 0)
			{
				// end of group delimiter, stop parsing
				done = true;
			}
			else
			{
				// check if it's a shape of some sort
				ShapeGroup* sg = parseShapeGroup(in, next, MAX_LINE, file);

				// not a shape, so we don't recognize it
				if (!sg)
				{
					cerr << "ERROR: Group: unrecognized directive: " << next << endl;
					in.setstate(istream::failbit);
					return in;
				}

				// add the shape to our list
				add(sg);
			}
		}
		else
		{
			cerr << "ERROR: Group: unrecognized symbol: " << next << endl;
			in.setstate(istream::failbit);
			return in;
		}
	}

	// exited because stream failed...
	if (!done)
	{
		cerr << "ERROR: Group: missing #group_end" << endl;

		if (!in.eof())
		{
			cerr << "ERROR: Group: unknown stream failure" << endl;
		}

		return in;
	}

	// calculate our inverse and our inverse transpose
	invTransform          = transform.invert();
	invTransposeTransform = transform.transpose().invert();

	return in;
}

Matrixd Group::readMatrixTransforms(istream &in)
{
	char  next[MAX_LINE];

	Matrixd result = Matrixd::IdentityMatrix();

	while(in)
	{
		peekToken(in, next, MAX_LINE);

		if (next[0] == '#')
		{
			if(strncmp(next, "#translate", 10) == 0)
				result = result.rightMult(readTranslate(in));
			else if(strncmp(next, "#rotate", 7) == 0)
				result = result.rightMult(readRotate(in));
			else if(strncmp(next, "#scale", 6) == 0)
				result = result.rightMult(readScale(in));
			else // done with transforms, stop parsing
				break;
		}
		else
		{
			cerr << "ERROR: Group: unrecognized transform: " << next << endl;
			in.setstate(istream::failbit);
			return result;
		}
	}

	return result;
}

Matrixd Group::readTranslate(istream &in)
{	
	// chomp #translate token
	chompToken(in);

	double dx = nextDouble(in);
	double dy = nextDouble(in);
	double dz = nextDouble(in);

	//cerr << "trans: " << dx << " " << dy << " " << dz << "\n";

	Matrixd result = Matrixd::IdentityMatrix();

	result(0, 3) = dx;
	result(1, 3) = dy;
	result(2, 3) = dz;

	return result;
}

Matrixd Group::readRotate(istream &in)
{
	// chomp #rotate token
	chompToken(in);

	double theta = nextDouble(in);
	double rx = nextDouble(in);
	double ry = nextDouble(in);
	double rz = nextDouble(in);

	double sum = sqrt(rx*rx + ry*ry + rz*rz);

	assert(sum != 0);

	rx /= sum;
	ry /= sum;
	rz /= sum;

	double degreesToRadians = 3.14159265358979323846264338327950288419716939937510 / 180.0;

	double cosTheta = cos(theta * degreesToRadians);
	double sinTheta = sin(theta * degreesToRadians);

	Matrixd result = Matrixd::IdentityMatrix();

	result(0,0) = cosTheta + (1 - cosTheta)*rx*rx;
	result(0,1) = (1 - cosTheta)*rx*ry - rz * sinTheta;
	result(0,2) = (1 - cosTheta)*rx*rz + ry * cosTheta;

	result(1,0) = (1 - cosTheta)*rx*ry + rz * sinTheta;
	result(1,1) = cosTheta + (1 - cosTheta)*ry*ry;
	result(1,2) = (1 - cosTheta)*ry*rz - rx * sinTheta;

	result(2,0) = (1 - cosTheta)*rx*rz - ry * sinTheta;
	result(2,1) = (1 - cosTheta)*ry*rz + rx * sinTheta;
	result(2,2) = cosTheta + (1 - cosTheta)*rz*rz;

	return result;
}

Matrixd Group::readScale(istream &in)
{
	// chomp #scale token
	chompToken(in);

	double sx = nextDouble(in);
	double sy = nextDouble(in);
	double sz = nextDouble(in);

	//cerr << "Scale: " << sx << " " << sy << " " << sz << "\n";

	Matrixd result = Matrixd::IdentityMatrix();

	result(0,0) = sx;
	result(1,1) = sy;
	result(2,2) = sz;

	return result;
}

inline ostream& operator<< (ostream& out, Group& g)
{
	return g.write(out);
}


ostream& Group::write (ostream& out)
{
	out << "#group_begin ";
	if (name[0] != '\0')
		out << "-n " << name;
	out << "-m ";
	out << " --" << endl;

	out << transform << endl;

	for (VECTOR(ShapeGroup*)::iterator sg = list.begin(); sg != list.end(); ++sg)
		out << **sg << endl;

	out << "#group_end" << endl;

	return out;
}
