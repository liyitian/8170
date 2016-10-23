/********************************************************************

  StateVector.h Header File

  State Vector related, Methods, and Procedures
    Yitian Li  Oct 22, 2016
    Clemson University

  Copyright (C) - Yitian Li, 2016


*********************************************************************/

#ifndef _H_Vector
#define _H_Vector

#include "Utility.h"
#include "Vector.h"

using namespace std;

/* StateVector Descriptions and Operations */

class StateVector;

class StateVector {
protected:
  int N;
  std::vector<Vector3d> Array;
  
public:
  StateVector(n, std::vector<Vector3d> Array);
  StateVector(const StateVector& Array);
  ~StateVector();

  set()

  double& operator[](int i);
  const double& operator[](int i) const;  
  friend StateVector operator+(const StateVector& v1, const StateVector& v2);//addition 
  friend StateVector operator-(const StateVector& v1, const StateVector& v2);//subtract
  friend StateVector operator*(const StateVector& v, double s); // scalar mult
  const StateVector& operator=(const StateVector& v2);

};



#endif
