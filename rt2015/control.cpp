#include "control.h"
#include "main.h"
#include "rayfile.h"
#include "image.h"
#include "camera.h"

#include <fstream>
#include <time.h>


enum {
	M_QUIT = 0,
	M_HELP,

	M_FILE_OPEN,
	M_FILE_RELOAD,
	M_FILE_SAVE_IMAGE,
	M_FILE_SAVE_RAYFILE,

	M_RENDER_RAYTRACE,
	M_RENDER_JITTER,
	M_RENDER_SHADOWS,
	M_RENDER_RDEPTH,
	M_RENDER_CLIMIT,
	M_RENDER_SET_SIZE,

	M_OPTIONS_LIGHTING,
	M_OPTIONS_TRANSFORMS,
	M_OPTIONS_TEXTURES,
	M_OPTIONS_BUMPMAPS,
	M_OPTIONS_TRANSPARENCY,
	M_OPTIONS_BACKFACE,

	M_PRINT_CONFIG,

	M_LAST_ENUM
};


int  mouse_state  = GLUT_UP;
int  mouse_button = -1;
int  old_x        = -1;
int  old_y        = -1;
bool shiftKey     = false;


int make_menu ()
{
  int file = glutCreateMenu(menu_func);
  glutAddMenuEntry( "Open...",			M_FILE_OPEN);
  glutAddMenuEntry( "Reload",			M_FILE_RELOAD);
  glutAddMenuEntry( "Save Image...",		M_FILE_SAVE_IMAGE);
  glutAddMenuEntry( "Save RayFile...",		M_FILE_SAVE_RAYFILE);

  int render = glutCreateMenu(menu_func);
  glutAddMenuEntry( "Raytrace",			M_RENDER_RAYTRACE);
  glutAddMenuEntry( "Image Size...",		M_RENDER_SET_SIZE);
  glutAddMenuEntry( "Jittering...",		M_RENDER_JITTER);
  glutAddMenuEntry( "Shadows Rays...",		M_RENDER_SHADOWS);
  glutAddMenuEntry( "Recursive Depth...",	M_RENDER_RDEPTH);
  glutAddMenuEntry( "Contribution Limit...",	M_RENDER_CLIMIT);

  int options = glutCreateMenu(menu_func);
  glutAddMenuEntry( "Toggle Lighting",		M_OPTIONS_LIGHTING);
  glutAddMenuEntry( "Toggle Transforms",	M_OPTIONS_TRANSFORMS);
  glutAddMenuEntry( "Toggle Textures",		M_OPTIONS_TEXTURES);
    glutAddMenuEntry( "Toggle Bump Maps",		M_OPTIONS_BUMPMAPS);
  glutAddMenuEntry( "Toggle Transparency",	M_OPTIONS_TRANSPARENCY);
  glutAddMenuEntry( "Toggle Backface Culling",	M_OPTIONS_BACKFACE);

  int main = glutCreateMenu(menu_func);
  glutAddSubMenu  ( "File",			file);
  glutAddSubMenu  ( "Render",			render);
  glutAddSubMenu  ( "Options",			options);
  glutAddMenuEntry( "Print Options",		M_PRINT_CONFIG);
  glutAddMenuEntry( "Help",			M_HELP);
  glutAddMenuEntry( "Quit",			M_QUIT);

  glutAttachMenu(GLUT_RIGHT_BUTTON);

  return main;
}


void menu_func (int value)
{
  switch (value)
  {
  case M_QUIT:  
    exit(0);
    break;

  case M_HELP:
    menu_help();
    break;

  case M_PRINT_CONFIG:
    cerr << *options << endl;
    break;

  case M_FILE_OPEN:
    menu_file_open();
    break;

  case M_FILE_RELOAD:
    menu_file_reload();
    break;

  case M_FILE_SAVE_IMAGE:
    menu_file_save_image();
    break;

  case M_FILE_SAVE_RAYFILE:
    menu_file_save_rayfile();
    break;

  case M_RENDER_RAYTRACE:
    menu_render_raytrace();
    break;

  case M_RENDER_JITTER:
    menu_render_jitter();
    break;

  case M_RENDER_SHADOWS:
    menu_render_shadows();
    break;

  case M_RENDER_RDEPTH:
    menu_render_rdepth();
    break;

  case M_RENDER_CLIMIT:
    menu_render_climit();
    break;

  case M_RENDER_SET_SIZE:
    menu_render_set_size();
    break;

  case M_OPTIONS_LIGHTING:
    options->lighting = !options->lighting;
    cerr << "Lights " << (options->lighting?"on":"off") << endl;
    glutPostRedisplay();
    break;

  case M_OPTIONS_TRANSFORMS:
    options->transforms = !options->transforms;
    cerr << "Transforms " << (options->transforms?"on":"off") << endl;
    glutPostRedisplay();
    break;

  case M_OPTIONS_TEXTURES:
    options->textures = !options->textures;
    cerr << "Textures " << (options->textures?"on":"off") << endl;
    glutPostRedisplay();
    break;


  case M_OPTIONS_BUMPMAPS:
    options->bumpMaps = !options->bumpMaps;
    cerr << "Bump maps " << (options->bumpMaps?"on":"off") << endl;
    glutPostRedisplay();
    break;

  case M_OPTIONS_TRANSPARENCY:
    options->transparency = !options->transparency;
    cerr << "Transparency " << (options->transparency?"on":"off") << endl;
    glutPostRedisplay();
    break;

  case M_OPTIONS_BACKFACE:
    options->backface = !options->backface;
    cerr << "Backface Culling " << (options->backface?"on":"off") << endl;
    glutPostRedisplay();
    break;

  default:
    break;
  }

  cerr << "done!" << endl;
}


