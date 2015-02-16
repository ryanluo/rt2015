#include "rayfileinstance.h"
#include "rayfile.h"
#include "group.h"
#include "parse.h"


RayFileInstance::RayFileInstance ()
: ShapeGroup ()
{
  rayfile = NULL;
  root    = NULL;
}


RayFileInstance::~RayFileInstance ()
{
}


double RayFileInstance::intersect (Intersection& info)
{
  if (root)
    return root->intersect(info);
  else
    return -1.0;
}


void RayFileInstance::glDraw ()
{
  if (root)
    root->glDraw();
}


int RayFileInstance::makeLinks (RayFile* rf)
{
  rayfile = rf->getRayFile(name);

  if (!rayfile)
    return -1;

  root = rayfile->root;

  return root->makeLinks(rayfile);
}


istream& RayFileInstance::read (istream& in)
{
  nextToken(in, name, MAX_NAME);

  if (in.fail())
  {
    cerr << "ERROR: RayFileInstance: unknown stream error" << endl;
    return in;
  }
 
  return in;
}


ostream& RayFileInstance::write (ostream& out)
{
  out << "#rayfile_instance " << name << endl;

  return out;
}


ostream& operator<< (ostream& out, RayFileInstance& i)
{
  return i.write(out);
}


istream& operator>> (istream& in, RayFileInstance& i)
{
  return i.read(in);
}
