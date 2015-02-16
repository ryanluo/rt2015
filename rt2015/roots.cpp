#include "roots.h"


// constants

#define ROOT_EPSILON         1e-10

// macros

#define isZero(x)	((fabs(x) < ROOT_EPSILON) ? true : false)


int solveQuadratic (double coeffs[3], double results[2])
{
  int n;
  double discriminant = sqr(coeffs[1]) - 4*coeffs[0]*coeffs[2];

  if (discriminant < 0)
  {
      n = 0;
  }
  else if (discriminant == 0)
  {
      n = 1;
      results[0] = -coeffs[1] / 2 / coeffs[0];
  }
  else
  {
      n = 2;
      results[0] = (-coeffs[1] + sqrt(discriminant)) / 2 / coeffs[0];
      results[1] = (-coeffs[1] - sqrt(discriminant)) / 2 / coeffs[0];
  }

  return n;
}

int solveCubic (double coeffs[4], double results[3])
{
  /* done using Cardano's formula. see mathworld.wolfram.com for details */
  int n = 0;

  if (coeffs[0] == 0)
    {
      double qCoeffs[3];
      for(int i = 0; i < 3; ++i)
	qCoeffs[i] = coeffs[i+1];
      return solveQuadratic(qCoeffs, results);
    }
  /* x^3 + a2 * x^2 + a1 * x + a0 = 0 */
  double a2 = coeffs[1] / coeffs[0];
  double a1 = coeffs[2] / coeffs[0];
  double a0 = coeffs[3] / coeffs[0];

  /*
   * some neat substitution here. let z = x - a2/3 and we can do a little
   * algebraic manipulation to get rid of the z^2 term. Cardano's formula
   * gives us the rest.
   */
  double Q = a1 / 3.0 - sqr(a2) / 9.0;
  double R = (27 * a0 + 2 * cube(a2) - 9 * a2 * a1) / 54.0; 

  double D = Q * sqr(Q) + sqr(R); 

  if (isZero(D))
    {
      if (isZero(R))
	{
	  results[0] = 0;
	  n = 1;
	}
      else
	{
	  double u = cbrt(-R);      
	  results[0] = 2 * u;
	  results[1] = -u;
	  n = 2;
	}
    }
  else if (D > 0)
    {
      double Dsqrt = sqrt(D);
      results[0] = cbrt(Dsqrt - R) - cbrt(Dsqrt + R);
      n = 1;
    }
  else
    {
      double theta = acos(-R / sqrt(-cube(Q))) / 3.0;
      double t = 2 * sqrt(-Q);
      results[0] = t * cos(theta);
      results[1] = -t * cos(theta + M_PI / 3);
      results[2] = -t * cos(theta - M_PI / 3);
      n = 3;
    }

  /* 
   * Correct for the fact that earlier algebraic manipulation substituted
   * z - a2/3 for x (the calculations that gave the formulas for Q and R)
   */

  double toSub = a2 / 3.0;
  for (int i = 0; i < n; ++i)
    results[i] -= toSub;

  return n;

}

int solveQuartic (double coeffs[5], double results[4])
{
  /*
   * Starting with the equation:
   *   
   * x^4 + ax^3 + bx^2 + cx + d = 0
   *   
   * yields the "resolvent cubic equation"
   *
   * y^3 - by^2 + (ac - 4d)y - a^2d + 4bd - c^2 = 0
   *   
   * Let y be any root of this equation and define R as:
   *
   * R = sqrt[(a^2)/4 - b + y]
   *
   * If R is *not* zero, then
   *
   * D = sqrt[(3a^2)/4 - R^2 - 2b + (4ab - 8c - a^3)/(4R)]
   *
   * E = sqrt[(3a^2)/4 - R^2 - 2b - (4ab - 8c - a^3)/(4R)]
   *
   * If R *is* zero, then
   *
   * D = sqrt[(3a^2)/4 - 2b + 2*sqrt(y^2 - 4d)]
   *
   * E = sqrt[(3a^2)/4 - 2b - 2*sqrt(y^2 - 4d)]
   *
   * The four roots of the original equation are:
   *
   * x = -a/4 - R/2 - E/2
   * x = -a/4 - R/2 + E/2
   * x = -a/4 + R/2 - D/2
   * x = -a/4 + R/2 + D/2
   */

  int n = 0;
  bool solValid[4];
  for (int i = 0; i < 4; ++i)
    solValid[i] = false;

  double a = coeffs[1] / coeffs[0];
  double b = coeffs[2] / coeffs[0];
  double c = coeffs[3] / coeffs[0];
  double d = coeffs[4] / coeffs[0];

  double cubicCoeffs[4];
  double cubicSols[3];
  cubicCoeffs[0] = 1;
  cubicCoeffs[1] = -b;
  cubicCoeffs[2] = a * c - 4 * d;
  cubicCoeffs[3] = -sqr(a) * d + 4 * b * d - sqr(c);
  
  int cubSols = solveCubic(cubicCoeffs, cubicSols);

  if (cubSols == 0)
    return 0;

  double D, E, R;
  double y = cubicSols[0];
  double Rsqr = sqr(a) / 4 - b + y;

  if (Rsqr < 0)
    {
      /*
      if (((y == (sqr(a) / 4.0)) && 
	   (4 * a * b - 8 * c - a * sqr(a) == 0)))
	cerr << "complex part in roots cancels, not handled yet" << endl;
      */
      return 0;
    }
  else if (!isZero(Rsqr))
    {
      R = sqrt(Rsqr);
      double Dsqr = 3 * sqr(a) / 4 - Rsqr - 2 * b + 
	(4 * a * b - 8 * c - cube(a)) / (4 * R);
      double Esqr = 3 * sqr(a) / 4 - Rsqr - 2 * b - 
	(4 * a * b - 8 * c - cube(a)) / (4 * R);
      if (Dsqr >= 0)
	{
	  D = sqrt(Dsqr);
	  solValid[0] = true;
	  solValid[1] = true;
	  n += 2;
	  results[0] = -a / 4.0 + R / 2.0 - D / 2.0;
	  results[1] = -a / 4.0 + R / 2.0 + D / 2.0;
	}
      if (Esqr >= 0)
	{
	  E = sqrt(Esqr);
	  solValid[2] = true;
	  solValid[3] = true;
	  n += 2;
	  results[2] = -a / 4.0 - R / 2.0 - E / 2.0;
	  results[3] = -a / 4.0 - R / 2.0 + E / 2.0;
	}
    }
  else
    {
      double tmp = sqr(y) - 4.0 * d;
      if (tmp < 0)
	return 0;

      double Dsqr = 3 * sqr(a) / 4.0 - 2 * b + 2 * tmp;
      double Esqr = 3 * sqr(a) / 4.0 - 2 * b - 2 * tmp;
      if (Dsqr >= 0)
	{
	  D = sqrt(Dsqr);
	  solValid[0] = true;
	  solValid[1] = true;
	  n += 2;
	  results[0] = -a / 4.0 - D / 2.0;
	  results[1] = -a / 4.0 + D / 2.0;
	}
      if (Esqr >= 0)
	{
	  E = sqrt(Esqr);
	  solValid[2] = true;
	  solValid[3] = true;
	  n += 2;
	  results[2] = a / -4.0 - E / 2.0;
	  results[3] = a / -4.0 + E / 2.0;
	}
    }

  /* pack the solution array */
  int currSol = 0;
  for (int i = 0; i < n; ++i, ++currSol)
    {
      while (!solValid[currSol])
	{
	  currSol++;
	}
      results[i] = results[currSol];
    }
  return n;
}