void keyboard_func (unsigned char key, int x, int y)
{
  switch (key)
  {
  case 'Q':
  case 'q':
    exit(0);
    break;

  default:
    break;
  }
}


void mouse_click_func (int button, int state, int x, int y)
{
  // mouse was up and is being pushed down
  if (state == GLUT_DOWN && mouse_state == GLUT_UP)
  {
    shiftKey = (glutGetModifiers() == GLUT_ACTIVE_SHIFT);
        
    if (button == GLUT_LEFT_BUTTON)
      scene->getCamera()->lockRotate();
  }
  // mouse was down and is being released
  else if (state == GLUT_UP && mouse_state == GLUT_DOWN)
  {
    if (button == GLUT_LEFT_BUTTON)
    {
      mouse_rotate_camera(x - old_x, y - old_y);
      scene->getCamera()->unlockRotate();

      glutPostRedisplay();
    }
    else if (button == GLUT_MIDDLE_BUTTON)
    {
      mouse_translate_camera(x - old_x, y - old_y);
      glutPostRedisplay();
    }   
  }
  // ignore mouse clicks when one button is already down
  else
  {
    return;
  }

  mouse_state  = state;
  mouse_button = button;
  old_x        = x;
  old_y        = y;
}
        
  
void mouse_move_func (int x, int y)
{
  if (mouse_button == GLUT_LEFT_BUTTON)
  {
    drawPolys = true;
    mouse_rotate_camera(x - old_x, y - old_y);
    glutPostRedisplay();
  }
  else if (mouse_button == GLUT_MIDDLE_BUTTON)
  {
    drawPolys = true;
    mouse_translate_camera(x - old_x, y - old_y);
    glutPostRedisplay();
  }   

  old_x = x;
  old_y = y;
}


void menu_help ()
{
  cerr << "hmc cs155 ray tracer\n\
\n\
action                result\n\
------                ------\n\
left mouse            rotate view left-right and up-down\n\
shift-left mouse      rotate view around view direction\n\
middle mouse          translate left-right and up-down\n\
shift-middle mouse    translate along view direction\n\
right mouse           access pop-up menu\n\
q key                 quit\n\
\n\
See the rt manual for a more detailed explaination." 
      << endl;
}


void menu_file_open ()
{
  // reset the globals
  scene->clear();
  image->clear();
  memset(options->source, 0, MAX_PATH);
  drawPolys = true;


  cerr << "Open file (string) : " << flush;
  cin.getline(options->source,MAX_PATH);
  if (!cin)
  {
    cerr << "ERROR: couldn't read filename" << endl;
    return;
  }
  // need to remove quotes from getline
  int loc=0;
  if (options->source[0]=='"')
  {
	  while (loc<MAX_PATH && options->source[loc+1]!='"')
	  {
		  options->source[loc]=options->source[loc+1];
		  loc++;
	  }
	  if (loc==MAX_PATH)
	  {
		  cerr << "ERROR: couldn't read filename" << endl;
		  return;
	  }
	  options->source[loc]=0;
  }


  // read in our new rayfile
  ifstream sourceFile(options->source);
  //ifstream sourceFile("C:/Documents and Settings/sweedyk/My Documents/cs155/rtWindows/Debug/test_box.ray");
  sourceFile.exceptions(0);
  if (sourceFile.good())
  {
    scene->setFile(options->source);
    sourceFile >> *scene;
    sourceFile.close();
  }
  else
  {
    cerr << "ERROR: couldn't open " << options->source << endl;
    memset(options->source, 0, MAX_PATH);
  }

  glutPostRedisplay();
}


