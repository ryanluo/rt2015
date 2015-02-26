#include "rayfile.h"
#include "image.h"
#include "camera.h"
#include "group.h"
#include "main.h"
#include "material.h"
#include "light.h"
#include "shape.h"
#include <vector>

double distance(Point3d point) {
    return sqrt(point[0]*point[0] + point[1]*point[1] + point[2]*point[2]);
}

/*
* method to cast rays through each pixel of image into the scene.
* uses getColor to find the color of an intersection point for each 
* pixel.  makes pretty pictures.
*/
void RayFile::raytrace (Image* image)
{

	// these will be useful
	int imageWidth = image->getWidth();
	int imageHeight = image->getHeight();
    
    // RAY_CASTING TODO
    // Read camera info and compute distance D to view window and the coordinates of its center and top left
    // Hint: check out camera object, which is part of RayFile
    // Hint: the half height angle is input in degrees but trig function require angles in radians
    // RAY_CASTING TODO
    
    Camera* camera = this->camera;
    float D = 0.5 * imageHeight / tan(camera->getHalfHeightAngle()/180*M_PI);
    Point3d origin = camera->getPos();
    Point3d center = origin + D * camera->getDir();
    Point3d leftCorner = center + (imageHeight/2) * camera->getUp() - (imageWidth/2) * camera->getRight();
    
	// for printing progress to stderr...
	double nextMilestone = 1.0;

	// 
	// ray trace the scene
	//

	for (int j = 0; j < imageHeight; ++j)
	{
		for (int i = 0; i<imageWidth; ++i)
		{

            // Compute the ray to trace
    
            // RAY_CASTING TODO
            // Compute and set the starting poisition and direction of the ray through pixel i,j
            // HINT: be sure to normalize the direction vector
            // RAY_CASTING TODO
            Point3d pixel = leftCorner + ((double) i + .5) * camera->getRight() - ((double) j + 0.5) * camera->getUp();
            Point3d difference = pixel - origin;
            Vector3d pixelV = ((Vector3d)(pixel-origin)).normalize();

            Rayd theRay(origin, pixelV);
			// get the color at the closest intersection point

			Color3d theColor = getColor(theRay, options->recursiveDepth);

			// the image class doesn't know about color3d so we have to convert to pixel
			// update pixel
			Pixel p;

			p.r = theColor[0];
			p.g = theColor[1];
			p.b = theColor[2];

			image->setPixel_(i, j, p);

		} // end for-i

		// update display 
		// you don't need to touch this part!

		if (options->progressive)
		{

				display();
		}
		else if (!options->quiet)
		{
			if (((double) j / (double) imageHeight) <= nextMilestone)
			{
				cerr << "." << flush;
				nextMilestone -= (1.0 / 79.0);
			}
		}
	} // end for-j
}


/* 
* get the color of the scene with respect to theRay
*/


Color3d RayFile::getColor(Rayd theRay, int rDepth)
{
	// some useful constants
	Color3d white(1,1,1);
	Color3d black(0,0,0);

	// check for intersections
	Intersection intersectionInfo;
	intersectionInfo.theRay=theRay;
    //Color3d mspec = intersectionInfo.material->getSpecular();
	double dist = root->intersect(intersectionInfo);

    if (dist <=EPSILON) {
		return background;
    }

	// intersection found so compute color
	Color3d color;

	// check for texture
    if (intersectionInfo.textured){
        
    }
    
	// add emissive term
    color += intersectionInfo.material->getEmissive();
	// add ambient term
    color += intersectionInfo.material->getAmbient();
    
    // add contribution from each light
    for (VECTOR(Light*)::iterator theLight = lights.begin(); theLight != lights.end(); ++theLight)
    {
        if (!((*theLight)->getShadow(intersectionInfo, root))) {
            color += (*theLight)->getDiffuse(intersectionInfo);
            color += (*theLight)->getSpecular(intersectionInfo);
        }
    }

    color.clampTo(0,1);
	// stop if no more recursion required
    if (rDepth == 0)
		return color; // done
        

	// stop if we are already at white
	
	if (color==white) // can't add any more
		return color;

    Vector3d v = theRay.getDir();
    Vector3d n = intersectionInfo.normal;
    if (n.dot(v) > 0){
        n = (-1) * n;
    }
    Vector3d vPrime = v + 2 * (-v.dot(n))*n;
    
    Rayd newRay;
    newRay.setDir(vPrime);
    newRay.setPos(intersectionInfo.iCoordinate+EPSILON*n);
	// recursive step
    Color3d RecColor = getColor(newRay, rDepth-1);
	// reflection
    for (int i = 0; i < 3; ++i) color[i] += (intersectionInfo.material->getSpecular()[i]) * RecColor[i];

	// transmission
    
    Rayd transRay;
    Vector3d vTrans;
    double refind = intersectionInfo.material->getRefind();
    double ktrans = intersectionInfo.material->getKtrans();
    // compute transmitted ray using snell's law
    float beta;
    
    if (n.dot(v) <= 0){
        beta = refind;
        intersectionInfo.entering = true;
    }
    else{
        beta = 1/refind;
        n = (-1) * n;
        intersectionInfo.entering = false;
    }
    
    float thetaIn = acos(v.dot(-n));
    float thetaOut = asin(beta * sin(thetaIn));
    
    if(beta * (sin(thetaIn)) < 0){
        cout<<"Error on beta sin thetaIn for transmission"<<endl;
    }
    else if (beta * (sin(thetaIn)) > 1){
        //no transmission
    }
    else if (beta * (sin(thetaIn)) == 0){
        vTrans = v;
        //same lines as below
        transRay.setDir(vTrans);
        transRay.setPos(intersectionInfo.iCoordinate+EPSILON*n);
        
        for (int i = 0; i < 3; ++i){
            color[i] += ktrans * (intersectionInfo.material->getSpecular()[i]) *getColor(transRay, rDepth-1)[i];
        }
    }
    else{
        Vector3d vs = (v - cos(thetaIn) * (-n)) / sin(thetaIn);
    
        vTrans = cos(thetaOut) * (-n) + sin(thetaOut) * vs.normalize();
    
        transRay.setDir(vTrans);
        transRay.setPos(intersectionInfo.iCoordinate+EPSILON*n);

        for (int i = 0; i < 3; ++i){
            color[i] += ktrans * (intersectionInfo.material->getSpecular()[i]) *getColor(transRay, rDepth-1)[i];
        }
    }
    
    color.clampTo(0,1);

	return color;
}

