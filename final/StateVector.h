/********************************************************************

  StateVector.h Header File

  State Vector related, Methods, and Procedures
    Yitian Li  Oct 22, 2016
    Clemson University

  Copyright (C) - Yitian Li, 2016


*********************************************************************/

#ifndef _H_StateVector
#define _H_StateVector

#include "Vector.h"
#include "Quaternion.h"
#include <vector>

using namespace std;

/* StateVector Descriptions and Operations */

class StateVector {
public:
  StateVector();
  StateVector(double Mass, Matrix3x3 I0);
  StateVector::StateVector(Vector3d X,double Mass,Vector3d v,Vector3d p,Vector3d l,Quaternion Q,Matrix3x3 I0,double btime, double ltime);

  StateVector(const StateVector& V);
  ~StateVector();

  void print() const;
  Vector3d x;
  Vector3d velocity;
  Vector3d P;
  Vector3d L;
  Quaternion q;
  Matrix3x3 I0;
  double m;
  double Borntime;
  double lifetime;

  friend StateVector operator+(const StateVector& v1, const StateVector& v2);//addition 
  friend StateVector operator-(const StateVector& v1, const StateVector& v2);//subtract
  friend StateVector operator*(const StateVector& v, double s); // scalar mult
  friend StateVector operator*(double s, const StateVector& v);
  const StateVector& operator=(const StateVector& v2);
  friend ostream& operator<< (ostream& os, const StateVector& v);
};

#endif
