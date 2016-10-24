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
StateVector::StateVector(int n):N(n){
  Array.reserve(2*N);
}

//Copy Constructor
StateVector::StateVector(const StateVector& V){
  set(V);
}


// Destructor
StateVector::~StateVector(){
  Array.clear();
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

const Vector3d& StateVector::operator[](int i) const
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

// Set size of generic Vector
void StateVector::setsize(int vN){
  if(vN < 0){
    cerr << "vector dimension of " << vN << " invalid" << endl;
    exit(1);    
  }

  N = vN;
  if(N == 0)
    Array.clear();
  else{
    Array.clear();
    Array.reserve(2*N);
  }

  int i;
  for(int i = 0; i < 2*N; i++)
    Array[i] = 0;
}

// Print a Vector to the standard output device.
void StateVector::print() const
{
  cout << "[" << N << "]" << endl;
  for(int i=0;i<N;i++){
    cout<< Array[i] << " " << Array[i+N] << endl;
  }
}

//  Addition of 2 Vectors.
StateVector operator+(const StateVector& v1, const StateVector& v2)
{
  
  if (v1.getn()!=v2.getn()){
    cout<< "Size not Match!\n" <<endl;
    exit(1);
  }

  StateVector r(v1.getn());
  
  for(int i=0;i<2*r.N;i++){
    r.Array[i]=v1.Array[i]+v2.Array[i];
  }

  return r;
}

//  Subtract two Vectors.
StateVector operator-(const StateVector& v1, const StateVector& v2)
{
  
  if (v1.getn()!=v2.getn()){
    cout<< "Size not Match!\n" <<endl;
    exit(1);
  }

  StateVector r(v1.getn());
  
  for(int i=0;i<2*r.N;i++){
    r.Array[i]=v1.Array[i]-v2.Array[i];
  }

  return r;
}

// Product of vector and scalar
StateVector operator*(const StateVector& v, double s)
{
  StateVector r(v.getn());  
  for(int i=0;i<2*r.N;i++){
    r.Array[i]=v.Array[i]*s;
  }
  return r;
}

StateVector operator*(double s, const StateVector& v)
{
  StateVector r(v.getn());  
  for(int i=0;i<2*r.N;i++){
    r.Array[i]=v.Array[i]*s;
  }
  return r;
}




// assign one vector to another
const StateVector& StateVector::operator=(const StateVector& v2){
  int i;

  if(N != v2.N){
    Array.clear();
    setsize(v2.N);
  }
  for(int i = 0; i < 2*N; i++)
    Array[i] = v2.Array[i];

  return *this;
}

ostream& operator<< (ostream& os, const StateVector& v){
  os << "[" << v.N << "]" << endl;
  for(int i=0;i<v.N;i++){
    os << v.Array[i] << " " << v.Array[i+v.N] << endl;
  }
  return os;
}

