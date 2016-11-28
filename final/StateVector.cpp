/********************************************************************

	StateVector.cpp	Source File

	State Vector related, Methods, and Procedures
		Yitian Li  Oct 22, 2016
		Clemson University

	Copyright (C) - Yitian Li, 2016

*********************************************************************/

#include "StateVector.h"

using namespace std;


StateVector::StateVector(Vector3d X,double Mass,Vector3d v,Vector3d p,Vector3d l,Quaternion Q,Matrix3x3 I0,double btime, double ltime)
                      :x(X),m(Mass),velocity(v),P(p),L(l),q(Q),I(I0),Borntime(btime),lifetime(ltime){
}
StateVector::StateVector(){
  Vector3d t(0,0,0);
  double Mass=1;
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
}
StateVector::StateVector(double Mass, Matrix3x3 I0),m(Mass),I(I0){
  Vector3d t(0,0,0);
  Vector3d P0(-2,8,-2);
  Vector3d V0(0,0,0);
  x=P0;
  velocity=V0;
  P=V0;
  L=V0;
  q=Quaternion(V0);
  
}

//Copy Constructor
StateVector::StateVector(const StateVector& V){
  m=V.m;
  x=V.x;
  P=V.P;
  L=V.L;
  q=V.q;
  I0=V.I0;
  velocity=V.velocity;
}


// Destructor
StateVector::~StateVector(){

}



// Print a Vector to the standard output device.
void StateVector::print() const
{
  cout << "[" << N << "]" << endl;
}

//  Addition of 2 Vectors.
StateVector operator+(const StateVector& v1, const StateVector& v2)
{

  StateVector r();

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
  StateVector r();
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
  StateVector r();  
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
  StateVector r();  
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

