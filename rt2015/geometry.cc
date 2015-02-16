#ifndef GEOMETRY_ICC
#define GEOMETRY_ICC

#include "geometry.h"
#include <math.h>
//#include <GLUT/gl.h>
#include "parse.h"

template <typename T, int N>
istream& operator >> (istream& stream, Tuple<T, N>& inputTuple)
{
  for (int i = 0; i < N; ++i)
    inputTuple[i] = (T) nextDouble(stream);
  return stream;
}

template <typename T, int N>
ostream& operator << (ostream& stream, const Tuple<T, N>& outputTuple)
{
  if (N == 0)
    return stream;
  
  for (int i = 0; i < N - 1; ++i)
    stream << outputTuple[i] << " ";
  
  stream << outputTuple[N - 1];
  return stream;
}


template <typename T, int N>
Tuple<T, N>::Tuple ()
{
  for (int i = 0; i < N; ++i)
    data[i] = 0;
}

template <typename T, int N>
Tuple<T, N>::Tuple (const Tuple<T, N>& toCopy)
{
  for (int i = 0; i < N; ++i)
    data[i] = toCopy[i];
}

template <typename T, int N>
Tuple<T, N>::~Tuple ()
{

}

template <typename T, int N>
Tuple<T, N>& Tuple<T, N>::operator = (const Tuple<T, N>& toCopy)
{
  if (this == &toCopy)
    return *this;

  for (int i = 0; i < N; ++i)
    data[i] = toCopy[i];

  return *this;
}


template <typename T, int N>
T& Tuple<T, N>::operator [] (int index)
{
  return data[index];
}

template <typename T, int N>
const T& Tuple<T, N>::operator [] (int index) const
{
  return data[index];
}

template <typename T, int N>
T& Tuple<T, N>::get(int index) const
{
  return data[index];
}

template <typename T, int N>
Point<T, N>::Point (T t1, T t2, T t3, T t4)
: Tuple<T, N>()
{
  if (N >= 4)
    this->data[3] = t4;
  if (N >= 3)
    this->data[2] = t3;
  if (N >= 2)
    this->data[1] = t2;
  if (N >= 1)
    this->data[0] = t1;
}

template <typename T, int N>
Point<T, N>::~Point ()
{
}

template <typename T, int N>
Point<T, N>::Point (const Point<T, N>& toCopy)
: Tuple<T, N>(toCopy)
{
}

template <typename T, int N>
Point<T, N>& Point<T, N>::operator = (const Point<T, N>& toCopy)
{
  if (this == &toCopy)
    return *this;

  Tuple<T, N>::operator= (toCopy);

  return *this;
}

template <typename T, int N>
bool Point<T, N>::operator == (const Point<T, N>& compare)
{
  for (int i = 0; i < N; ++i)
    if (this->data[i] != compare[i])
      return false;
  
  return true;
}

template <typename T, int N>
bool Point<T, N>::operator != (const Point<T, N>& compare)
{
  for (int i = 0; i < N; ++i)
    if (this->data[i] != compare[i])
      return true;

  return false;
}

template <typename T, int N>
Point<T, N> Point<T, N>::operator + (const Vector<T, N>& toAdd)
{
  Point sum = *this;
  sum += toAdd;
  return sum;
}

template <typename T, int N>
Point<T, N>& Point<T, N>::operator += (const Vector<T, N>& toAdd)
{
  for (int i = 0; i < N; ++i)
    this->data[i] += toAdd[i];
  return *this;
}

#ifndef WIN32
template <typename T, int N>
Point<T, N> operator + (const Vector<T, N>& v, const Point<T, N>& p)
{
  return p + v;
}
#endif

template <typename T, int N>
Point<T, N> Point<T, N>::operator - (const Vector<T, N>& toSub)
{
  Point diff = *this;
  diff -= toSub;
  return diff;
}

template <typename T, int N>
Point<T, N>& Point<T, N>::operator -= (const Vector<T, N>& toSub)
{
  for (int i = 0; i < N; ++i)
    this->data[i] -= toSub[i];
  return *this;
}

