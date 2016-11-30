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
#include "Matrix.h"
#include <vector>

using namespace std;

/* StateVector Descriptions and Operations */

class StateVector {
public:
  StateVector();
  StateVector(Vector3d X,Quaternion Q,Vector3d p,Vector3d l);

  StateVector(const StateVector& V);
  ~StateVector();

  void print() const;
  Vector3d x;
  Quaternion q;
  Vector3d P;
  Vector3d L;

  friend StateVector operator+(const StateVector& v1, const StateVector& v2);//addition 
  friend StateVector operator-(const StateVector& v1, const StateVector& v2);//subtract
  friend StateVector operator*(const StateVector& v, double s); // scalar mult
  friend StateVector operator*(double s, const StateVector& v);
  const StateVector& operator=(const StateVector& v2);
  friend ostream& operator<< (ostream& os, const StateVector& v);
};

#endif
