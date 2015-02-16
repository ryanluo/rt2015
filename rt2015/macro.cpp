#include "macro.h"
#include "group.h"
#include "parse.h"


Macro::Macro (RayFile *file)
{
  this->file = file;
  root = new Group(file);
}


Macro::~Macro ()
{
  delete root;
}


istream& operator>> (istream& in, Macro& m)
{
  return m.read(in);
}


ostream& operator<< (ostream& out, Macro& m)
{
  return m.write(out);
}


istream& Macro::read (istream& in)
{
  int numFlags = 1;
  Flag flags[1] = { { (char*)"-n", STRING, true,  MAX_NAME, name } };
 
  if (parseFlags(in, flags, numFlags) == -1)
  {
    cerr << "ERROR: Macro: flag parsing failed!" << endl;
    return in;
  }

  char  next[MAX_LINE];
  bool  done = false;

  // parse while we have a good stream and haven't reached #macro_end
  while (in && !done)
  {
    nextToken(in, next, MAX_LINE);
    
    if (next[0] == '#')
    {
      if (strncmp(next, "#macro_end", 10) == 0)
      {
        // end of macro definition
        done = true;
      }
      else
      {
        // check if it's a shape
        ShapeGroup* sg = parseShapeGroup(in, next, MAX_LINE, file);
 
        // not a shape, so we don't recognize it
        if (!sg)
        {
          cerr << "ERROR: Macro: unrecognized directive: " << next << endl;
          in.setstate(istream::failbit);
          return in;
        }
 
        // add the shape to the macro
        root->add(sg);
      }
    }
    else
    {
      cerr << "ERROR: Macro: unrecognized symbol: " << next << endl;
      in.setstate(istream::failbit);
      return in;
    }
  }

  // stopped parsing because the stream failed...
  if (!done)
  {
    cerr << "ERROR: Macro: missing #macro_end" << endl;
     
    if (!in.eof())
    {  
      cerr << "ERROR: Macro: unknown stream failure" << endl;
    }
      
    return in;
  }
  
  return in;
}


ostream& Macro::write (ostream& out)
{
  out << "#macro_begin " << flush;
  if (name[0] != '\0')
    out << "-n " << name << flush;
  out << " --" << endl << endl;

  /* does this work? */
  for (VECTOR(ShapeGroup*)::iterator sg = root->begin(); sg != root->end(); ++sg)
    out << **sg << endl;

  out << "#macro_end" << endl;

  return out;
}
