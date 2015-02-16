#include "directionallight.h"
#include "shapegroup.h"
#include "geometry.h"
#include "parse.h"
#include "material.h"
#include "main.h"
#include "common.h"


DirectionalLight::DirectionalLight ()
: Light ()
{
}


DirectionalLight::~DirectionalLight ()
{
}


Color3d DirectionalLight::getDiffuse (Intersection& info)
{
  /*
   * Intensity of diffuse lighting is equal to the dot product of
   * the normal with the vector opposite the incident light's direction  
   */

  double angleFactor = -direction.dot(info.normal); 

  if (angleFactor<0)
	  return Color3d(0,0,0);  // light is falling on other side of surface
  else 
	  return color * info.material->getDiffuse(info) * angleFactor;

}


Color3d DirectionalLight::getSpecular (Intersection& info)
{
  /* 
   * Intensity of specular lighting is the dot product of the light's
   * reflected ray and the ray pointing to the viewer, raised to
   * some power (in this case, kshine). 
   * Note:  we are using a slightly different model than opengl
   * so our highlights will appear more focused.
   */ 

	
    
	Vector3d reflect=direction - info.normal*(2*direction.dot(info.normal));
	reflect.normalize();
	double angleFactor = - reflect.dot(info.theRay.getDir());
	if (angleFactor<0)
		return Color3d(0,0,0);
	else {
		angleFactor = pow(angleFactor,info.material->getKshine());
		return   color * info.material->getSpecular() * angleFactor;
	}
	



}


bool DirectionalLight::getShadow (Intersection& iInfo, ShapeGroup* root)
{
  /*
   * To determine if an intersection is in shadow or not with respect
   * to a light, cast a ray from the intersection towards the light
   * and see if it intersects anything.
   */

	
	if (direction.dot(iInfo.normal)>0)
		return true;
	// otherwise we'll check
	Rayd shadowRay;
	shadowRay.setDir(direction*-1);
	shadowRay.setPos(iInfo.iCoordinate + iInfo.normal * EPSILON);
	Intersection tmpInfo;
	tmpInfo.theRay=shadowRay;
	if (root->intersect(tmpInfo) > EPSILON)
		return true;
	return false;

}


void DirectionalLight::glLoad (GLenum light)
{
  /*
   * load the specified OpenGL light as a directional light with the 
   * correct direction and color.
   */

  // unfortunately, we need our values in this format for OpenGL
  GLfloat vals[2][4] = { { static_cast<GLfloat>(-direction[0]), static_cast<GLfloat>(-direction[1]), static_cast<GLfloat>(-direction[2]), 0.0 },
                         {  static_cast<GLfloat>(color[0]),      static_cast<GLfloat>(color[1]),      static_cast<GLfloat>(color[2]),     0.0 }
                       };

  glLightfv(light, GL_POSITION, vals[0]);
  glLightfv(light, GL_DIFFUSE,  vals[1]);
  glLightfv(light, GL_SPECULAR, vals[1]);

  glEnable(light);
}


void DirectionalLight::glDraw ()
{
  // no rendering for directional lights
}


ostream&  DirectionalLight::write       (ostream&       out)
{
  out << "#light_dir" << flush;
  if (name[0] != '\0')
    out << " -n " << name << flush;
  out << " --" << endl;
  
  out << "  " << color << endl;
  out << "  " << direction << endl;

  return out;
}

  
istream& DirectionalLight::read (istream& in)
{
  int numFlags = 1;
  Flag flags[1] = { { (char*)"-n", STRING, false,  MAX_NAME, name }
                  };

  if (parseFlags(in, flags, numFlags) == -1)
  {
    cerr << "ERROR: Directional Light: flag parsing failed!" << endl;
    return in;
  }

  in >> color >> direction;

  direction.normalize();

  if (in.fail())
  {
    cerr << "ERROR: Directional Light: unknown stream failure" << endl;
    return in;
  }

  return in;
}


istream& operator>> (istream& in, DirectionalLight& l)
{
  return l.read(in);
}


ostream& operator<< (ostream& out, DirectionalLight& l)
{
  return l.write(out);
}