#ifndef WIN32
template <typename T, int N>
Point<T, N> operator - (const Vector<T, N>& v, const Point<T, N>& p)
{
  Point<T, N> diff;
  for (int i = 0; i < N; ++i)
    diff[i] = v[i] - p[i];
  return diff;
}
#endif

template <typename T, int N>
void Point<T, N>::glLoad()
{
  switch(N)
    {
    case 2:
      glVertex2f(this->data[0], this->data[1]);
      break;
    case 3:
      glVertex3f(this->data[0], this->data[1], this->data[2]);
      break;
    case 4:
      glVertex4f(this->data[0], this->data[1], this->data[2], this->data[3]);
      break;
    default:
      cerr << "glVertex* doesn't support " << N << " arguments\n" << endl;
    }
}

template <typename T, int N>
Vector<T, N>::Vector (T t1, T t2, T t3, T t4)
: Tuple<T, N>()
{
  if (N >= 4)
    this->data[3] = t4;
  if (N >= 3)
    this->data[2] = t3;
  if (N >= 2)
    this->data[1] = t2;
  if (N >= 1)
    this->data[0] = t1;
}

template <typename T, int N>
Vector<T, N>::~Vector ()
{
}

template <typename T, int N>
Vector<T, N>::Vector (const Vector<T, N>& toCopy)
: Tuple<T, N>(toCopy)
{
}

template <typename T, int N>
Vector<T, N>::Vector(const Point<T, N>& dest)
: Tuple<T, N>(dest)
{
}

template <typename T, int N>
Vector<T, N>::Vector(const Point<T, N>& start, const Point<T, N>& end)
: Tuple<T, N>()
{
  for (int i = 0; i < N; ++i)
    this->data[i] = end[i] - start[i];
}

template <typename T, int N>
Vector<T, N>& Vector<T, N>::operator = (const Point<T, N>& dest)
{
  Tuple<T, N>::operator= (dest);

  return *this;
}

template <typename T, int N>
Vector<T, N>& Vector<T, N>::operator = (const Vector<T, N>& toCopy)
{
  if (this == &toCopy)
    return *this;

  Tuple<T, N>::operator= (toCopy);

  return *this;
}

template <typename T, int N>
Vector<T, N> Vector<T, N>::operator + (const Vector<T, N>& toAdd) const
{
  Vector<T, N> sum = *this;
  sum += toAdd;
  return sum;
}

template <typename T, int N>
Vector<T, N>& Vector<T, N>::operator += (const Vector<T, N>& toAdd)
{
  for (int i = 0; i < N; ++i)
    this->data[i] += toAdd[i];
  return *this;
}

template <typename T, int N>
Vector<T, N> Vector<T, N>::operator - () const
{
  Vector<T, N> neg = *this;
  for (int i = 0; i < N; ++i)
    neg[i] *= -1;
  return neg;
}

template <typename T, int N>
Vector<T, N> Vector<T, N>::operator - (const Vector<T, N>& toSub) const
{
  Vector<T, N> diff = *this;
  diff -= toSub;
  return diff;
}

template <typename T, int N>
Vector<T, N>& Vector<T, N>::operator -= (const Vector<T, N>& toSub)
{
  for (int i = 0; i < N; ++i)
    this->data[i] -= toSub[i];
  return *this;
}

template <typename T, int N>
Vector<T, N> Vector<T, N>::operator * (const Vector<T, N>& toMult) const
{
  Vector<T, N> product = *this;
  product *= toMult;
  return product;
}

template <typename T, int N>
Vector<T, N>& Vector<T, N>::operator *= (const Vector<T, N>& toMult)
{  
  for (int i = 0; i < N; ++i)
    this->data[i] *= toMult[i];
  return *this;
}

template <typename T, int N>
Vector<T, N> Vector<T, N>::operator / (const Vector<T, N>& toDiv) const
{
  Vector<T, N> quotient = *this;
  quotient /= toDiv;
  return quotient;
}

template <typename T, int N>
Vector<T, N>& Vector<T, N>::operator /= (const Vector<T, N>& toDiv)
{
  for (int i = 0; i < N; ++i)
    this->data[i] /= toDiv[i];
  return *this;
}

template <typename T, int N>
Vector<T, N> Vector<T, N>::operator * (double scaleFactor) const
{
  Vector<T, N> product = *this;
  product *= scaleFactor;
  return product;
}

