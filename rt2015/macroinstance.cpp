#include "macroinstance.h"
#include "macro.h"
#include "group.h"
#include "parse.h"
#include "rayfile.h"


MacroInstance::MacroInstance ()
: ShapeGroup ()
{
  macro = NULL;
  root  = NULL;
}


MacroInstance::~MacroInstance ()
{
}


double MacroInstance::intersect (Intersection& info)
{
  if (root)
    return root->intersect(info);
  else
    return -1.0;
}


void MacroInstance::glDraw ()
{
  if (root)
    root->glDraw();
}


int MacroInstance::makeLinks (RayFile* rf)
{
  macro = rf->getMacro(name);

  if (!macro)
    return -1;

  root = macro->root;

  return root->makeLinks(rf);
}


istream& MacroInstance::read (istream& in)
{
  nextToken(in, name, MAX_NAME);

  if (in.fail())
  {
    cerr << "ERROR: MacroInstance: unknown stream error" << endl;
    return in;
  }

  return in;
}


ostream& MacroInstance::write (ostream& out)
{
  out << "#macro_instance " << name << endl;

  return out;
}


ostream& operator<< (ostream& out, MacroInstance& i)
{
  return i.write(out);
}


istream& operator>> (istream& in, MacroInstance& i)
{
  return i.read(in);
}
