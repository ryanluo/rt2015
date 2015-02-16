#include "rayfile.h"
#include "camera.h"
#include "light.h"
#include "directionallight.h"
#include "pointlight.h"
#include "spotlight.h"
#include "material.h"
#include "group.h"
#include "sphere.h"
#include "triangle.h"
#include "box.h"
#include "cone.h"
#include "cylinder.h"
#include "torus.h"
#include "parse.h"
#include "macro.h"
#include "macroinstance.h"
#include "rayfileinstance.h"
#include "main.h"

#include <stdio.h>
#include <vector>
#include <map>
#include <fstream>


/*
 * RayFile::raytrace and RayFile::getColor are both in 
 * rayfile_render.cpp, not in this file!  You shouldn't need to modify
 * anything in here for any required points!
 */

static const GLenum glLightEnum[8] = { GL_LIGHT0, GL_LIGHT1, GL_LIGHT2,
                                       GL_LIGHT3, GL_LIGHT4, GL_LIGHT5,
                                       GL_LIGHT6, GL_LIGHT7 };


RayFile::RayFile ()
{
  memset(name, 0, MAX_NAME);
  memset(file, 0, MAX_PATH);
  memset(dir,  0, MAX_PATH);

  camera = new Camera();
  root   = new Group(this);
}


RayFile::~RayFile ()
{
  if (camera) delete camera;
  if (root)   delete root;

  /*
   * rayfile is responsible for all the memory, so go through each
   * container of objects and delete each pointer individually.  then
   * clear the container
   */
  for (VECTOR(Light*)::iterator l = lights.begin(); 
       l != lights.end(); 
       ++l)
    delete *l;
  lights.clear();
    
  for (MAP(string, Material*)::iterator m = materials.begin();
       m != materials.end();
       ++m)
    delete m->second;
  materials.clear();

  for (MAP(string, Macro*)::iterator m = macros.begin();
       m != macros.end();
       ++m)
    delete m->second;
  macros.clear();

  for (MAP(string, RayFile*)::iterator rf = rayfiles.begin();
       rf != rayfiles.end();
       ++rf)
    delete rf->second;
  rayfiles.clear();
}


void RayFile::clear ()
{
  // first delete all the old stuff
  if (camera) delete camera;
  if (root)   delete root;

  for (VECTOR(Light*)::iterator l = lights.begin(); 
       l != lights.end(); 
       ++l)
    delete *l;
  lights.clear();
    
  for (MAP(string, Material*)::iterator m = materials.begin();
       m != materials.end();
       ++m)
    delete m->second;
  materials.clear();

  for (MAP(string, Macro*)::iterator m = macros.begin();
       m != macros.end();
       ++m)
    delete m->second;
  macros.clear();

  for (MAP(string, RayFile*)::iterator rf = rayfiles.begin();
       rf != rayfiles.end();
       ++rf)
    delete rf->second;
  rayfiles.clear();
  // done deleting the old stuff

  // now reinit the rest of it
  memset(name, 0, MAX_NAME);
  memset(file, 0, MAX_PATH);
  memset(dir,  0, MAX_PATH);
 
  camera = new Camera();
  root   = new Group(this); 
}


void RayFile::glDraw ()
{
  // setup projection matrix
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  camera->gluPerspectiveWrapper();

  // camera position in the modelview matrix
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  camera->gluLookAtWrapper();

  if (options->lighting)
  {
    glEnable(GL_LIGHTING);

    int i = 0;
    for (VECTOR(Light*)::iterator l = lights.begin(); 
         i < 9 && l != lights.end(); 
         ++l, ++i)
    {
      (*l)->glLoad(glLightEnum[i]);
      (*l)->glDraw();
    }
  }

  if (options->backface)
  {
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
  }

  if (options->transparency)
  {
    /*
     * do a two pass render.  first, only draw opaque objects (that is,
     * alpha == 1.0).  then, turn on blending, turn off writing to the
     * depth buffer, and draw all the transparent objects (that is,
     * alpha != 1.0).  this is far from perfect, but it makes an
     * approximation of translucent behavior.
     */
    glEnable(GL_ALPHA_TEST);

    glAlphaFunc(GL_EQUAL, 0.0);
    root->glDraw();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glAlphaFunc(GL_NOTEQUAL, 0.0);
    glDepthMask(GL_FALSE);
    root->glDraw();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
  }
  else
  {
    root->glDraw();
  }

  glDisable(GL_LIGHTING);
  glDisable(GL_CULL_FACE);
}


