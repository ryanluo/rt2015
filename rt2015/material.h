class Material;


#ifndef MATERIAL_H
#define MATERIAL_H


#include "common.h"
#include "geometry.h"
#include "rayfile.h"
#include "perlin.h"

class Image;


/*
* class to encapsulate properties for materials specification.
* ambient, diffuse, specular are all colors representing responses
* to types of light, while emissive is light added from the material.
* kshine is the shininess of the material (a la OpenGL), ktrans is the
* transparency (0.0 is opaque, 1.0 is transparent), and refind is
* the refractive index of the material (1.0 is air)
*/
class Material
{
public:
	Material ();
	~Material ();

	// accessors
	const Color3d&	getAmbient  ()  { return ambient;  }
	//const Color3d&	getDiffuse  ()  { return diffuse;  }
	const Color3d&	getSpecular ()  { return specular; }
	const Color3d&	getEmissive ()  { return emissive; }

	double		getKshine    ()  { return kshine;    }
	double		getKtrans   ()  { return ktrans;   }
	double		getRefind   ()  { return refind;   }
	const char*	getName     ()  { return name;     }
	bool		textured ();
	bool		bumpMapped();
	bool		procTextured();

	const Color3d  getDiffuse (Intersection& info);
	const Color3d getTexture(double u, double v);
	const Color3d getProceduralTexture(Point3d point);



	//Vector3d	bumpNormal (double u,double v);
	void		bumpNormal (Vector3d& normal, 
		Vector3d& up,
		Vector3d& right,
		Intersection& info,
		double bumpScale);



	// OpenGL calls  
	void		glLoad ();
	void		glUnload ();

	// read and write .ray directives
	istream&	read  (istream& in);
	ostream&	write (ostream& out);

	// set relative dir for material
	void		setMatDirectory (const char* dir);

private:

	char		name[MAX_NAME];

	Color3d		ambient;
	Color3d		diffuse;
	Color3d		specular;
	Color3d		emissive;

	double		kshine;
	double		ktrans;
	double		refind;

	char		texname[MAX_PATH];
	Image*		texture;
	GLuint		glTexID;


	char		bumpname[MAX_PATH];
	Image*		bumpMap;



	PerlinNoise*	pNoise[3];
	int				pOctaves;
	double			pPersistence;

	long seeds[3];

};


istream& operator>> (istream& in,  Material& m);
ostream& operator<< (ostream& out, Material& m);


#endif // MATERIAL_H
