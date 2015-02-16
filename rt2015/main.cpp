#include "main.h"

#include "control.h"
#include "rayfile.h"
#include "image.h"

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <fstream>


Options* options = NULL;	// rendering options
RayFile* scene   = NULL;	// current scene
Image*   image   = NULL;	// last raytraced image

bool drawPolys   = true;	// use OpenGL to display polygons or not


int main (int argc, char** argv)
{
  if (argc > 1)
  {
    if ((strncmp(argv[1], "-H", 2) == 0) ||
        (strncmp(argv[1], "--help", 6) == 0))
    {
      help();
      return 0;
    }
    else if ((strncmp(argv[1], "-I", 2) == 0) || 
             (strncmp(argv[1], "--noninteractive", 16) == 0))
    {
      return nonInteractiveMode(argc-1, argv+1);
    }
  }

  // set up the window
  glutInit(&argc, &argv[0]); 
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowPosition(100,100);
  glutInitWindowSize(100, 100);
  glutCreateWindow("hmc cs155 ray tracer");
 
  // register call back functions
  glutDisplayFunc(display);
  glutReshapeFunc(unreshape);

  // initialize parameters
  init(argc, argv);

  // wait for something to happen
  glutMainLoop();

  return 0;
}


void init (int argc, char** argv)
{
  glClearColor(0.0,0.0,0.0,0.0);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_NORMALIZE);
  glDisable(GL_CULL_FACE);

  // two-sided lighting, use both sides of the polgyons
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,      GL_TRUE);

  // use infinite viewer for specular lighting - looks good, speedy
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,  GL_FALSE);

#ifdef GL_VERSION_1_2
  // only separate specular light if we're drawing textures.  otherwise, 
  // lump all the colors together with this option
  glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SINGLE_COLOR);
#endif

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  // init random number generator
  srand48(time(0));

  initOptions();

  // parse the command line options
  parseArgs(argc, argv);

  // create our scene object
  scene = new RayFile();

  if (options->source[0] != '\0')
  {
    // read in the file specified on the command line
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
  }

  // create the image to raytrace to
  image = new Image(options->width, options->height);

  // setup main menu
  make_menu();

  // register keyboard callback function
  glutKeyboardFunc(keyboard_func);
  glutMouseFunc(mouse_click_func);
  glutMotionFunc(mouse_move_func);
}


#define ARG_ERROR(e)	{ cerr << e << endl; exit(-1); }

void parseArgs (int argc, char** argv)
{
  int i = 1;
  while (i < argc)
  {
    if (argv[i][0] != '-')  usage();

    switch (argv[i][1])
    {
    case 'z':				// [ -z width height ]
      if (++i == argc) usage();
      options->width = atoi(argv[i]);
      if (options->width < 1)
        ARG_ERROR("width must be greater than 0!");

      if (++i == argc) usage();
      options->height = atoi(argv[i]);
      if (options->height < 1)
        ARG_ERROR("height must be greater than 0!");
      break;

    case 'r':				// [ -r recursive limit ]
      if (++i == argc) usage();
      options->recursiveDepth = atoi(argv[i]);
      if (options->recursiveDepth < 0)
        ARG_ERROR("recursive limit just be non-negative!");
      break;

    case 'c':				// [ -c contribution limit ]
      if (++i == argc) usage();
      options->contribution = atof(argv[i]);
      if (options->contribution < 0.0)
        ARG_ERROR("contribution limit must be non-negative!");
      break;

	case 'b':				// [ -b ]
		options->bumpMaps=true;
		break;

    case 'j':				// [ -j jitter rays ]
      if (++i == argc) usage();
      options->jitter = atoi(argv[i]);
      break;

    case 'h':				// [ -h shadow rays ]
      if (++i == argc) usage();
      options->shadow = atoi(argv[i]);
      break;

    case 's':				// [ -s source rayfile ]
      if (++i == argc) usage();
      strncpy(options->source, argv[i], MAX_PATH);
      break;

    case 'd':				// [ -d destination image ]
      if (++i == argc) usage();
      strncpy(options->destination, argv[i], MAX_PATH);
      break;

    case 'Q':                           // [ -Q ] (quiet mode)
      options->quiet = true;
      break;

    case '-':
      if (strncmp(&argv[i][2], "quiet", 6) == 0)
      {
        options->quiet = true;
        break;
      }

    default:
      cerr << "ERROR: unrecognized flag: -" << argv[i] << endl;
      usage();
    }

    i++;
  }
}


