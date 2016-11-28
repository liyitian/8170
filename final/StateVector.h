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
protected:
  int N;
public:
  StateVector();
  StateVector(int n);
  StateVector(int n, vector<Vector3d> x,vector<Vector3d> v);

  StateVector(const StateVector& V);
  ~StateVector();

   Vector3d& operator[](int i) ;
  int getn() const; 
  void setn(int n);


  void set(vector<Vector3d> x,vector<Vector3d> v);
  void set(const StateVector &t);
  void add(double x, double y, double z);
  void setsize(int vN);
  void print() const;
  vector<Vector3d> x;
  vector<Vector3d> velocity;
  vector<Vector3d> P;
  vector<Vector3d> L;
  vector<Quaternion> q;
  vector<Matrix3x3> I0;
  vector<double> m;

  friend StateVector operator+(const StateVector& v1, const StateVector& v2);//addition 
  friend StateVector operator-(const StateVector& v1, const StateVector& v2);//subtract
  friend StateVector operator*(const StateVector& v, double s); // scalar mult
  friend StateVector operator*(double s, const StateVector& v);
  const StateVector& operator=(const StateVector& v2);
  friend ostream& operator<< (ostream& os, const StateVector& v);
};



#endif
