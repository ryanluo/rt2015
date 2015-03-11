#include "material.h"
#include "parse.h"
#include "image.h"
#include "main.h"
#include "rayfile.h"
#include "perlin.h"



   
const Color3d Material::getDiffuse(Intersection& info)
  {
	  if (!textured() && !procTextured())
		  return diffuse;
	  if (textured())
		  return getTexture(info.texCoordinate[0], info.texCoordinate[1]);
	  return getProceduralTexture(info.iCoordinate);
  }

Material::Material ()
{
  memset(name,    0, MAX_NAME);
  memset(texname, 0, MAX_PATH);
  texture=NULL;
  bumpMap=NULL;
  pNoise[0]=pNoise[1]=pNoise[2]=NULL;
  pOctaves=3;
  pPersistence=.5;
  glTexID = 0;
}


Material::~Material ()
{
  if (texture) 
  {
    delete texture;
    glDeleteTextures(1, &glTexID);
  }
  if (bumpMap)
	  delete bumpMap;
}


bool Material::textured ()
{
  return (texture != NULL);
}

bool Material::bumpMapped ()
{
  return (bumpMap != NULL);
}

bool Material::procTextured()
{ 
	if (pNoise[0]==NULL)
		return false;

	return true;
}

const Color3d Material::getTexture(double u, double v)
{

    Color3d tempColor;
    double imageU;
    double imageV;
    imageU = u*texture->getWidth();
    imageV = v*texture->getHeight();
    
    for (int i = 0; i < 3; ++i){
        tempColor[i] = texture->getPixel_(imageU, imageV, i);
    }
	return tempColor;
}
const Color3d Material::getProceduralTexture(Point3d point)
{

	
	return Color3d(0,0,0);

}
void Material::bumpNormal (Vector3d& normal, 
						   Vector3d& up,
						   Vector3d& right,
						   Intersection& info,
						   double bumpScale)
{
	if (!options->bumpMaps || !bumpMapped())
		return;

	if (bumpMap != NULL)
	{	
        double x;
        double y;
        int width = bumpMap->getWidth();
        int height = bumpMap->getHeight();
        x = info.texCoordinate[0]*(double)width;
        y = info.texCoordinate[1]*(double)height;
        
        double p1 = bumpMap->getPixel(floor(x), y, 0);
        double p2 = bumpMap->getPixel(((int)floor(x+1))%width, y, 0);
        double p3 = bumpMap->getPixel(x, floor(y), 0);
        double p4 = bumpMap->getPixel(x, (int)floor(y+1)%height, 0);
        
        double dx = p2-p1;
        double dy = p3-p4;
        
        Vector3d perturb = dx*right+dy*up;
        perturb *= bumpScale;
        info.normal += perturb;
        info.normal.normalize();
	}
	return;
}

void Material::glLoad ()
{
  /*
   * load the material's properties into OpenGL...turn on
   * texturing if necessary.
   */

  // if lighting is on, set material properties
  if (options->lighting)
  {
    // unfortunately, opengl needs the args in this format
    GLfloat vals[4][4] = { { static_cast<GLfloat>(ambient[0]),  static_cast<GLfloat>(ambient[1]),  static_cast<GLfloat>(ambient[2]),  static_cast<GLfloat>(ktrans) },
                           { static_cast<GLfloat>(diffuse[0]),  static_cast<GLfloat>(diffuse[1]),  static_cast<GLfloat>(diffuse[2]),  static_cast<GLfloat>(ktrans) },
                           { static_cast<GLfloat>(specular[0]), static_cast<GLfloat>(specular[1]), static_cast<GLfloat>(specular[2]), static_cast<GLfloat>(ktrans) },
                           { static_cast<GLfloat>(emissive[0]), static_cast<GLfloat>(emissive[1]), static_cast<GLfloat>(emissive[2]), static_cast<GLfloat>(ktrans) }
                         };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   vals[0]);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   vals[1]);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  vals[2]);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  vals[3]);
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, kshine );
  }
  // if lighting is off, just set the color
  else
  {
    (ambient * scene->getAmbient() + emissive).glLoad();
  }

  // separate the specular light out for texture calculations
  if (texture && options->textures)
  {
#ifdef GL_VERSION_1_2
    glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
#endif

    glBindTexture(GL_TEXTURE_2D, glTexID);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  }
}


