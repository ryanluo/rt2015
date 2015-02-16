class PerlinNoise;

#ifndef PERLIN_H
#define PERLIN_H

class PerlinNoise {

public:
	PerlinNoise(int newSeed);
	~PerlinNoise(void);

 
   double noise(double x, double y, double z);

private:
   int p[512];
   int seed;
   double fade(double t) { return t * t * t * (t * (t * 6 - 15) + 10); }
   double lerp(double t, double a, double b) { return a + t * (b - a); }
   double grad(int hash, double x, double y, double z) {
      int h = hash & 15;                      // CONVERT LO 4 BITS OF HASH CODE
      double u = h<8 ? x : y,                 // INTO 12 GRADIENT DIRECTIONS.
             v = h<4 ? y : h==12||h==14 ? x : z;
      return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
   }

 
 

};

#endif // PERLIN_H