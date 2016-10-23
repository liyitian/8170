/*
  Graphics.cpp

   Routines to draw handy OpenGL objects

   VIZA 659            Donald H. House         3/9/00
*/

#include "Graphics.h"

using namespace std;

/*
  Routine to draw a circle of specified radius with center at (x, y)
  kind is typically either GL_POLYGON for filled circle or GL_LINE_LOOP
  for outlined circle
*/
void drawCircle(double x, double y, double radius, int kind){

  static const double CIRC_INC = 2 * PI / 60;
  double theta;

  glBegin(kind);
  for(theta = 0.0; theta < 2 * PI; theta += CIRC_INC)
      glVertex2f(x + radius * cos(theta), y + radius * sin(theta));
  glEnd();
}

/*
  Routine to draw an arrow of unit length from the origin to the 
  point (1, 0, 0)
*/
void drawArrow(){
  glBegin(GL_POLYGON);
    glVertex2f(0, 0.03);
    glVertex2f(0.8, 0.03);
    glVertex2f(0.8, -0.03);
    glVertex2f(0, -0.03);
  glEnd();
  glBegin(GL_POLYGON);
    glVertex2f(0.8, 0.1);
    glVertex2f(1, 0);
    glVertex2f(0.8, -0.1);
  glEnd();
}

/*
  Routine to draw a pointer of unit width with tip at the origin
*/
void drawPointer(){
  glBegin(GL_POLYGON);
    glVertex2f(-1, 0.57735);        // (-1, 1 / sqrt(3))
    glVertex2f(0, 0);
    glVertex2f(-1, -0.57735);
  glEnd();
}

/*
  Routine to draw a unit square. type is typically either GL_POLYGON 
  for filled square or GL_LINE_LOOP for outlined square
*/
void drawBox(double x0, double y0, int type){
  glBegin(type);
    glVertex2f(x0 - 0.5, y0 - 0.5);
    glVertex2f(x0 - 0.5, y0 + 0.5);
    glVertex2f(x0 + 0.5, y0 + 0.5);
    glVertex2f(x0 + 0.5, y0 - 0.5);
  glEnd();
}

