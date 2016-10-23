/*
  Graphics.h

   Routines to draw handy OpenGL objects

   VIZA 659            Donald H. House         3/9/00
*/

#include <iostream>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include <cmath>   /* definitions for math routines */

#define PI      3.1415926536

// circle of specified radius with center at (x, y)
// kind is GL_POLYGON for filled circle or GL_LINE_LOOP for outlined
void drawCircle(double x, double y, double radius, int kind);

// arrow of unit length from the origin to the point (1, 0, 0)
void drawArrow();

// pointer of unit width with tip at the origin
void drawPointer();

// unit square with center at (x, y). type is GL_POLYGON for filled
// square or GL_LINE_LOOP for outlined square
void drawBox(double x, double y, int type);
