#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <iostream>

using namespace std;

template <typename T, int N> class Tuple;
template <typename T, int N> class Point;
template <typename T, int N> class Vector;
template <typename T, int N> class Color;
template <typename T, int N> class TexCoord;
template <typename T>        class Ray;
template <typename T>        class Matrix;

typedef Point    <int, 2>    Point2i;
typedef Point    <double, 2> Point2d;
typedef Point    <float, 2>  Point2f;
typedef Point    <char, 2>   Point2c;

typedef Point    <int, 3>    Point3i;
typedef Point    <double, 3> Point3d;
typedef Point    <float, 3>  Point3f;
typedef Point    <char, 3>   Point3c;

typedef Point    <int, 4>    Point4i;
typedef Point    <double, 4> Point4d;
typedef Point    <float, 4>  Point4f;
typedef Point    <char, 4>   Point4c;

typedef Vector   <int, 2>    Vector2i;
typedef Vector   <double, 2> Vector2d;
typedef Vector   <float, 2>  Vector2f;
typedef Vector   <char, 2>   Vector2c;

typedef Vector   <int, 3>    Vector3i;
typedef Vector   <double, 3> Vector3d;
typedef Vector   <float, 3>  Vector3f;
typedef Vector   <char, 3>   Vector3c;

typedef Vector   <int, 4>    Vector4i;
typedef Vector   <double, 4> Vector4d;
typedef Vector   <float, 4>  Vector4f;
typedef Vector   <char, 4>   Vector4c;

typedef Color    <double, 3> Color3d;
typedef Color3d              Colord;

typedef TexCoord <double, 2> TexCoord2d;

typedef Ray      <double>    Rayd;

typedef Matrix   <double>    Matrixd;
typedef Matrix   <float>     Matrixf;


template <typename T, int N>
istream& operator >> (istream& stream, Tuple<T, N>& inputTuple);

template <typename T, int N>
ostream& operator << (ostream& stream, const Tuple<T, N>& outputTuple);

template <typename T, int N> 
class Tuple
{
public:
	Tuple			();
	Tuple			(const Tuple& toCopy);
	virtual ~Tuple	();
	Tuple& operator =	(const Tuple& toCopy);

	T& operator []	(int index);
	const T& operator []	(int index) const;
	T& get		(int index) const;

	virtual void glLoad	() = 0;
	int size		()   { return N; }

protected:
	T data[N];
};


template <typename T, int N> 
class Point : public Tuple<T, N>
{
public:
	Point			(T t1 = 0, T t2 = 0, T t3 = 0, T t4 = 0);
	virtual ~Point	();
	Point			(const Point& toCopy);
	Point& operator =	(const Point& toCopy);

	bool   operator ==	(const Point& toCompare);
	bool   operator !=	(const Point& toCompare);

	Point  operator +	(const Vector<T, N>& toAdd);
	Point& operator +=	(const Vector<T, N>& toAdd);
    

#ifndef WIN32
	template<typename U, int M>
	friend Point<U, M> operator + (const Vector<U, M>& v, const Point<U, M>& p);
#endif

	Point  operator -	(const Vector<T, N>& toSub);
	Point& operator -=	(const Vector<T, N>& toSub);

#ifndef WIN32
	template<typename U, int M>
	friend Point<U, M> operator - (const Vector<U, M>& v, const Point<U, M>& p);
#endif

	void glLoad ();
    

};


template <typename T, int N> 
class Vector : public Tuple<T, N>
{
public:
	Vector		(T t1 = 0, T t2 = 0, T t3 = 0, T t4 = 0);

	virtual ~Vector	();
	Vector		(const Point<T, N>& dest);
	Vector		(const Point<T, N>& start, const Point<T, N>& end);
	Vector		(const Vector& toCopy);

	Vector& operator =	(const Point<T, N>& dest); 
	Vector& operator =	(const Vector& toCopy); 

	bool    operator ==	(const Vector& toCompare) const;
	bool    operator !=	(const Vector& toCompare) const;

	Vector  operator +	(const Vector& toAdd) const;
	Vector& operator +=	(const Vector& toAdd);

	Vector  operator -    () const;
	Vector  operator -	(const Vector& toSub) const;
	Vector& operator -=	(const Vector& toSub);

	Vector  operator *	(const Vector& toMult) const;
	Vector& operator *=	(const Vector& toMult);

	Vector  operator /	(const Vector& toDiv) const;
	Vector& operator /=	(const Vector& toDiv);

	Vector  operator *	(double scaleFactor) const;
	Vector& operator *=	(double scaleFactor);

#ifndef WIN32
	template<typename U, int M>
	friend Vector<U, M> operator * (double scaleFactor, const Vector<U, M>& v);
#endif


