/********************************************************************

	StateVector.cpp	Source File

	State Vector related, Methods, and Procedures
		Yitian Li  Oct 22, 2016
		Clemson University

	Copyright (C) - Yitian Li, 2016

*********************************************************************/

#include "StateVector.h"

using namespace std;
StateVector::StateVector()
{

}

StateVector::StateVector(int n, std::vector<Vector3d> x,std::vector<Vector3d> v):N(n),Array(v){
  for(int i=0;i<n;i++){
    Array.push_back(v[i]);
  }
}
StateVector::StateVector(int n):N(n){
  Array.reserve(4*N);
  Vector3d t(0,0,0);
  for(int i=0;i<4*N;i++){
    Array.push_back(t);
  }
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
  if(i < 0 || i > 4*N){
    cerr << "2D vector index bounds error :" <<i<< endl;
    exit(1);
  }
  return Array[i];
}


int StateVector::getn() const{
  return N;
}

void StateVector::setn(int n) {
  N=n;
}

int StateVector::getArraysize() const{
  return Array.size();
}
void StateVector::add(double x, double y, double z){
  Vector3d t(x,y,z);
  Array.push_back(t);
}
//  Set the components of a Vector according to the arguments
void StateVector::set(std::vector<Vector3d> x,std::vector<Vector3d> v)
{
  Array.clear();
  N=x.size();
  Array.reserve(N*2);
  for(int i=0;i<N;i++)
  {
    Array.push_back(x[i]);
  }
  for(int i=0;i<N;i++){
    Array.push_back(v[i]);
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
    Array.reserve(4*N);
  }
    double Mass=7;
  Matrix3x3 BigI(Mass/12*32,0,0,0,Mass/12*32,0,0,0,Mass/12*32);
    Vector3d P0(-2,8,-2);
    Vector3d V0(0,0,0);
    x=P0;
    m=Mass;
    velocity=V0;
    P=V0;
    L=V0;
    q=Quaternion(V0);
    I0= BigI;
    //cout<<"reset size"<<endl;
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

  r.x=v1.x+v2.x;
  r.P=v1.P+v2.P;
  r.L=v1.L+v2.L;
  r.q=v1.q+v2.q;
  r.I0=v1.I0+v2.I0;
  r.velocity=v1.velocity+v2.velocity;
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

  r.x=v1.x-v2.x;
  r.P=v1.P-v2.P;
  r.L=v1.L-v2.L;
  r.q=v1.q-v2.q;
  r.I0=v1.I0-v2.I0;
  r.velocity=v1.velocity-v2.velocity;

  return r;
}

// Product of vector and scalar
StateVector operator*(const StateVector& v, double s)
{
  StateVector r(v.getn());  
  for(int i=0;i<2*r.N;i++){
    r.Array[i]=v.Array[i]*s;
  }

  r.x=v.x*s;
  r.P=v.P*s;
  r.L=v.L*s;
  r.q=v.q*s;
  r.I0=v.I0*s;
  r.velocity=v.velocity*s;

  return r;
}

StateVector operator*(double s, const StateVector& v)
{
  StateVector r(v.getn());  
  for(int i=0;i<2*r.N;i++){
    r.Array[i]=v.Array[i]*s;
  }
  r.x=v.x*s;
  r.P=v.P*s;
  r.L=v.L*s;
  r.q=v.q*s;
  r.I0=v.I0*s;
  r.velocity=v.velocity*s;
  
  return r;
}




// assign one vector to another
const StateVector& StateVector::operator=(const StateVector& v2){
  int i;

  if(N != v2.N){
    Array.clear();
    setsize(v2.N);
    for(int i = 0; i < 4*N; i++)
      Array.push_back(v2.Array[i]);
  }
  else{
    for(int i = 0; i < 4*N; i++)
      Array[i] = v2.Array[i];
  }

  x=v2.x;
  P=v2.P;
  L=v2.L;
  q=v2.q;
  I0=v2.I0;
  velocity=v2.velocity;

  return *this;
}

ostream& operator<< (ostream& os, const StateVector& v){

  os << "{x=" <<v.x<<",v="<<v.velocity<<",P="<<v.P<<",L="<<v.L<<",q="<<v.q<<"}"<<endl;

  // os << "[" << v.N << "]" << endl;
  // for(int i=0;i<v.N;i++){
  //   os << v.Array[i] << " " << v.Array[i+v.N] << endl;
  // }
  return os;
}