void RayFile::setFile (const char* file_)
{
  memset(file, 0, MAX_PATH);
  strncpy(file, file_, MAX_PATH);

  setDir (NULL);
}


void RayFile::setDir (const char* relativeTo)
{
  memset(dir, 0, MAX_PATH);

  bool done = false;
  int i;
  for (i = MAX_PATH - 1; !done && i >= 0; --i)
    if (file[i] == '/')
      done = true;

  int len = 1;
  if (relativeTo)
  {
    len = (int) strlen(relativeTo) + 1;
    strncpy(dir, relativeTo, len);
  }

  if (i > 0)
  {
    if (relativeTo)
      strncat(dir, "/", 2);

    strncat(dir, file, i+1);

    char temp[MAX_PATH];
    strncpy(temp, file, MAX_PATH);
    memset(file, 0, MAX_PATH);
    strncpy(file, temp+i+2, strlen(temp+i+2));
  }
}


Material* RayFile::getMaterial (const char* label)
{
  MAP(string, Material*)::iterator pos = materials.find(label);

  if (pos == materials.end())
  {
    cerr << "ERROR: RayFile: couldn't find Material: " << label << endl;
    return NULL;
  }

  return pos->second;
}


Macro* RayFile::getMacro (const char* label)
{
  MAP(string, Macro*)::iterator pos = macros.find(label);

  if (pos == macros.end())
  {
    cerr << "ERROR: RayFile: couldn't find Macro: " << label << endl;
    return NULL;
  }

  return pos->second;
}


RayFile* RayFile::getRayFile (const char* label)
{
  MAP(string, RayFile*)::iterator pos = rayfiles.find(label);

  if (pos == rayfiles.end())
  {
    cerr << "ERROR: RayFile: couldn't find RayFile: " << label << endl;
    return NULL;
  }

  return pos->second;
}


istream& RayFile::read (istream& in)
{
  char   next[MAX_LINE];
       
  bool done = false;
  while (in && !done)
  {
    nextToken(in, next, MAX_LINE);

    if (strncmp(next, "#version", 8) == 0)
    {
      nextToken(in, next, MAX_LINE);

      if (strncmp(next, VersionTag02, strlen(VersionTag02)) == 0)
      {
        done = true;
		strcpy(version, VersionTag02);
      }
	  else if (strncmp(next, VersionTag05, strlen(VersionTag05)) == 0)
      {
        done = true;
		strcpy(version, VersionTag05);
      }
      else
      {
        cerr << "ERROR: RayFile: unrecognized file type" << endl;
        in.setstate(istream::failbit);
        return in;
      }
    }
  }

  if (!in)
  {
    cerr << "ERROR: RayFile: no RayFile found on stream" << endl;
    cerr << "     : possibly missing #version directive?" << endl;
    return in;
  }

  // parse while we have a good stream and haven't reached #rayfile_end
  done = false;
  while (in && !done)
  {       
    nextToken(in, next, MAX_LINE);

    if (in && next[0] == '#')
    {  
      if (strncmp(next, "#rayfile_end", 12) == 0)
      {
        // here's the end of the file
        done = true;
      }
      else if (strncmp(next, "#background", 10) == 0)  
      {
        in >> background;
        glClearColor(background[0], background[1], background[2], 0.0);
      }
      else if (strncmp(next, "#ambient", 7) == 0)   
      {
        in >> ambient;
        float vals[4] = { static_cast<float>(ambient[0]), static_cast<float>(ambient[1]), static_cast<float>(ambient[2]), 1.0 };
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, vals);
      } 
      else if (strncmp(next, "#camera", 6) == 0)
      {
        in >> *camera;
      }
      else if (strncmp(next, "#light_dir", 9) == 0)
      {
        DirectionalLight* dl = new DirectionalLight();
        in >> *dl;
        lights.push_back(dl);
      }   
      else if (strncmp(next, "#light_point", 11) == 0)
      {
        PointLight* pl = new PointLight();
        in >> *pl;
        lights.push_back(pl);
      }
      else if (strncmp(next, "#light_spot", 10) == 0)
      {
        SpotLight* sl = new SpotLight();
        in >> *sl;
        lights.push_back(sl);
      }
      else if (strncmp(next, "#material", 8) == 0)
      {
        Material* material = new Material();
        material->setMatDirectory(dir);
        in >> *material;
        materials[material->getName()] = material;
      }
      else if (strncmp(next, "#macro_begin", 11) == 0)
      {
        Macro* macro = new Macro(this);
        in >> *macro;
        macros[macro->getName()] = macro;
      }
      else if (strncmp(next, "#rayfile_include", 17) == 0)
      {
        includeRayFile(in);
      }
      else
      {
        // see if it's a shape of some sort
        ShapeGroup* sg = parseShapeGroup(in, next, MAX_LINE, this);

        // it isn't, so we don't recognize the directive
        if (!sg)
        {
          cerr << "ERROR: RayFile: unrecognized directive: " << next << endl;
          in.setstate(istream::failbit);
          return in;
        }

        // add the shape to our group
        root->add(sg);
      }
    }
    else if (in)
    {   
      cerr << "ERROR: RayFile: unrecognized symbol: " << next << endl;
      in.setstate(istream::failbit);
      return in;
    }
  }

  // stream failed before eof...not sure what went wrong
  if (!done && !in.eof())
  {    
    cerr << "ERROR: RayFile: unknown stream error" << endl;
    return in;
  }

  // now make the links between materials and macros and such
  if (root->makeLinks(this) == -1)
  {
    cerr << "ERROR: RayFile: couldn't make links" << endl;
    in.setstate(istream::failbit);
    return in;
  }

  return in;
}


