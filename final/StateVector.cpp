/********************************************************************

	StateVector.cpp	Source File

	State Vector related, Methods, and Procedures
		Yitian Li  Oct 22, 2016
		Clemson University

	Copyright (C) - Yitian Li, 2016

*********************************************************************/

#include "StateVector.h"

using namespace std;


StateVector::StateVector(Vector3d X,Quaternion Q,Vector3d p,Vector3d l):x(X),q(Q),P(p),L(l){
}
StateVector::StateVector(){
  Vector3d t(0,0,0);
  x=t;
  q=Quaternion(t);
  P=t;
  L=t;
}
//Copy Constructor
StateVector::StateVector(const StateVector& V){
  x=V.x;
  q=V.q;
  P=V.P;
  L=V.L;
}


// Destructor
StateVector::~StateVector(){

}



// Print a Vector to the standard output device.
void StateVector::print() const
{
  //cout << "[" << N << "]" << endl;
}

//  Addition of 2 Vectors.
StateVector operator+(const StateVector& v1, const StateVector& v2)
{

  StateVector r;

  r.x=v1.x+v2.x;
  r.q=v1.q+v2.q;
  r.P=v1.P+v2.P;
  r.L=v1.L+v2.L;

  return r;
}

//  Subtract two Vectors.
StateVector operator-(const StateVector& v1, const StateVector& v2)
{
  StateVector r;

  r.x=v1.x-v2.x;
  r.q=v1.q-v2.q;
  r.P=v1.P-v2.P;
  r.L=v1.L-v2.L;

  return r;
}

// Product of vector and scalar
StateVector operator*(const StateVector& v, double s)
{
  StateVector r;  

  r.x=v.x*s;
  r.q=v.q*s;
  r.P=v.P*s;
  r.L=v.L*s;

  return r;
}

StateVector operator*(double s, const StateVector& v)
{
  StateVector r;  

  r.x=v.x*s;
  r.q=v.q*s;
  r.P=v.P*s;
  r.L=v.L*s;

  return r;
}


// assign one vector to another
const StateVector& StateVector::operator=(const StateVector& v2){

  x=v2.x;
  q=v2.q;
  P=v2.P;
  L=v2.L;

  return *this;
}

ostream& operator<< (ostream& os, const StateVector& v){

  os << "{x="<<v.x<<",q="<<v.q<<",P="<<v.P<<",L="<<v.L<<"}"<<endl;

  // os << "[" << v.N << "]" << endl;
  // for(int i=0;i<v.N;i++){
  //   os << v.Array[i] << " " << v.Array[i+v.N] << endl;
  // }
  return os;
}