template <typename T, int N>
Vector<T, N>& Vector<T, N>::operator *= (double scaleFactor)
{
  for (int i = 0; i < N; ++i)
    this->data[i] *= scaleFactor;
  return *this;
}

#ifndef WIN32
template <typename T, int N>
Vector<T, N> operator * (double scaleFactor, const Vector<T, N>& v)
{
  return v * scaleFactor;
}
#endif

template <typename T, int N>
Vector<T, N> Vector<T, N>::operator / (double scaleFactor) const
{
  Vector<T, N> quotient = *this;
  quotient /= scaleFactor;
  return quotient;
}

template <typename T, int N>
Vector<T, N>& Vector<T, N>::operator /= (double scaleFactor)
{
  if (scaleFactor == 0)
    {
      cerr << "cannot divide by zero " << endl;
      return *this;
    }
  for (int i = 0; i < N; ++i)
    this->data[i] /= scaleFactor;
  return *this;
}

template <typename T, int N>
T Vector<T, N>::dot (const Vector<T, N>& toDot)
{
  T dotProduct = 0;
  for (int i = 0; i < N; ++i)
    dotProduct += this->data[i] * toDot[i];
  return dotProduct;
}

/*
 * Take the cross product of two 3-dimensional vectors. Note that if called 
 * on anything but 3-dimensional vectors a compiler error will result.
 */
template <typename T, int N>
Vector<T, N> Vector<T, N>::cross (const Vector<T, N>& toCross)
{
  Vector<T, 3> crossProduct;
  crossProduct[0] = this->data[1] * toCross[2] - this->data[2] * toCross[1];
  crossProduct[1] = this->data[2] * toCross[0] - this->data[0] * toCross[2];
  crossProduct[2] = this->data[0] * toCross[1] - this->data[1] * toCross[0];
  return crossProduct;
}

template <typename T, int N>
Vector<T, N>& Vector<T, N>::normalize ()
{
  double len = this->length();

  if (len == 0)
    {
      cerr << "Cannot normalize 0-length vector " << endl;
      return *this;
    }

  for (int i = 0; i < N; ++i)
    this->data[i] /= len;
  
  return *this;
}

template <typename T, int N>
Vector<T, N> Vector<T, N>::getUnit ()
{
Vector<T, N> unit;
  unit[0]=this->data[0];
  unit[1]=this->data[1];
  unit[2]=this->data[2];
  if (unit.length() == 0)
  {
	cerr << "Warning:  cannot normalize zero length vector.";
	return unit;
	}
  return unit.normalize();
}

template <typename T, int N>
double Vector<T, N>::length()
{
  double sumSquares = 0;
  for (int i = 0; i < N; ++i)
    sumSquares += this->data[i] * this->data[i];

  return sqrt(sumSquares);
}

template <typename T, int N>
void Vector<T, N>::glLoad ()
{
  if (N == 3)
    glNormal3f(this->data[0], this->data[1], this->data[2]);
  else
    cerr << "glNormal* doesn't support " << N << " arguments\n" << endl;
}

template <typename T, int N>
Color<T, N>::Color (T t1, T t2, T t3, T t4)
: Tuple<T, N>()
{
  if (N >= 4)
    this->data[3] = t4;
  if (N >= 3)
    this->data[2] = t3;
  if (N >= 2)
    this->data[1] = t2;
  if (N >= 1)
    this->data[0] = t1;
}

template <typename T, int N>
Color<T, N>::~Color ()
{
}

template <typename T, int N>
Color<T, N>::Color (const Color& toCopy)
: Tuple<T, N>(toCopy)
{
}

template <typename T, int N>
Color<T, N>& Color<T, N>::operator = (const Color& toCopy)
{
  if (this == &toCopy)
    return *this;

  Tuple<T, N>::operator= (toCopy);

  return *this;
}

template <typename T, int N>
Color<T, N> Color<T, N>::operator + (const Color<T, N>& toAdd) const
{
  Color<T, N> sum = *this;
  sum += toAdd;
  return sum;
}

template <typename T, int N>
Color<T, N>& Color<T, N>::operator += (const Color<T, N>& toAdd)
{  
  for (int i = 0; i < N; ++i)
    this->data[i] += toAdd[i];
  return *this;
}

