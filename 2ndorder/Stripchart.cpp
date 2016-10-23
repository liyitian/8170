/*
   OpenGL object to represent a stripchart recorder

   VIZA 659            Donald H. House         3/7/00

*/

#include "Stripchart.h"

using namespace std;

#define round(x)    ((int)((x) > 0? (x) + 0.5 : (x) - 0.5))

#define CHARTWIDTH  11
#define CHARTHEIGHT 4
#define BORDER      0.5

Stripchart::Stripchart(){
  reset();
  setposition(0, 0);
  setsize(12);
  setdeltat(0.1);
  setminmax(-1, 1);
}
Stripchart::Stripchart(double dt, double minv, double maxv){
  setminmax(minv, maxv);
  reset();
  setposition(0, 0);
  setsize(12);
  setdeltat(dt);
}
Stripchart::Stripchart(double x, double y, double s, double dt, 
               double minv, double maxv){
  setminmax(minv, maxv);
  reset();
  setposition(x, y);
  setsize(s);
  setdeltat(dt);
}

void Stripchart::setposition(double x, double y){
  x0 = x;
  y0 = y;
}
void Stripchart::setsize(double s){
  size = s;
}
void Stripchart::setminmax(double minv, double maxv){
  minval = minv;
  maxval = maxv;
}

void Stripchart::setdeltat(double dt){
  deltat = dt;
}

void Stripchart::reset(){
  ntime = -1;
  nvalues = 0;
  stopflag = 0;
}

void Stripchart::settime(double t){
  ntime = round(t / deltat);
  if(ntime >= nvalues)
    ntime = nvalues;
  else if(ntime < 0)
    ntime = 0;
}

void Stripchart::incrtime(int n){
  ntime += n;
}
void Stripchart::incrtime(double dt){
  int n = round(dt / deltat);
  incrtime(n);
}

void Stripchart::setvalue(double v){
  if(!stopflag){
    if(v > maxval) v = maxval;
    else if(v < minval) v = minval;
    history[nvalues++] = v;

    incrtime(1);
    stopflag = (nvalues >= MAXVALUES);
  }
}
double Stripchart::value(){
  if(ntime < 0)
    return (minval + maxval) / 2;
  else
    return history[ntime];
}

void Stripchart::draw(){
  int i;
  double xloc, yloc;
  double dx;

  glPushMatrix();
    glLineWidth(2);
    glTranslatef(x0, y0, 0);
    glScalef(-size / 12, size / 12, 1);

    glPushMatrix();
      glScalef(12, 5, 1);
      glColor3f(0.4, 0.4, 0.4);
      drawBox(0, 0, GL_POLYGON);
      glColor3f(1, 1, 1);
      drawBox(0, 0, GL_LINE_LOOP);
    glPopMatrix();

    glPushMatrix();
      glScalef(11, 4, 1);
      glColor3f(0, 0, 0);
      drawBox(0, 0, GL_POLYGON);
    glPopMatrix();

    glPushMatrix();
      glColor3f(0.3, 0.3, 0);
      glTranslatef(-5.5, 0, 0);
      glBegin(GL_LINES);
        for(i = -2; i <= 2; i++){
	  glVertex2f(0, i);
	  glVertex2f(11, i);
	}
      glEnd();

      glTranslatef(0, 0, 0);
      dx = (ntime * deltat) - (int)(ntime * deltat);
      glPushMatrix();
        glTranslatef(dx, 0, 0);
	glBegin(GL_LINES);
	  for(i = 0; i < 11; i++){
	    glVertex2f(i, -2);
	    glVertex2f(i, 2);
	  }
	glEnd();
      glPopMatrix();

      if(ntime < 0)
	yloc = 0;
      else
	yloc = 4 * (value() - minval) / (maxval - minval) - 2;
      glColor3f(1, 1, 1);     
      glPushMatrix();
        glTranslatef(0, yloc, 0);
	glScalef(0.3, 0.3, 1);
	drawPointer();
      glPopMatrix();

      glBegin(GL_LINE_STRIP);
        xloc = 0;
	for(i = 0; i * deltat <= 11 && i <= ntime; i++){
	  yloc = 4 * (history[ntime - i] - minval) / (maxval - minval) - 2;
	  glVertex2f(xloc, yloc);
	  xloc += deltat;
	}
      glEnd();
    glPopMatrix();

    glPushMatrix();
      glScalef(11, 4, 1);
      glColor3f(1, 1, 1);
      drawBox(0, 0, GL_LINE_LOOP);
    glPopMatrix();

  glPopMatrix();
}