void usage ()
{
  cerr << "usage: ./rt [ -H | --help ]"			<< endl
       << endl
       << "    interactive-mode" 			<< endl
       << "       ./rt [ -s source ]" 			<< endl
       << "            [ -z width height ]" 		<< endl
       << "            [ -r recursive depth ]" 		<< endl
       << "            [ -c contribution limit ]" 	<< endl
       << "            [ -j jitter rays ]" 		<< endl
       << "            [ -h shadow rays ]" 		<< endl
       << endl
       << "    non-interactive-mode" 			<< endl
       << "       ./rt [ -I | --noninteractive" 	<< endl 
       << "            [ -Q | --quiet ]"		<< endl
       << "            -s source" 			<< endl
       << "            -d destination" 			<< endl
       << "            [ -z width height ]" 		<< endl
       << "            [ -r recursive depth ]" 		<< endl
       << "            [ -c contribution limit ]" 	<< endl
       << "            [ -j jitter rays ]" 		<< endl
       << "            [ -h shadow rays ]" 		<< endl
       << endl;

  exit(-1);
}


void display ()
{
  // check if there have been any openGL problems
//  GLenum errCode = glGetError();
//  if (errCode != GL_NO_ERROR) 
//  {
//      const GLubyte* errString = gluErrorString(errCode);
//      cerr << "OpenGL error: " << errString << endl;
//  }

  // clear the frame buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (drawPolys)
  {
    // don't need to set up projection and modelview matrices,
    // because scene->glDraw() does it

    // draw polygons cause they're quick and interactive
    scene->glDraw();
  }
  else
  {
    // setup orthographic projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, options->width, 0.0, options->height);

    // default mode should be modelview
    glMatrixMode(GL_MODELVIEW);   
    glLoadIdentity();  

    // draw the image
    image->glDrawPixelsWrapper();
  }

  // swap buffers
  glutSwapBuffers();  
}


void unreshape (int width, int height)
{
  // don't allow user to manually resize the window
  reshape(options->width, options->height);
}


void reshape (int width, int height) 
{
  // delete the old image and make a new one if we're actually
  // resizing.  otherwise don't so we can save the image still
  if (max(width,64)  != options->width  || 
      max(height,64) != options->height)
  {
    if (image) delete image;
    image = new Image(max(width, 64), max(height,64));
    drawPolys = true;
  }

  // set window height and width
  options->width  = max(width,  64);
  options->height = max(height, 64); 

  // change the actual window's size
  glutReshapeWindow(options->width, options->height);

  // the lower left corner of the viewport is 0,0
  // the upper right corner is width, height
  glViewport(0, 0, (GLint) options->width, (GLint) options->height);
}


int nonInteractiveMode (int argc, char** argv)
{
  // init random number generator
  srand48(time(0));

  time_t start = time(NULL);

  initOptions();

  // parse the command line options
  parseArgs(argc, argv);
  options->progressive = false;

  if (!options->quiet)
    cerr << *options << endl;

  // create our scene object
  scene = new RayFile();

  if (options->source[0] == '\0')
  {
    cerr << "ERROR: non-interactive mode requires -s flag" << endl;
    usage();
    return -1;
  }

  if (options->destination[0] == '\0')
  {
    cerr << "ERROR: non-interactive mode requires -d flag" << endl;
    usage();
    return -1;
  }

  // read in the rayfile
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
    cerr << "ERROR: couldn't open file " << options->source << endl;
    return -1;
  }

  // create the image to raytrace to
  image = new Image(options->width, options->height);

  // raytrace the scene into image
  scene->raytrace(image);

  // and write image out to the destination
  if (image->write(options->destination) == -1)
  {
    cerr << "ERROR: couldn't save image to " << options->destination << endl;
    return -1;
  }

  time_t finish = time(NULL);
  if (!options->quiet)
    cerr << "elapsed time " << (finish - start) << "s" << endl;

  return 0;
}



void help ()
{
  usage();
}


void initOptions ()
{
  // create the object to store our options in
  options = new Options();

  // set our default values
  options->width        = 300;
  options->height       = 300;
  options->jitter       = 0;
  options->shadow       = 0;
  options->recursiveDepth    = 0;
  options->contribution = 0;
  options->lighting     = true;
  options->transforms   = true;
  options->textures     = true;
  options->transparency = true;
  options->backface     = false;
  options->progressive  = true;

  memset(options->source,      0, MAX_PATH);
  memset(options->destination, 0, MAX_PATH);

  options->quiet = false;
}


ostream& operator<< (ostream& out, Options& o)
{ 
  out << "              source: " << options->source            << endl
      << "         destination: " << options->destination       << endl
      << "          image size: " << options->width << " x "
                                  << options->height            << endl
      //<< "         jitter rays: " << options->jitter            << endl
      //<< "         shadow rays: " << options->shadow            << endl
      << "     recursive depth: " << options->recursiveDepth         << endl
      << "  contribution limit: " << options->contribution      << endl;

  return out;
} 