void menu_file_reload ()
{
  // reset the globals
  scene->clear();
  image->clear();
  drawPolys = true;

  // reload the same rayfile
  ifstream sourceFile(options->source);
  sourceFile.exceptions(0);
  if (sourceFile.good())
  {
    scene->setFile(options->source);
    sourceFile >> *scene;
    sourceFile.close();
  }
  else
  {
    cerr << "ERROR: couldn't open " << options->source << endl;
    memset(options->source, 0, MAX_PATH);
  }

  glutPostRedisplay();
}


void menu_file_save_image ()
{
  if (drawPolys || !image)
  {
    cerr << "ERROR: no image to save" << endl;
    return;
  }

  memset(options->destination, 0, MAX_PATH); 
  cerr << "Save image as (string) : " << flush;
  cin  >> options->destination;

  if (cin.fail())
  {
    cerr << "ERROR: couldn't read filename" << endl;
    return;
  }

  // write out our image 
  if (image->write(options->destination) == -1)
  {
    cerr << "ERROR: couldn't save image to " << options->destination << endl;
  }

  memset(options->destination, 0, MAX_PATH);
}


void menu_file_save_rayfile ()
{
  if (!scene)
  {
    cerr << "ERROR: no scene to save" << endl;
    return;
  }

  memset(options->destination, 0, MAX_PATH); 
  cerr << "Save rayfile as (string) : " << flush;
  cin  >> options->destination;

  if (cin.fail())
  {
    cerr << "ERROR: couldn't read filename" << endl;
    return;
  }

  // write the scene to our destination file
  ofstream destinationFile(options->destination);
  destinationFile.exceptions(0);
  if (destinationFile.good())
  {
    destinationFile << *scene;
    destinationFile.close();
  }
  else
  {
    cerr << "ERROR: couldn't open " << options->destination << endl;
  }

  memset(options->destination, 0, MAX_PATH);
}


void menu_render_raytrace ()
{
  if (!scene)
  {
    cerr << "ERROR: no scene to raytrace" << endl;
    return;
  }

  cerr << "raytracing..." << endl;
  cerr << *options << endl;

  time_t start = time(NULL);

  // get the OpenGL version into the frame buffer
  display();
  drawPolys=false;

  // put the framebuffer into our image to start
  image->clear();
  image->glReadPixelsWrapper();

  // raytrace into image
  scene->raytrace(image);

  time_t finish = time(NULL);
  cerr << "elapsed time " << (finish - start) << "s" << endl;

  glutPostRedisplay();
}


void menu_render_jitter ()
{
  int jitter;
  cerr << "Old jitter rays : " << options->jitter << endl
       << "New jitter rays (int) : " << flush;
  cin  >> jitter;

  if (cin.fail())
  {
    cerr << "ERROR: jitter must be an integer" << endl;
    return;
  }

  options->jitter = jitter;
}


void menu_render_shadows ()
{
  int shadows;
  cerr << "Old shadow rays : " << options->shadow << endl
       << "New shadow rays (int) : " << flush;
  cin  >> shadows;

  if (cin.fail())
  {
    cerr << "ERROR: shadow rays must be an integer" << endl;
    return;
  }

  options->shadow = shadows;
}


void menu_render_rdepth ()
{
  int rdepth;
  cerr << "Old recursive depth : " << options->recursiveDepth << endl
       << "New recursive depth (int) : " << flush;
  cin  >> rdepth;

  if (cin.fail() || rdepth < 0)
  {
    cerr << "ERROR: recursive depth must be a non-negative integer" << endl;
    return;
  }

  options->recursiveDepth = rdepth;
}


void menu_render_climit ()
{
  double climit;
  cerr << "Old contribution limit : " << options->contribution << endl
       << "New contribution limit (double) : " << flush;
  cin  >> climit;

  if (cin.fail() || climit < 0)
  {
    cerr << "ERROR: contribution limit must be a non-negative double" << endl;
    return;
  }

  options->contribution = climit;
}


void menu_render_set_size ()
{
  int width, height;
  cerr << "Old size : " << options->width << " " << options->height << endl
       << "New size (int int) : " << flush;
  cin  >> width >> height;

  if (cin.fail() || width <= 0 || height <= 0)
  {
    cerr << "ERROR: width and height must be positive integers" << endl;
    return;
  }

  reshape(width, height);
}


void mouse_rotate_camera (int dx, int dy)
{     
  double scale = 0.01;
       
  if (shiftKey)
  {     
    scene->getCamera()->rotateZ(scale*dx);
  }
  else
  {
    scene->getCamera()->rotateXY(scale*dx*-1.0, scale*dy*-1.0);
  } 
}   
   
  
void mouse_translate_camera (int dx, int dy)
{   
  double scale = 0.05;
    
  if (shiftKey)
  {
    scene->getCamera()->translate(0, 0, scale*dy);
  }
  else
  {
    scene->getCamera()->translate(scale*dx, scale*dy, 0);
  }
}

