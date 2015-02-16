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
    Point3d leftCorner = origin + D * camera->getDir() + (imageHeight/2) * camera->getUp() - (imageWidth/2) * camera->getRight();
    Point3d center = origin + D * camera->getDir();
    
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
            Vector3d pixelV = ((double) 1/distance(difference))*((Vector3d) (pixel - origin));
            Rayd theRay(origin, pixelV);
			// get the color at the closest intersection point

			Color3d theColor = getColor(theRay, options->recursiveDepth);

			// the image class doesn't know about color3d so we have to convert to pixel
			// update pixel
			Pixel p;

			p.r = theColor[0];
			p.g = theColor[1];
			p.b = theColor[2];

			image->setPixel(i, j, p);

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
	double dist = root->intersect(intersectionInfo);

    if (dist <=EPSILON) {
		return background;
    }

	// intersection found so compute color
	Color3d color;

	// check for texture

	// add emissive term

	// add ambient term
    
    
    // add contribution from each light
    for (VECTOR(Light*)::iterator theLight = lights.begin(); theLight != lights.end(); ++theLight)
    {
        if (!((*theLight)->getShadow(intersectionInfo, root))) {
            color += (*theLight)->getDiffuse(intersectionInfo);
            color += (*theLight)->getSpecular(intersectionInfo);
        }
    }


	// stop if no more recursion required
	if (rDepth == 0)
		return color; // done


	// stop if we are already at white
	color.clampTo(0,1);
	if (color==white) // can't add any more
		return color;

	// recursive step

	// reflection

	// transmission

	// compute transmitted ray using snell's law

	return color;
}