void Material::glUnload ()
{
  // go back to normal single color mode
  if (texture && options->textures)
  {
#ifdef GL_VERSION_1_2
    glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SINGLE_COLOR);
#endif

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
  }
}


ostream& operator<< (ostream& out, Material& m)
{
  return m.write(out);
}


ostream& Material::write (ostream& out)
{
  out << "#material -n " << name << flush;
  if (texture)
    out << " -t " << texname << flush;
  out << " --" << endl;

  out << "  " << ambient << endl;
  out << "  " << diffuse << endl;
  out << "  " << specular << endl;
  out << "  " << emissive << endl;
  out << "  " << kshine << " " << ktrans << " " << refind << endl;

  return out;
}


istream& operator>> (istream& in, Material& m)
{
  return m.read(in);
}


istream& Material::read (istream& in)
{
  char texfile[MAX_PATH];
  texfile[0] = '\0';

  char bumpfile[MAX_PATH];
  bumpfile[0] = '\0';

  bool pNoiseTrue=false;

  int numFlags = 6;
  Flag flags[6] = { { (char*)"-n", STRING, true,  MAX_NAME, name    },
                    { (char*)"-t", STRING, false, MAX_PATH, texfile },
                    { (char*)"-u", STRING, false, MAX_PATH, bumpfile },
					{ (char*)"-p", BOOL,   false, sizeof(bool), &pNoiseTrue },
					{ (char*)"-po", INT, false, sizeof(int), &pOctaves},
					{ (char*)"-pp", DOUBLE, false, sizeof(double), &pPersistence}
                  };

  if (parseFlags(in, flags, numFlags) == -1)
  {
    cerr << "ERROR: Material: flag parsing failed!" << endl;
    return in;
  }

  in >> ambient >> diffuse >> specular >> emissive;

  kshine  = nextDouble(in);
  if (kshine < 0 || kshine>1) 
  {
	  cerr << "WARNING: Invalid kshine value clamped to [0,1]." << endl;
  }
  kshine = clamp(kshine,0,1);
  // now we scale kshine to range [0,128]
  kshine *= 128.0;

  ktrans = nextDouble(in);
  refind = nextDouble(in);
  if (refind<1)
  {
	  cerr << "WARNING: Invalid refractive index (refind) reset to 1." << endl;
	  refind=1;
  }

  if (in.fail())
  {
    cerr << "ERROR: Material: unknown stream failure" << endl;
    return in;
  }

  if (texfile[0] != '\0')
  {
    if (texture)
      delete texture;

    strncat(texname, "/", 2);
    strncat(texname, texfile, MAX_PATH);
    texture = new Image(texname);

    if(texture->good())
    {
      glGenTextures(1, &glTexID);
      glBindTexture(GL_TEXTURE_2D, glTexID);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      texture->glTexImage2DWrapper();
    }
    else
    {
      cerr << "WARNING: Unable to load texture for material: " << name;
      cerr << endl;
      delete texture;
      texture = NULL;
    }
  }
  if (bumpfile[0] != '\0')
  {
    if (bumpMap)
      delete bumpMap;

    strncat(bumpname, "/", 2);
    strncat(bumpname, bumpfile, MAX_PATH);
    bumpMap = new Image(bumpname);

    if(!bumpMap->good())
    {
      cerr << "WARNING: Unable to load bump map for material: " 
	   << name << endl;
      delete bumpMap;
      bumpMap = NULL;
    }
  }
  
	if (pNoiseTrue) {
		pNoise[0] = new PerlinNoise(137777893); // for now hard code seed
		pNoise[1] = new PerlinNoise(9973281);
		pNoise[2] = new PerlinNoise(399990001);
	}
	
  return in;
  }


void Material::setMatDirectory (const char* dir)
{
  memset(texname, 0, MAX_PATH);
  if (dir == NULL || dir[0] == '\0')
	  strncpy(texname, ".", 2);
  else
      strncpy(texname, dir, MAX_PATH);


  memset(bumpname, 0, MAX_PATH);
  if (dir == NULL || dir[0] == '\0')
  	  strncpy(bumpname, ".", 2);
  else
    strncpy(bumpname, dir, MAX_PATH);

}
