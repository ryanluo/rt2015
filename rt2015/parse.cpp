#include "parse.h"
#include "shapegroup.h"
#include "group.h"
#include "sphere.h"
#include "triangle.h"
#include "box.h"
#include "cone.h"
#include "cylinder.h"
#include "torus.h"
#include "macroinstance.h"
#include "rayfileinstance.h"

#include <string.h>
#include <stdlib.h>
#include <errno.h>


static char linebuffer[MAX_LINE];


/*
 * grab a double off of the stream, skipping comments.  return 0.0
 * and set the stream's failbit in the case of an error
 */
double nextDouble (istream& in)
{
  memset(linebuffer, 0, MAX_LINE);

  if (in.fail())
    return 0.0;

  nextToken(in, linebuffer, MAX_LINE);

  if (in.fail())
    return 0.0;

  errno = 0;
  double result = atof(linebuffer);
  if (errno != 0)
  {
    cerr << "ERROR: expected double, got " << linebuffer << endl;
    in.setstate(istream::failbit);
    return 0;
  }

  return result;
}


/*
 * grab an int off of the stream, skipping comments.  return 0
 * and set the stream's failbit in the case of an error
 */
int nextInt (istream& in)
{
  memset(linebuffer, 0, MAX_LINE);

  if (in.fail())
    return 0;

  nextToken(in, linebuffer, MAX_LINE);

  if (in.fail())
    return 0;

  errno = 0;
  int result = atoi(linebuffer);
  if (errno != 0)
  {
    cerr << "ERROR: expected int, got " << linebuffer << endl;
    in.setstate(istream::failbit);
    return 0;
  }

  return result;
}


/*
 * grab a string off of the stream, skipping comments, and put it into
 * token.  size is the number of bytes in token, so we won't overwrite
 * it.  return NULL and set the stream's failbit in the case of an error
 * otherwise, returns token.
 */
char* nextToken (istream& in, char* token, int size)
{
		
  assert(size > 0);
  assert(token);

  token[0] = '\0';

  if (in.fail())
    return NULL;

  in >> token;

  if (in.fail())
  {
    token[0] = '\0';
    return NULL;
  }

  if (token[0] == '/')
  {
    if (token[1] == '/')
    {
      // end of line comment - ignore rest of line
      in.getline(token, size);

      return nextToken(in, token, size);
    }
    else if (token[1] == '*')
    {
      // block comment - get tokens until end sym, ignore everything else

      while (in && (token[0] != '*' || token[1] != '/'))
      {
        in >> token;

        if (token[0] == '/' && token[1] == '*')
          cerr << "WARNING: nested block comment ignored" << endl;
      }

      if (in.fail())
      {
        cerr << "WARNING: block comment not closed" << endl;
        return NULL;
      }

      return nextToken(in, token, size);
    }
  }

  return token;
}

/*
 * Returns the next token on the stream without removing the token from
 * the stream.
 */
char* peekToken (istream& in, char* token, int size)
{
	std::streamoff oldPos = in.tellg();
	
	nextToken(in, token, size);
	
	in.seekg(oldPos);



	return token;
}

/* 
 * Consumes the next token in the stream without returning it.
 */
void    chompToken(istream& in)
{
	char token[MAX_LINE];
	nextToken(in, token, MAX_LINE);

}


/*
 * parse flags, ending with a --  Read the comment in parse.h 
 * for a more detailed explaination
 */
