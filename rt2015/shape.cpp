#include "shape.h"
#include "rayfile.h"


Shape::Shape ()
: ShapeGroup ()
{
  memset(matname, 0, MAX_NAME);
  material = NULL;
  textured = false;
  bumpMapped=false;
  bumpScale=0;
}


Shape::~Shape ()
{
}


ostream& operator<< (ostream& out, Shape& s)
{
  return s.write(out);
}


istream& operator>> (istream& in, Shape& s)
{
  return s.read(in);
}


int Shape::makeLinks (RayFile* rf)
{
  material = rf->getMaterial(matname);
 
  if (!material)
    return -1;
 
  return 0;
}
