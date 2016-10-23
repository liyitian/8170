/********************************************************************

	StateVector.cpp	Source File

	State Vector related, Methods, and Procedures
		Yitian Li  Oct 22, 2016
		Clemson University

	Copyright (C) - Yitian Li, 2016

*********************************************************************/

#include "StateVector.h"

using namespace std;

StateVector::StateVector(int n, std::vector<Vector3d> x,std::vector<Vector3d> v):N(n),Array(v){
  for(int i=1;i<=n;i++){
    Array.push_back(v[i-1]);
  }
}


// Destructor
StateVector::~StateVector(){
  delete []Array;
}

//
// Array index form of a vector
// Routines returning an lvalue: i.e. X[i] returns addr of X[i]
//
Vector3d& StateVector::operator[](int i)
{
  if(i < 0 || i > 1){
    cerr << "2D vector index bounds error" << endl;
    exit(1);
  }
  return Array[i];
}

int StateVector::getn() const{
  return N;
}

//  Set the components of a Vector according to the arguments
void StateVector::set(std::vector<Vector3d> x,std::vector<Vector3d> v)
{
  Array.clear();
  N=x.size();
  Array.reserve(N*2);
  for(int i=0;i<N;i++)
  {
    Array[0]=x[i];
    Array[N+i]=v[i];
  }
}
void StateVector::set(const StateVector &t)
{
  N = t.N;
  Array = t.Array;
}

// Print a Vector to the standard output device.
void Vector2d::print() const
{
  cout << "[" << x << " " << y << "]";
}
void Vector2d::print(int w, int p) const
{
  cout << "[" << setw(w) << setprecision(p) << Round(x, p) << " ";
  cout << setw(w) << setprecision(p) << Round(y, p) << "]";
}


// Unary negation of a vector
Vector2d operator-(const Vector2d& v1){
  Vector2d r(-v1.x, -v1.y);
  return r;
}

//  Addition of 2 Vectors.
Vector2d operator+(const Vector2d& v1, const Vector2d& v2)
{
  Vector2d r;
  r.x = v1.x + v2.x;
  r.y = v1.y + v2.y;
  return r;
}

//  Subtract two Vectors.
Vector2d operator-(const Vector2d& v1, const Vector2d& v2)
{
  Vector2d r;
  r.x = v1.x - v2.x;
  r.y = v1.y - v2.y;
  return r;
}

// Product of vector and scalar
Vector2d operator*(const Vector2d& v, double s)
{
  Vector2d r;

  r.x = v.x * s;
  r.y = v.y * s;
  return r;
}
Vector2d operator*(double s, const Vector2d& v)
{
  Vector2d r;

  r.x = v.x * s;
  r.y = v.y * s;
  return r;
}

// Inner product of two Vectors
double operator*(const Vector2d& v1, const Vector2d& v2)
{
  return(v1.x * v2.x +
	 v1.y * v2.y);
}

//  Component-wise multiplication of two Vectors
Vector2d operator^(const Vector2d& v1, const Vector2d& v2)
{
  Vector2d r;
  r.x = v1.x * v2.x;
  r.y = v1.y * v2.y;
  return r;
}

// Cross product of two Vectors
Vector3d operator%(const Vector2d& v1, const Vector2d& v2)
{
  Vector3d cp;
  cp.x = 0;
  cp.y = 0;
  cp.z = v1.x * v2.y - v1.y * v2.x;
  return (cp);
}

// Divide a vector by a scalar.
Vector2d operator/(const Vector2d& v, double s)
{
  Vector2d r;
  r.x = v.x / s;
  r.y = v.y / s;
  return(r);
}

// Determine if two Vectors are identical.
short operator==(const Vector2d& one, const Vector2d& two)
{
  return((one.x == two.x) && (one.y == two.y));
}

// assign one vector to another
const Vector& Vector::operator=(const Vector& v2){
  int i;

  if(N != v2.N){
    delete []v;
    setsize(v2.N);
  }
  for(i = 0; i < N; i++)
    v[i] = v2.v[i];

  return *this;
}

ostream& operator<< (ostream& os, const Vector2d& v){
  os << "[" << v.x << " " << v.y << "]";
  return os;
}

