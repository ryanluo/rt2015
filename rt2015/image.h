class Image;

#ifndef IMAGE_H
#define IMAGE_H


#include <fstream>

using namespace std;


/*
* definitions for channel values.  for example,
*    image->getPixel(0,0,GREEN);
* gets the green value at pixel 0,0 in image.
*/
#define RED	0
#define GREEN	1
#define BLUE	2
#define ALPHA	3


/*
* struct to contain an entire pixel's data, up to four channels.  if
* fewer than four channels exist in the image, then some values in
* this struct will be ignored (eg. for an RGB image, the struct
* member 'a' is ignored).  For images with more than four channels,
* this struct is not sufficient to access all the image data.
*/
typedef struct
{
	double		r;
	double		g;
	double		b;
	double		a;
} Pixel;


/*
* generic multi channel 8-bit max image class.  can read and write
* BMP, ascii PNM, and JPG file formats, and supports some useful OpenGL 
* calls.
*
* get and set pixel methods use doubles from 0.0 to 1.0.  these 
* values are mapped to integer values from 0 to the maximum value
* allowed by the number of bits per channel in the image.
*/
class Image
{
public:
	Image ();
	~Image ();

	// create empty image with specified characteristics
	Image (int width_, int height_);
	Image (int width_, int height_, int channels_);
	Image (int width_, int height_, int channels_, 
		int bits_);

	// create image and read data from filename
	// use good() or bad() to check success
	Image (const char* filename);

	// copy constructor and assignment operator
	// _deep_ copy!
	Image (const Image& image);
	Image&		operator= (const Image& image);

	// accessors
	int		getWidth ()    { return width;    }
	int		getHeight ()   { return height;   }
	int		getChannels () { return channels; }
	int		getBits ()     { return bits;     }

	// check if the image is valid
	bool		good ();
	bool		bad ();

	// set all the pixel data to zero
	void		clear ();

	// retrieve pixel data.  methods with _ at the
	// end of their name return 0.0 if the x and y
	// are out of range.  otherwise, an assertion
	// failure occurs
	double		getPixel  (int x, int y, int channel);
	double		getPixel_ (int x, int y, int channel);
	Pixel		getPixel  (int x, int y);
	Pixel		getPixel_ (int x, int y);
	Pixel&		getPixel  (int x, int y, Pixel& pixel);
	Pixel&		getPixel_ (int x, int y, Pixel& pixel);

	// retrieve sample through linear interpolation
	Pixel		getSample  (double x, double y);
	Pixel&		getSample  (double x, double y, Pixel& pixel);

	// set pixel data.  if x and y are out of range,
	// an assertion failure occurs
	void		setPixel  (int x, int y, int channel, double value);
	void		setPixel_ (int x, int y, int channel, double value);
	void		setPixel  (int x, int y, Pixel& pixel);
	void		setPixel_ (int x, int y, Pixel& pixel);

	// OpenGL call wrappers
	void		glDrawPixelsWrapper ();
	void		glTexImage2DWrapper ();
	void		glReadPixelsWrapper ();

	// top-level file read and write calls,
	// determines file type
	int		read (const char* filename);
	int		write (const char* filename);

	// BMP specific read and write calls
	int		readBMP (const char* filename);
	int		readBMP (ifstream& file);
	int		writeBMP (const char* filename);
	int		writeBMP (ofstream& file);

	// PNM specific read and write calls
	int		readPNM (const char* filename);
	int		readPNM (ifstream& file);
	int		writePNM (const char* filename);
	int		writePNM (ofstream& file);
	/*
	// JPG specific read and write calls
	int		readJPG (const char* filename);
	int		readJPG (ifstream& file);
	int		writeJPG (const char* filename);
	int		writeJPG (ofstream& file);*/

private:

	int		index(int x, int y, int c);

	int		width;
	int		height;
	int		channels;	// number of channels per pixel
	int		bits;		// number of bits per channel
	int		maxValue;	// max that can be stored in bits

	unsigned char*	pixels;		// image data

};


#endif // IMAGE_H
