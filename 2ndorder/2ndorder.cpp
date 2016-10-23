/*
  2ndorder.cpp

   Demonstration Program for Second Order Systems
   Uses Stripchart Recorder and Meter Objects

   VIZA 659            Donald H. House         3/9/00

   To compile:
      g++ -o 2ndorder 2ndorder.C Meter.o Stripchart.o -lglut -lGL -lm
*/
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#include <cstdlib>
#include <cstdio>

#include "Meter.h"
#include "Stripchart.h"
#include "Strut.h"

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

using namespace std;

#define TRUE        1
#define FALSE       0
#define PI	    3.1415926536

//*****************************************************************
//  Demo Code
//*****************************************************************

#define WIDTH           1000	// window dimensions
#define HEIGHT          800
#define XMIN            -100	// window drawing coordinates
#define XMAX            100
#define YMIN            -80
#define YMAX            80

#define STRUTWIDTH	(4 * (XMAX - XMIN) / 5)
#define STRUTX		(XMIN + STRUTWIDTH / 2)
#define STRUTY		(3 * (YMAX - YMIN) / 4 + YMIN)

#define CHARTWIDTH	(2 * (XMAX - XMIN) / 3)
#define CHARTX		0
#define CHARTY		((YMAX - YMIN) / 4 + YMIN)

#define METERWIDTH	((XMAX - XMIN) / 8)
#define METERY		((YMAX - YMIN) / 4 + YMIN)
#define DMETERX		(XMAX - ((XMAX - XMIN) - CHARTWIDTH) / 4)
#define SMETERX		(XMIN + ((XMAX - XMIN) - CHARTWIDTH) / 4)

#define MenuContinuous	1
#define MenuKD		2
#define MenuMD		3
#define MenuMK		4
#define MenuMKD		5
#define MenuStop	6
#define MenuReset	7
#define MenuQuit	8

#define MAXDISPLACEMENT 10
#define DELTAT      	0.05	// time increment for simulation
#define CHARTDT		(DELTAT)	// strip chart dt
#define KMOUSE		0.1

double M = 1;
double Ks = (PI * PI);	// freq = 1/(2PI) sqrt(K/M) hz
double Kd = 1;		// T = 2M/D sec
double MaxForce = MAXDISPLACEMENT * (PI * PI);

double X0 = 0.8;
double V0 = 0;

int leftButton = FALSE;		// Mouse button status 
int middleButton = FALSE;
int rightButton = FALSE;

int currX, currY;		// Most recent button-down mouse coords
double mouseX, mouseY;

void TimerCallback(int value);

static int Step = TRUE;
static int Start = TRUE;
static int Stopped = TRUE;

static double TimeStep;
static double DispTime;
static int TimerDelay;

static double Time = 0;

//
// Meter and Stripchart Objects
//

// Spring/Mass/Damper Strut
Strut *myStrut;

// Damping force meter
Meter *DMeter;

// Spring force meter
Meter *SMeter;

// X displacement stripchart
Stripchart *XChart;

/*
  Restart the simulation by resetting IC's and clearing the stripchart
*/
void RestartSimulation(){
  double fs, fd;

  Start = TRUE;
  Stopped = TRUE;
  glutIdleFunc(NULL);
  myStrut->setICs(X0 * MAXDISPLACEMENT, V0);
  XChart->reset();
  XChart->setvalue(myStrut->displacement());

  myStrut->forces(fs, fd);
  SMeter->setvalue(4 * fs);			// meter spring force
  DMeter->setvalue(8 * fd);			// meter damping force

  Time = 0;

  glutPostRedisplay();
}

/*
  Run a single time step in the simulation
*/
void Simulate(){
  double fs, fd;

  if(Stopped)
    return;

  myStrut->dotimestep(TimeStep);

  myStrut->forces(fs, fd);
  SMeter->setvalue(4 * fs);				// meter spring force
  DMeter->setvalue(8 * fd);				// meter damping force
  XChart->setvalue(myStrut->displacement()); 	// chart vert motion

  Time += TimeStep;

  //if(Time / DispTime - (int)(Time / DispTime) < TimeStep)
    glutPostRedisplay();

  if(Step){
    Stopped = TRUE;
    glutIdleFunc(NULL);
  }
  else{
    Stopped = FALSE;
    glutTimerFunc(TimerDelay, TimerCallback, 0);
    glutIdleFunc(NULL);
  }
}

/*
  Run a single time step in the simulation
*/
void TimerCallback(int){

  Simulate();
}

/*
   Display callback: clear the screen and draw the meter and stripchart
*/
void drawScreen(){
  
  /* clear the window to the background color */
  glClear(GL_COLOR_BUFFER_BIT);
  
  /* draw the spring/mass/damper system */
  myStrut->draw();

  /* draw the metering devices */
  DMeter->draw();
  SMeter->draw();
  XChart->draw();

  glutSwapBuffers();
}

/*
  Callback routine to respond to Mouse Button
*/
void handleButtons(int button, int state, int x, int y){
  y = HEIGHT - y;
  
  switch(button){
  case GLUT_LEFT_BUTTON:
    leftButton = (state == GLUT_DOWN);

    currX = x;
    currY = y;

    if(state == GLUT_UP){
      if(Start){
	Start = FALSE;
	Stopped = FALSE;	
	glutPostRedisplay();
	glutIdleFunc(Simulate);   
      }
      else if(Stopped){
	Stopped = FALSE;
	glutIdleFunc(Simulate);    
      }
      else{
	Stopped = TRUE;
	glutIdleFunc(NULL);    
      }
    }
    break;
  case GLUT_MIDDLE_BUTTON:
    middleButton = (state == GLUT_DOWN);
    break;
  case GLUT_RIGHT_BUTTON:
    rightButton = (state == GLUT_DOWN);
    break;
  }
}