ostream& RayFile::write (ostream& out)
{
  out << "#version HMCCS155FALL2002" << endl << endl;
  out << "#background " << background << endl;
  out << *camera << endl;
  out << "#ambient " << ambient << endl << endl;

  for (VECTOR(Light*)::iterator l = lights.begin(); 
       l != lights.end(); 
       ++l)
    out << **l << endl;
  out << endl;
    
  for (MAP(string, Material*)::iterator m = materials.begin();
       m != materials.end();
       ++m)
    out << *(m->second) << endl;
  out << endl;

  for (MAP(string, Macro*)::iterator m = macros.begin();
       m != macros.end();
       ++m)
    out << *(m->second) << endl;
  out << endl;

  for (MAP(string, RayFile*)::iterator rf = rayfiles.begin();
       rf != rayfiles.end();
       ++rf)
    out << *(rf->second) << endl;
  out << endl;

  // does this change work?
  for (VECTOR(ShapeGroup*)::iterator sg = root->begin(); sg != root->end(); ++sg)
    out << *sg << endl;


  out << "#rayfile_end" << endl;

  return out;
}


istream& operator>> (istream& in, RayFile& rf)
{
  return rf.read(in);
}


ostream& operator<< (ostream& out, RayFile& rf)
{
  return rf.write(out);
}


/*
 * need a separate function to include another rayfile, cause we
 * already use >> for reading in a top-level rayfile.  this function
 * reads an external rayfile, sets up its directory value and lets
 * it run
 */
istream& RayFile::includeRayFile (istream& in)
{
  RayFile* rayfile = new RayFile();

  int numFlags = 2;
  Flag flags[2] = { { (char*)"-n", STRING, true, MAX_NAME, rayfile->name },
                    { (char*)"-f", STRING, true, MAX_PATH, rayfile->file }
                  };

  if (parseFlags(in, flags, numFlags) == -1)
  {
    cerr << "ERROR: RayFile : flag parsing failed!" << endl;
    return in;
  }

  rayfiles[rayfile->getName()] = rayfile;

  rayfile->setDir(dir);
  char filename[MAX_PATH];
  strncpy(filename, rayfile->dir, MAX_PATH);
  strncat(filename, "/", 2);
  strncat(filename, rayfile->file, MAX_PATH);

  ifstream sourceFile(filename);
  if (sourceFile.good())
  {
    sourceFile >> *rayfile;
    sourceFile.close();
  }
  else
  {
    cerr << "ERROR: RayFile: couldn't open included rayfile "
         << rayfile->getName() << " : " << rayfile->getFile() << endl;
    in.setstate(istream::failbit);
    return in;
  }

  return in;
}