template <typename T, int N>
Color<T, N> Color<T, N>::operator - (const Color<T, N>& toSub) const
{
  Color<T, N> diff = *this;
  diff -= toSub;
  return diff;
}

template <typename T, int N>
Color<T, N>& Color<T, N>::operator -= (const Color<T, N>& toSub)
{  
  for (int i = 0; i < N; ++i)
    this->data[i] -= toSub[i];
  return *this;
}

template <typename T, int N>
Color<T, N> Color<T, N>::operator * (const Color<T, N>& toMult) const
{
  Color<T, N> product = *this;
  product *= toMult;
  return product;
}

template <typename T, int N>
Color<T, N>& Color<T, N>::operator *= (const Color<T, N>& toMult)
{  
  for (int i = 0; i < N; ++i)
    this->data[i] *= toMult[i];
  return *this;
}

template <typename T, int N>
Color<T, N> Color<T, N>::operator / (double scaleFactor) const
{
  Color<T, N> product = *this;
  product /= scaleFactor;
  return product;
}

template <typename T, int N>
Color<T, N>& Color<T, N>::operator /= (double scaleFactor)
{
  if (scaleFactor == 0)
    {
      cerr << "cannot divide by 0" << endl;
      return *this;
    }
  for (int i = 0; i < N; ++i)
    this->data[i] /= scaleFactor;
  return *this;
}


template <typename T, int N>
Color<T, N> Color<T, N>::operator * (double scaleFactor) const
{
  Color<T, N> product = *this;
  product *= scaleFactor;
  return product;
}

template <typename T, int N>
Color<T, N>& Color<T, N>::operator *= (double scaleFactor)
{
  for (int i = 0; i < N; ++i)
    this->data[i] *= scaleFactor;
  return *this;
}

#ifndef WIN32
template <typename T, int N>
Color<T, N> operator * (double scaleFactor, const Color<T, N>& c)
{
  return c * scaleFactor;
}
#endif

template <typename T, int N>
bool Color<T, N>::operator == (const Color<T, N>& compare)
{
  for (int i = 0; i < N; ++i)
    if (this->data[i] != compare[i])
      return false;
  
  return true;
}

template <typename T, int N>
bool Color<T, N>::operator != (const Color<T, N>& compare)
{
  for (int i = 0; i < N; ++i)
    if (this->data[i] != compare[i])
      return true;

  return false;
}

template <typename T, int N>
double Color<T, N>::intensity () const
{
  double sum = 0;
  for (int i = 0; i < N; ++i)
    sum += this->data[i] * this->data[i];

  return (sum / (float) N);
}

template <typename T, int N>
void Color<T, N>::glLoad ()
{
  if (N == 3)
    glColor3f(this->data[0], this->data[1], this->data[2]);
  else
    cerr << "glColor* doesn't support " << N << " arguments\n" << endl;
}

template <typename T, int N>
void Color<T, N>::clampTo(double min, double max)
{
  for (int i = 0; i < N; ++i)
    {
      if (this->data[i] < min)
	this->data[i] = min;
      else if (this->data[i] > max)
	this->data[i] = max;
    }  
}


template <typename T, int N>
TexCoord<T, N>::TexCoord (T t1, T t2, T t3, T t4)
: Tuple<T, N>()
{
  if (N >= 4)
    this->data[3] = t4;
  if (N >= 3)
    this->data[2] = t3;
  if (N >= 2)
    this->data[1] = t2;
  if (N >= 1)
    this->data[0] = t1;
}

template <typename T, int N>
TexCoord<T, N>::~TexCoord ()
{
}

template <typename T, int N>
TexCoord<T, N>::TexCoord (const TexCoord& toCopy)
: Tuple<T, N>(toCopy)
{
}

template <typename T, int N>
TexCoord<T, N>& TexCoord<T, N>::operator = (const TexCoord& toCopy)
{
  if (this == &toCopy)
    return *this;

  Tuple<T, N>::operator= (toCopy);

  return *this;
}