/*
  Callback routine to respond to Mouse motion
*/
void handleMotion(int x, int y){
  double dx, dy;

  y = HEIGHT - y;
  
  if(leftButton){	// track left button
    dx = x - currX;
    currX = x;
    dy = y - currY;
    currY = y;
    mouseX += dx;
    mouseY += dy;
  }
}

/*
 Keyboard Callback Routine: 'q' or ESC quit
*/
void handleKey(unsigned char key, int x, int y){
  
  switch(key){
    case 'q':		// q - quit
    case 'Q':
    case 27:		// esc - quit
      exit(0);
      
    default:		// not a valid key -- just ignore it
      return;
  }
}

/*
  Menu callback
*/
void HandleMenu(int index){

  switch(index){

  case MenuContinuous:
    if(Step){
      Step = FALSE;
      glutChangeToMenuEntry(index, "Step", index);
    }
    else{
      Step = TRUE;
      glutChangeToMenuEntry(index, "Continuous", index);
    }  
    break;

  case MenuKD:
    myStrut->setconstants(0, Ks, Kd);
    glutPostRedisplay();
    break;

  case MenuMD:
    myStrut->setconstants(M, 0, Kd);
    glutPostRedisplay();
    break;

  case MenuMK:
    myStrut->setconstants(M, Ks, 0);
    glutPostRedisplay();
    break;

  case MenuMKD:
    myStrut->setconstants(M, Ks, Kd);
    glutPostRedisplay();
    break;

  case MenuStop:
    Stopped = TRUE;
    break;

  case MenuReset:
    RestartSimulation();
    break;

  case MenuQuit:
    exit(0);

  }
}

/*
   Set up pop-up menu on right mouse button
*/
void MakeMenu(){

  int id = glutCreateMenu(HandleMenu);

  glutAddMenuEntry("Continuous", MenuContinuous);
  glutAddMenuEntry("K-D", MenuKD);
  glutAddMenuEntry("M-D", MenuMD);
  glutAddMenuEntry("M-K", MenuMK);
  glutAddMenuEntry("M-K-D", MenuMKD);
  glutAddMenuEntry("Stop", MenuStop);
  glutAddMenuEntry("Reset", MenuReset);
  glutAddMenuEntry("Quit", MenuQuit);

  glutSetMenu(id);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}
 
/*
  Procedure to get mass, spring constant, damping constant, and initial
  displacement and velocity from the command line. 
  If x0 not present it is set to 0.8 (relative to max of stripchart)
  If v0 not present it is set to 0
*/
void initialize(int argc, char *argv[]){
  if(argc < 4){
    cout << "usage: 2ndorder m k d [x0 [v0]]" << endl;
    exit(1);
  }

  sscanf(argv[1], "%lf", &M);
  sscanf(argv[2], "%lf", &Ks);
  sscanf(argv[3], "%lf", &Kd);
  if(argc >= 5)
    sscanf(argv[4], "%lf", &X0);
  if(argc >= 6)
    sscanf(argv[5], "%lf", &V0);

  // Spring/Mass/Damper Strut
  myStrut = new Strut(STRUTX, STRUTY, STRUTWIDTH, MAXDISPLACEMENT, 
		      M, Ks, Kd);

  // Damping force meter
  DMeter = new Meter(0, DMETERX, METERY, METERWIDTH, -MaxForce, MaxForce);

  // Spring force meter
  SMeter = new Meter(0, SMETERX, METERY, METERWIDTH, -MaxForce, MaxForce);

  // X displacement stripchart
  XChart = new Stripchart(CHARTX, CHARTY, CHARTWIDTH, CHARTDT, 
			  -MAXDISPLACEMENT, MAXDISPLACEMENT);

  myStrut->setICs(X0 * MAXDISPLACEMENT, V0);
  XChart->setvalue(myStrut->displacement());
}


/*
   Main program to drive the meter and stripchart
*/
int main(int argc, char* argv[]){

  initialize(argc, argv);

  /* start up the glut utilities */
  glutInit(&argc, argv);

  DispTime = TimeStep = DELTAT;
  TimerDelay = int(0.5 * TimeStep * 1000);
  Time = 0;

  /* create the graphics window, giving width, height, and title text */
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize(WIDTH, HEIGHT);
  glutCreateWindow("2nd Order System Demonstration");

  /* callback to draw graphics when window needs updating */
  glutDisplayFunc(drawScreen);
  /* callbacKs to respond to mouse clicks and motion */  
  glutMouseFunc(handleButtons);
  glutMotionFunc(handleMotion);
  glutKeyboardFunc(handleKey);

  /* Set up the menu to control the process */
  MakeMenu();

  /* lower left of window is (XMIN, YMIN), upper right is (XMAX, YMAX) */
  gluOrtho2D(XMIN, XMAX, YMIN, YMAX);

  /* specify window clear (background) color to be black */
  glClearColor(0, 0, 0, 0);

  glutMainLoop();
  return 0;
}