int parseFlags (istream& in, Flag* flags, int& num_)
{
  if (num_ == 0)
    return 0;

  assert(flags);

  int num = num_;
  num_ = 0;

  bool* found = new bool[num];
  for (int i = 0; i < num; ++i)
    found[i] = false;

  char  token[MAX_LINE];
  Flag* flag = NULL;
  bool  done = false;

  // loop while there's more input and more flags to read
  while (in && !done)
  {
    nextToken(in, token, MAX_LINE);
    if (token[0] == '\0')
    {
      cerr << "ERROR: couldn't read next token" << endl;
      delete[] found;
      in.setstate(istream::failbit);
      return -1;
    }

    int offset = -1;
    for (int i = 0; (offset == -1) && (i < num); ++i)
    {
      flag = &flags[i];
      if (strncmp(token, flag->symbol, max(strlen(flag->symbol),
                                           strlen(token))) == 0)
        offset = i;
    }

    if (offset == -1)
    {
      if (token[0] == '-' && token[1] == '-')
      {
        done = true;
      }
      else if (token[0] == '-')
      {
        cerr << "WARNING: ignoring unrecognized flag: " << token << endl;
      }
      else
      {
        cerr << "ERROR: unexpected symbol: " << token << endl;
        delete[] found;
        in.setstate(istream::failbit);
        return -1;
      }
    }
    else
    {
      if (found[offset])
      {
        cerr << "ERROR: duplicate flag: " << token << endl;
        delete[] found;
        in.setstate(istream::failbit);
        return -1;
      }
      found[offset] = true;

      switch (flag->type)
      {
      case STRING:
        nextToken(in, token, MAX_LINE);
        strncpy((char*)flag->val, token, flag->size);
        break;

      case DOUBLE:
        *(double*)flag->val = nextDouble(in);
        break;

      case INT:
        *(int*)flag->val = nextInt(in);
        break;

      case BOOL:
        *(bool*)flag->val = true;
        break;

      default:
        cerr << "ERROR: invalid flag type: " << flag->type << endl;
        delete[] found;
        in.setstate(istream::failbit);
        return -1;
      }

      if (token[0] == '\0')
      {
        cerr << "ERROR: couldn't read next token" << endl;
        delete[] found;
        in.setstate(istream::failbit);
        return -1;
      }

      ++num_;
    }
  }

  if (!done)
    cerr << "WARNING: -- never reached" << endl;

  for (int i = 0; i < num; ++i)
  {
    if (flags[i].required && !found[i])
    {
      cerr << "ERROR: missing required flag: " << flags[i].symbol << endl;
      delete[] found;
      in.setstate(istream::failbit);
      return -1;
    }
  }

  delete[] found;
  return 0;
}


/*
 * parse shapegroup directives and return the pointer to the new object.
 * return NULL if the token isn't a shapegroup
 *
 * to extend parsing to include additional primitives, add them here
 */
ShapeGroup* parseShapeGroup (istream& in, char* token, int size, RayFile *file)
{
  ShapeGroup* sg = NULL;

  if (strncmp(token, "#group_begin", 12) == 0)
  {
    sg = new Group(file);
    in >> *sg;
    return sg;
  }
  else if (strncmp(token, "#sphere", 7) == 0)
  {
    sg = new Sphere();
    in >> *sg;
    return sg;
  }
  else if (strncmp(token, "#triangle", 9) == 0)
  {
    sg = new Triangle();
    in >> *sg;
    return sg;
  }
 else if (strncmp(token, "#cylinder", 9) == 0)
  {
    sg = new Cylinder();
    in >> *sg;
    return sg;
  }
  else if (strncmp(token, "#cone", 5) == 0)
  {
    sg = new Cone();
    in >> *sg;
    return sg;
  }
  else if (strncmp(token, "#box", 4) == 0)
  {
    sg = new Box();
    in >> *sg;
    return sg;
  }
  else if (strncmp(token, "#torus", 6) == 0)
  {
    sg = new Torus();
    in >> *sg;
    return sg;
  }

  else if (strncmp(token, "#macro_instance", 15) == 0)
  {
    sg = new MacroInstance();
    in >> *sg;
    return sg;
  }
  else if (strncmp(token, "#rayfile_instance", 17) == 0)
  {
    sg = new RayFileInstance();
    in >> *sg;
    return sg;
  }

  return sg;
}