template <typename T, int N>
bool TexCoord<T, N>::operator == (const TexCoord<T, N>& compare)
{
  for (int i = 0; i < N; ++i)
    if (this->data[i] != compare[i])
      return false;
  
  return true;
}

template <typename T, int N>
bool TexCoord<T, N>::operator != (const TexCoord<T, N>& compare)
{
  for (int i = 0; i < N; ++i)
    if (this->data[i] != compare[i])
      return true;

  return false;
}

template <typename T, int N>
TexCoord<T, N> TexCoord<T, N>::operator + (const TexCoord<T, N>& toAdd) const
{
  TexCoord<T, N> sum = *this;
  sum += toAdd;
  return sum;
}

template <typename T, int N>
TexCoord<T, N>& TexCoord<T, N>::operator += (const TexCoord<T, N>& toAdd)
{  
  for (int i = 0; i < N; ++i)
    this->data[i] += toAdd[i];
  return *this;
}

template <typename T, int N>
TexCoord<T, N> TexCoord<T, N>::operator - (const TexCoord<T, N>& toSub) const
{
  TexCoord<T, N> diff = *this;
  diff -= toSub;
  return diff;
}

template <typename T, int N>
TexCoord<T, N>& TexCoord<T, N>::operator -= (const TexCoord<T, N>& toSub)
{  
  for (int i = 0; i < N; ++i)
    this->data[i] -= toSub[i];
  return *this;
}

template <typename T, int N>
TexCoord<T, N> TexCoord<T, N>::operator * (const TexCoord<T, N>& toMult) const
{
  TexCoord<T, N> product = *this;
  product *= toMult;
  return product;
}

template <typename T, int N>
TexCoord<T, N>& TexCoord<T, N>::operator *= (const TexCoord<T, N>& toMult)
{  
  for (int i = 0; i < N; ++i)
    this->data[i] *= toMult[i];
  return *this;
}

template <typename T, int N>
TexCoord<T, N> TexCoord<T, N>::operator * (double scaleFactor) const
{
  TexCoord<T, N> product = *this;
  product *= scaleFactor;
  return product;
}

template <typename T, int N>
TexCoord<T, N>& TexCoord<T, N>::operator *= (double scaleFactor)
{
  for (int i = 0; i < N; ++i)
    this->data[i] *= scaleFactor;
  return *this;
}

#ifndef WIN32
template <typename T, int N>
TexCoord<T, N> operator * (double scaleFactor, const TexCoord<T, N>& t)
{
  return t * scaleFactor;
}
#endif

template <typename T, int N>
void TexCoord<T, N>::glLoad ()
{
  switch (N)
    {
    case 1:
      glTexCoord1f(this->data[0]);
      break;
    case 2:
      glTexCoord2f(this->data[0], this->data[1]);
      break;
    case 3:
      glTexCoord3f(this->data[0], this->data[1], this->data[2]);
      break;
    case 4:
      glTexCoord4f(this->data[0], this->data[1], this->data[2], this->data[3]);
      break;
    default:
      cerr << "glTexCoord* doesn't support " << N << " arguments\n" << endl;
    }
}


template <typename T>
istream& operator >> (istream& in,  Ray<T>& r)
{
  Vector3d pos;
  Vector3d dir;
  in >> pos >> dir;
  r.setPos(pos);
  r.setDir(dir);
  return in;
}

template <typename T>
ostream& operator << (ostream& out, const Ray<T>& r)
{
  out << r.getPos() << endl 
      << r.getDir() << endl;

  return out;
}


template <typename T>
Ray<T>::Ray ()
{
}

template <typename T>
Ray<T>::Ray (const Point<T, 3>& position_, const Vector<T, 3>& direction_)
  :position(position_),
   direction(direction_)
{
  direction.normalize();
}

template <typename T>
Ray<T>::Ray (const Ray& toCopy)
  :position(toCopy.position),
   direction(toCopy.direction)
{
  direction.normalize();
}

template <typename T>
Point<T, 3> Ray<T>::translate(double alpha)
{
  Point<T, 3> result = position + (direction * alpha);
  return result;
}

template <typename T>
void Ray<T>::setPos (Point<T, 3> newPos)
{
  position = newPos;
}

template <typename T>
Point<T, 3> Ray<T>::getPos () const
{
  return position;
}

