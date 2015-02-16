class Camera;

#ifndef CAMERA_H
#define CAMERA_H


#include "common.h"
#include "geometry.h"


/*
* camera class that supports translation and rotation, and can setup
* OpenGL's matrices for the appropriate view angle.
*/
class Camera
{
public:
	Camera ();
	~Camera ();

	void		gluPerspectiveWrapper ();
	void		gluLookAtWrapper ();

	istream&	read  (istream& in);
	ostream&	write (ostream& out);

	Point3d		getPos		() { return pos; }
	Vector3d	getDir		() { return dir; }
	Vector3d	getUp		() { return up; }
	Vector3d	getRight	() { return right; }
	double		getHalfHeightAngle	() { return halfHeightAngle; }

	void		translate (double dx, double dy, double dz);
	void		lockRotate ();
	void		unlockRotate ();
	void		rotateXY  (double theta_, double pitch_);
	void		rotateZ   (double theta_);

private:

	Point3d		pos;
	Vector3d	dir;
	Vector3d	up;
	Vector3d	right;
	double		halfHeightAngle;	// in degrees

	bool		locked;
	Vector3d	oldup;
	Vector3d	olddir;
	Vector3d	oldright;
	double		theta;
	double		pitch;


};


istream& operator>> (istream& in,  Camera& c);
ostream& operator<< (ostream& out, Camera& c);


#endif // CAMERA_H
