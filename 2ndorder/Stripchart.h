/*
   OpenGL object to represent a stripchart recorder

   VIZA 659            Donald H. House         3/7/00
*/

#include "Graphics.h"

#define MAXVALUES   100000

class Stripchart{
protected:
  double x0, y0;    // center position of recorder
  double size;      // size of bounding square around recorder
  double minval, maxval;// max/min values that can be displayed

  double history[MAXVALUES]; // array of recorded values
  double deltat;    // recorder time step between samples
  int ntime;        // current recorder time in increments of deltat
  int nvalues;      // total number of values stored in recorder
  int stopflag;     // true if recorder full

public:
  Stripchart();
  Stripchart(double dt, double minv, double maxv);
  Stripchart(double x, double y, double s, double dt, 
         double minv, double maxv);

  void setposition(double x, double y);
  void setsize(double s);
  void setminmax(double minv, double maxv);
  void setdeltat(double dt);
  
  void reset();
  void settime(double t);
  void incrtime(int n);
  void incrtime(double dt);
    
  void setvalue(double v);
  double value();

  void draw();
};