template <typename T>
void Ray<T>::setDir (Vector<T, 3> newDir)
{
  direction = newDir;
  direction.normalize();
}

template <typename T>
Vector<T, 3> Ray<T>::getDir () const
{
  return direction;
}

template <typename T>
istream& operator >> (istream& in,  Matrix<T>& m)
{
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
      m(i,j) = (T) nextDouble(in);

  return in;
}

template <typename T>
ostream& operator << (ostream& out, const Matrix<T>& m)
{
  for (int i = 0; i < 4; ++i)
  {
    for (int j = 0; j < 4; ++j)
      out << m(i,j) << " ";
    out << endl;
  }

  return out;
}


template <typename T>
Matrix<T>::Matrix ()
{
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
      get(i,j) = 0;
}

template <typename T>
Matrix<T>::Matrix (const Matrix<T>& toCopy)
{
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
      get(i,j) = toCopy(i, j);
}

template <typename T>
Matrix<T>& Matrix<T>::operator = (const Matrix<T>& toCopy)
{
  if (this == &toCopy)
    return *this;

  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
      get(i,j) = toCopy(i, j);

  return *this;
}

template <typename T>
T const * Matrix<T>::ptr () const
{
  return (T const *) data;
}

template <typename T>
T& Matrix<T>::operator () (int i, int j)
{
  return get(i,j);
}

template <typename T>
T Matrix<T>::operator () (int i, int j) const
{
  return get(i,j);
}

template <typename T>
T& Matrix<T>::get (int i, int j)
{
  return data[j][i];
}

template <typename T>
T Matrix<T>::get (int i, int j) const
{
  return data[j][i];
}

template <typename T>
Matrix<T> Matrix<T>::operator * (const Matrix<T>& toMult) const
{
  Matrix product = *this;
  product *= toMult;
  return product;
}

template <typename T>
Matrix<T>& Matrix<T>::operator *= (const Matrix<T>& toMult)
{
  Matrix product;
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
      for (int k = 0; k < 4; ++k)
        product(i,j) += get(k,j) * toMult(i,k);
  *this = product;
  return *this;
}

template <typename T>
Matrix<T> Matrix<T>::leftMult (const Matrix& toMult) const
{
  return (*this) * toMult;
}

template <typename T>
Matrix<T> Matrix<T>::rightMult (const Matrix& toMult) const
{
  return toMult * (*this);
}

template <typename T>
Point<T, 3> Matrix<T>::multPos (const Point<T, 3>& toMult) const
{
  Point<T, 3> result;
  Point<T, 4> product;
  for (int i = 0; i < 3; ++i)
    {
      product[i] = get(i,3);
      for (int j = 0; j < 3; ++j)
	product[i] += get(i,j) * toMult[j];
    }

  for(int i = 0; i < 3; ++i)
    result[i] = product[i];
  return result;
}

template <typename T>
Point<T, 4> Matrix<T>::operator * (const Point<T, 4>& toMult) const
{
  Point<T, 4> product;
  
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
      product[i] += get(i,j) * toMult[j];
  
  return product;
}

template <typename T>
Vector<T, 3> Matrix<T>::multDir (const Vector<T, 3>& toMult) const
{
  Vector<T, 3> product;
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      product[i] += get(i,j) * toMult[j];

  return product;
}

template <typename T>
Vector<T, 4> Matrix<T>::operator * (const Vector<T, 4>& toMult) const
{
  Vector<T, 4> product;
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
      product[i] += get(i,j) * toMult[j];

  return product;
}

template <typename T>
Ray<T> Matrix<T>::operator * (const Ray<T>& toMult) const
{
  Ray<T> product;
  product.setPos(multPos(toMult.getPos()));
  product.setDir(multDir(toMult.getDir()));
  return product;  
}


template <typename T>
Matrix<T> Matrix<T>::transpose () const
{
  Matrix trans;
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
      trans(i,j) = get(j,i);

  return trans;
}

