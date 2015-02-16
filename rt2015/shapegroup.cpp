#include "shapegroup.h"
#include <string.h>


ShapeGroup::ShapeGroup ()
{
  memset(name, 0, MAX_NAME);
}


ShapeGroup::~ShapeGroup ()
{
}


ostream& operator<< (ostream& out, ShapeGroup& sg)
{
  return sg.write(out);
}


istream& operator>> (istream& in, ShapeGroup& sg)
{
  return sg.read(in);
}
