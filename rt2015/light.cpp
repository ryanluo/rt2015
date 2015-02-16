#include "light.h"
#include <string.h>


Light::Light()
{
  memset(name, 0, MAX_NAME);
}


Light::~Light()
{
}


istream& operator>> (istream& in, Light& l)
{
  return l.read(in);
}


ostream& operator<< (ostream& out, Light& l)
{
  return l.write(out);
}


const char* Light::getName()
{
  return name;
}