template <typename T>
Matrix<T> Matrix<T>::invert () const
{
  Matrix<T> toIdent = *this;
  Matrix<T> toInv = IdentityMatrix();

 /* 
   * Using elementary row operations on the matrix to be inverted, 
   * transform it into an upper-triangular matrix where all entries on the 
   * diagonal are 1. Then, again using elementary row operations, convert this
   * to the identity matrix. Perform symmetric row operations on an identity
   * matrix for all of these, and the identity matrix is converted to the
   * inverse of the original. 
   */

  for (int i = 0; i < 4; ++i)
    {
      int pivotRow = i;
      while(toIdent(pivotRow,i) == 0)
	{
	  ++pivotRow;
	  if(pivotRow >= 4)
	    {
	      cerr << *this << "is not an invertible matrix." << endl;
	      return IdentityMatrix();
	    }
	}

      /* if necessary, switch row i with the pivot row*/
      if (pivotRow != i)
	{
	  for (int j = 0; j < 4; ++j)
	    {
	      T toIdentTemp = toIdent(pivotRow, j);
	      toIdent(pivotRow, j) = toIdent(i, j);
	      toIdent(i, j) = toIdentTemp;

	      T toInvTemp = toInv(pivotRow, j);
	      toInv(pivotRow, j) = toInv(i, j);
	      toInv(i, j) = toInvTemp;
	    }
	}
      
      double rowScale = toIdent (i, i);
      if (rowScale != 1)
	{
	  for (int j = 0; j < 4; ++j)
	    {
	      toIdent(i, j) /= rowScale;
	      toInv(i, j) /= rowScale; 
	    }
	}

      for (int j = i + 1; j < 4; ++j)
	{
	  double pivotColVal = toIdent(j, i); 
	  for (int k = 0; k < 4; ++k)
	    {
	      toIdent(j, k) -= pivotColVal * toIdent(i, k);
	      toInv(j, k) -= pivotColVal * toInv(i, k);
	    }
	}
    }

  for (int i = 3; i > 0; --i)
    {
      for (int j = i - 1; j >= 0; --j)
	{
	  double subVal = toIdent(j, i);
	  toIdent(j, i) = 0;

	  for (int k = 0; k < 4; ++k)
	    toInv(j, k) -= subVal * toInv(i, k);
	  
	}
    }

  return toInv;
}


template <typename T>
double Matrix<T>::det ()
{
  double determinant = 1;

  /* 
   * Using elementary row operations on the matrix, transform it into an 
   * upper-triangular matrix where all entries on the diagonal are 1. 
   * Scale the determinant appropriately for each row operation:
   * 
   * If we switch two rows, multiply the determinant by -1
   * If we divide a row by k, multiply the determinant by k
   */

  Matrix<T> uTri = *this;
  
  /*
   * For column i, find a pivot row with a nonzero entry in that column.
   * Switch the pivot row with row i, divide the row moved to row i by the 
   * value it has in column i, and subtract multiples of this row from all 
   * lower rows so that each has a 0 in column i.
   */
  for (int i = 0; i < 4; ++i)
    {
      int pivotRow = i;
      while(uTri(pivotRow,i) == 0)
	{
	  ++pivotRow;
	  if(pivotRow >= 4)
	    return 0;
	}

      /* if necessary, switch row i with the pivot row*/
      if (pivotRow != i)
	{
	  determinant *= -1;
	  for (int j = 0; j < 4; ++j)
	    {
	      T temp = uTri(pivotRow, j);
	      uTri(pivotRow, j) = uTri(i, j);
	      uTri(i, j) = temp;
	    }
	}

      /* scale row such that uTri[i][i] = 1 */
      double rowScale = uTri (i, i);
      determinant *= rowScale;
      for (int j = i; j < 4; ++j)
	uTri(i, j) /= rowScale;

      /* 
       * subtract copies of uTri[i] from lower rows such that uTri[j][i] = 0
       * for j > i
       */
       for (int j = i + 1; j < 4; ++j)
	{
	  double pivotColVal = uTri(j, i); 
	  for (int k = i; k < 4; ++k)
	    uTri (j, k) -= pivotColVal * uTri(i, k);
	}
    }
  return determinant; 
}

template <typename T>
Matrix<T> Matrix<T>::IdentityMatrix()
{
  Matrix ident;
  for (int i = 0; i < 4; ++i)
    ident(i, i) = 1;
  
  return ident;
}

#endif // GEOMETRY_ICC
