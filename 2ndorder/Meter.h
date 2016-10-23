/*
  Meter.h

   OpenGL object to represent a meter with a dial

   VIZA 659            Donald H. House         3/6/00
*/

#include "Graphics.h"

class Meter{
protected:
  double x0, y0;
  double size;
  double minval, maxval;
  double val;
public:
  Meter();
  Meter(double val, double minv, double maxv);
  Meter(double val, double x, double y, double s, double minv, double maxv);

  void setposition(double x, double y);
  void setsize(double s);
  void setminmax(double minv, double maxv);

  void setvalue(double v);
  double value();

  void draw();
};