	Vector  operator /	(double scaleFactor) const;
	Vector& operator /=	(double scaleFactor);

	T dot			(const Vector& toDot);
	Vector cross          (const Vector& toCross);

	Vector& normalize 	();
	Vector  getUnit 	();

	double length		();
	void   glLoad 	();
};


template <typename T, int N>
class Color : public Tuple <T, N>
{
public:
	Color			(T t1 = 0, T t2 = 0, T t3 = 0, T t4 = 0);
	virtual ~Color	();
	Color 		(const Color& toCopy);  
	Color& operator = 	(const Color& toCopy);

	Color  operator +	(const Color& toAdd) const;
	Color& operator +=	(const Color& toAdd);

	Color  operator -	(const Color& toSub) const;
	Color& operator -=	(const Color& toSub);

	Color  operator *	(const Color& toMult) const;
	Color& operator *=	(const Color& toMult);

	Color  operator *	(double scaleFactor) const;
	Color& operator *=	(double scaleFactor);

#ifndef WIN32
	template<typename U, int M>
	friend Color<U, M> operator * (double scaleFactor, const Color<U, M>& c);
#endif

	Color  operator /	(double scaleFactor) const;
	Color& operator /=	(double scaleFactor);

	bool operator == 	(const Color& toCompare);
	bool operator != 	(const Color& toCompare);

	double intensity	() const;
	void glLoad		();
	void clampTo		(double min, double max);
};


template <typename T, int N>
class TexCoord : public Tuple <T, N>
{
public:
	TexCoord     		(T t1 = 0, T t2 = 0, T t3 = 0, T t4 = 0);
	virtual ~TexCoord	();
	TexCoord 		(const TexCoord& toCopy);
	TexCoord& operator = 	(const TexCoord& toCopy);

	bool operator == 	(const TexCoord& toCompare);
	bool operator != 	(const TexCoord& toCompare);

	TexCoord  operator +	(const TexCoord& toAdd) const;
	TexCoord& operator +=	(const TexCoord& toAdd);

	TexCoord  operator -	(const TexCoord& toSub) const;
	TexCoord& operator -=	(const TexCoord& toSub);

	TexCoord  operator *	(const TexCoord& toMult) const;
	TexCoord& operator *=	(const TexCoord& toMult);

	TexCoord  operator *	(double scaleFactor) const;
	TexCoord& operator *=	(double scaleFactor);

#ifndef WIN32
	template<typename U, int M>
	friend TexCoord<U, M> operator * 
		(double scaleFactor, const TexCoord<U, M>& t);
#endif

	void glLoad 		();
};


template <typename T>
istream& operator >> (istream& in,  Ray<T>& r);

template <typename T>
ostream& operator << (ostream& out, const Ray<T>& r);

template <typename T>
class Ray
{
public:
	Ray			();
	Ray			(const Point<T, 3>& position_, 
		const Vector<T, 3>& direction_);
	Ray 			(const Ray<T>& toCopy);

	Point<T, 3> translate	(double alpha);

	void		setPos  (Point<T, 3> newPos);
	Point<T, 3>   getPos	() const;
	void		setDir	(Vector<T, 3> newDir);
	Vector<T, 3>  getDir	() const;

private:

	Point<T, 3>  position;
	Vector<T, 3> direction;
};


template <typename T>
istream& operator >> (istream& in,  Matrix<T>& m);

template <typename T>
ostream& operator << (ostream& out, const Matrix<T>& m);

template <typename T>
class Matrix
{
public:
	Matrix		();
	Matrix		(const Matrix& toCopy);
	Matrix& operator = 	(const Matrix& toCopy);

	T const * ptr		() const;

	T& operator () 	(int i, int j);
	T  operator () 	(int i, int j) const;
	T& get      		(int i, int j);
	T  get       		(int i, int j) const;

	Matrix  operator * 	(const Matrix& toMult) const;
	Matrix& operator *= 	(const Matrix& toMult);
	Matrix  leftMult 	(const Matrix& toMult) const;
	Matrix  rightMult 	(const Matrix& toMult) const;

	Point<T, 3>   multPos     (const Point<T, 3>& toMult) const;
	Point<T, 4>   operator *  (const Point<T, 4>& toMult) const;

	Vector<T, 3>  multDir     (const Vector<T, 3>& toMult) const;
	Vector<T, 4>  operator *  (const Vector<T, 4>& toMult) const;

	Ray<T>        operator *  (const Ray<T>& toMult) const;

	Matrix       transpose () const;
	Matrix       invert    () const;

	double       det       ();

	static Matrix<T> IdentityMatrix ();

private:

	T data[4][4];

};


#include "geometry.cc"


#endif // GEOMETRY_H
