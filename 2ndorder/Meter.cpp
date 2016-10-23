/*
  Meter.cpp

   OpenGL object to represent a meter with a dial

   VIZA 659            Donald H. House         3/6/00
*/

#include "Meter.h"

using namespace std;

Meter::Meter(){
  setposition(0, 0);
  setsize(1);
  setminmax(-1, 1);
  setvalue(0);
}
Meter::Meter(double val, double minv, double maxv){
  setposition(0, 0);
  setsize(1);
  setminmax(minv, maxv);
  setvalue(val);

}
Meter::Meter(double val, double x, double y, double s, double minv, 
         double maxv){
  setposition(x, y);
  setsize(s);
  setminmax(minv, maxv);
  setvalue(val);
}

void Meter::setposition(double x, double y){
  x0 = x;
  y0 = y;
}
void Meter::setsize(double s){
  size = s;
}
void Meter::setminmax(double minv, double maxv){
  minval = minv;
  maxval = maxv;
}

void Meter::setvalue(double v){
  val = v;
  if(val > maxval) val = maxval;
  else if(val < minval) val = minval;
}
double Meter::value(){
  return val;
}

void Meter::draw(){
  int i;

  glPushMatrix();
    glLineWidth(2);
    glTranslatef(x0, y0, 0);
    glScalef(size / 2, size / 2, 1);
       
    glColor3f(0.4, 0.4, 0.4);
    drawCircle(0, 0, 1, GL_POLYGON);
    glColor3f(1, 1, 1);
    drawCircle(0, 0, 1, GL_LINE_LOOP);

    glPushMatrix();
      glScalef(0.9, 0.9, 1);
      glColor3f(0, 0, 0);
      drawCircle(0, 0, 1, GL_POLYGON);
      glColor3f(1, 1, 1);
      drawCircle(0, 0, 1, GL_LINE_LOOP);

      for(i = 0; i <= 10; i++){
	glPushMatrix();
	  glRotatef(240 - i * 30, 0, 0, 1);
	  if(i == 0 || i == 5 || i == 10){
	    glLineWidth(4);
	    glColor3f(0.5, 0.5, 0);
	    glBegin(GL_LINES);
	      glVertex2f(0.8, 0);
	      glVertex2f(1, 0);
	    glEnd();
	  }
	  else{
	    glLineWidth(2);
	    glColor3f(0.4, 0.4, 0);
	    glBegin(GL_LINES);
	      glVertex2f(0.9, 0);
	      glVertex2f(1, 0);
	    glEnd();
	  }
	glPopMatrix();
      }

      glPushMatrix();
        glScalef(0.95, 0.95, 1);
        glRotatef(240 - 300 * ((val - minval) / (maxval - minval)), 
		  0, 0, 1);
	glColor3f(1, 1, 1);
	drawArrow();
      glPopMatrix();

    glPopMatrix();
    glColor3f(1, 1, 1);
    drawCircle(0, 0, 1/16.0, GL_POLYGON);
    glColor3f(0, 0, 0);
    drawCircle(0, 0, 1/32.0, GL_POLYGON);

  glPopMatrix();
}
