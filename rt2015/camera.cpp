#include "camera.h"
#include "parse.h"
#include "main.h"



Camera::Camera ()
{
  locked = false;
}


Camera::~Camera ()
{
}


void Camera::lockRotate ()
{
  olddir   = dir;
  oldup    = up;
  oldright = right;
  theta    = 0;
  pitch    = 0;
  locked   = true;
}


void Camera::unlockRotate ()
{
  locked = false;
}


void Camera::translate (double dx, double dy, double dz)
{
  pos += right * dx * -1.0;
  pos += up * dy;
  pos += dir * dz * -1.0;
}


/*
 * rotate about the up and right vector simultaneously via
 * quaternion rotation composition
 */
void Camera::rotateXY (double theta_, double pitch_)
{
  if (!locked)
	  return;		// this replaces the assert locked which seems
					// to fail
  //assert(locked);	// have to call lockRotate() first!

  theta += theta_;
  pitch += pitch_;

  // q1 = (s1, v1), the quaternion to rotate around the up axis
  double   s1 = cos(theta / 2.0);
  Vector3d v1 = oldup * sin(theta / 2.0);

  // q2 = (s2, v2), the quaternion to rotate around the right axis
  double   s2 = cos(pitch / 2.0);
  Vector3d v2 = oldright * sin(pitch / 2.0);

  // q = (s,v) = q1*q2, the quaternion to do both rotations
  double   s = (s1 * s2) - v1.dot(v2);
  Vector3d v = (v1 * s2) + (v2 * s1) + v1.cross(v2);

  // now create the matrix that represents the rotation in q
  Matrixd m;
  m(0,0) = sqr(s) + sqr(v[0]) - sqr(v[1]) - sqr(v[2]);
  m(1,0) = 2*v[0]*v[1] - 2*s*v[2];
  m(2,0) = 2*v[0]*v[2] + 2*s*v[1];
  m(0,1) = 2*v[0]*v[1] + 2*s*v[2];
  m(1,1) = sqr(s) - sqr(v[0]) + sqr(v[1]) - sqr(v[2]);
  m(2,1) = 2*v[1]*v[2] - 2*s*v[0];
  m(0,2) = 2*v[0]*v[2] - 2*s*v[1];
  m(1,2) = 2*v[1]*v[2] + 2*s*v[0];
  m(2,2) = sqr(s) - sqr(v[0]) - sqr(v[1]) + sqr(v[2]);
  m(3,3) = sqr(s) + sqr(v[0]) + sqr(v[1]) + sqr(v[2]);

  // apply the rotation to our vectors and voila!
  dir   = m.multDir(olddir).getUnit();
  up    = m.multDir(oldup).getUnit();

  // now keep all three perpindicular
  right = dir.cross(up).getUnit();
  up    = right.cross(dir).getUnit();
}


/*
 * rotate around just the direction vector
 */
void Camera::rotateZ (double theta_)
{
	if (!locked)
	  return;		// this replaces the assert locked which seems
					// to fail
  //assert(locked);	// have to call lockRotate() first!

  theta += theta_;

  double ct = cos(theta);
  double st = sin(theta);

  Matrixd m;
  m(0,0) = ct + (1-ct) * olddir[0] * olddir[0];
  m(1,0) = (1-ct) * olddir[0] * olddir[1] - st * olddir[2];
  m(2,0) = (1-ct) * olddir[0] * olddir[2] + st * olddir[1];
  m(0,1) = (1-ct) * olddir[1] * olddir[0] + st * olddir[2];
  m(1,1) = ct + (1-ct) * olddir[1] * olddir[1];
  m(2,1) = (1-ct) * olddir[1] * olddir[2] - st * olddir[0];
  m(0,2) = (1-ct) * olddir[2] * olddir[0] - st * olddir[1];
  m(1,2) = (1-ct) * olddir[2] * olddir[1] + st * olddir[0];
  m(2,2) = ct + (1-ct) * olddir[2] * olddir[2];
  m(3,3) = 1;

  up    = m.multDir(oldup).getUnit();

  // now keep all three perpindicular
  right = dir.cross(up).getUnit();
  up    = right.cross(dir).getUnit();
}


void Camera::gluPerspectiveWrapper ()
{
  gluPerspective(halfHeightAngle *2,
                 ((double) options->width / (double) options->height),
                 0.1, 500);
}


void Camera::gluLookAtWrapper ()
{
  gluLookAt(pos[0], pos[1], pos[2],
            pos[0]+dir[0], pos[1]+dir[1], pos[2]+dir[2],
            up[0], up[1], up[2]);
}


ostream& operator<< (ostream& out, Camera& c)
{
  return c.write(out);
}


ostream& Camera::write (ostream& out)
{
  out << "#camera "     << endl
      << "   " << pos   << endl
      << "   " << dir   << endl
      << "   " << up    << endl
      << "   " << halfHeightAngle << endl;

  return out;
}


istream& operator>> (istream& in, Camera& c)
{
  return c.read(in);
}


istream& Camera::read (istream& in)
{
  in >> pos >> dir >> up;

  halfHeightAngle = nextDouble(in);

  // calculate right vector, and make sure up vector is perpendicular
  right = dir.cross(up);
  up    = right.cross(dir);

  // now normalize them all
  dir.normalize();
  right.normalize();
  up.normalize();

  if (in.fail())
  {
    cerr << "ERROR: Camera: unknown stream failure" << endl;
    return in;
  }

  return in;
}
