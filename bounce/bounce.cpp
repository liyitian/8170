/********************************************************************
 
 2D Bouncing Ball Demo
 Donald H. House  January 16, 1997
 Physically Based Animation
 Clemson University
 Digital Production Arts
 
 *********************************************************************/

#include "Vector.h"

#include <cstdlib>
#include <cstdio>

#ifdef __APPLE__
#  pragma clang diagnostic ignored "-Wdeprecated-declarations"
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

using namespace std;

#define WINDOW_WIDTH	1000	/* window dimensions */
#define WINDOW_HEIGHT	740

#define CIRC_INC	(2 * PI / 30)	/* fineness of circle drawing */

/* RGB colors */

#define RGBBLACK	0, 0, 0
#define RGBRED		1, 0, 0
#define RGBORANGE	1, 0.5, 0
#define RGBYELLOW	1, 1, 0
#define RGBGREEN	0, 1, 0
#define RGBBLUE		0.5, 0.5, 1
#define RGBVIOLET	1, 0, 0.5
#define RGBWHITE	1, 1, 1

#define FLOORHEIGHT	(WINDOW_HEIGHT / 20)
#define STARTX		(WINDOW_WIDTH / 20)
#define STARTY		(WINDOW_HEIGHT - WINDOW_HEIGHT / 20)
#define RADIUS		(((WINDOW_WIDTH + WINDOW_HEIGHT) / 2) / 50)

#define EPS		0.1

#define MAXSTEPS	10000

#define MenuContinuous	1
#define MenuThrow	2
#define MenuTrace	3
#define MenuWeight	4
#define MenuWind	5
#define MenuFloor	6
#define MenuReset	7
#define MenuClean	8
#define MenuQuit	9

#define NOAIR		0
#define LIGHT		1
#define MEDIUM		2
#define HEAVY		3

int FrameNumber = 0;

void TimerCallback(int value);

static double WinWidth = WINDOW_WIDTH;
static double WinHeight = WINDOW_HEIGHT;

static char *ParamFilename = NULL;

static int Throw = false;
static int Trace = true;
static int WeightMatters = 0;
static int HaveWind = false;
static int Floor = false;

static int Step = true;
static int Start = true;
static int Stopped = true;
static Vector2d LastMouse(0, 0);
static Vector2d V0;
static Vector2d Wind;
static Vector2d G(0, -10.0);

static double Mass;
static double Viscosity;
static double TimeStep;
static double DispTime;
static int TimeStepsPerDisplay;
static int TimerDelay;
static double CoeffofRestitution;

static int NSteps = 0;
static int NTimeSteps = -1;
static double Time = 0;
static Vector2d OldBall[MAXSTEPS];
static int Collision[MAXSTEPS];
static Vector2d Ball(STARTX, STARTY);
static Vector2d Velocity(0, 0);

/*
 Draw an outlined circle with center at position (x, y) and radius rad
 */
void OutlineCirclef(float x, float y, float rad){
  
  float theta;
  
  glLineWidth(1);
  
  glBegin(GL_LINE_LOOP);
  for(theta=0.0; theta < 2 * PI; theta += CIRC_INC)
    glVertex2f(x+rad*cos(theta), y+rad*sin(theta));
  glEnd();
}

/*
 Draw a filled circle with center at position (x, y) and radius rad
 */
void Circlef(float x, float y, float rad){
  
  float theta;
  
  glBegin(GL_POLYGON);
  for(theta=0.0; theta < 2 * PI; theta += CIRC_INC)
    glVertex2f(x+rad*cos(theta), y+rad*sin(theta));
  glEnd();
}

/*
 Erase the floor
 */
void EraseFloor(){
  
  glLineWidth(4);
  glColor3f(RGBBLACK);
  glBegin(GL_LINES);
  glVertex2f(0, FLOORHEIGHT);
  glVertex2f(WINDOW_WIDTH, FLOORHEIGHT);
  glEnd();
}

/*
 Draw the floor
 */
void DrawFloor(){
  
  glLineWidth(4);
  glColor3f(RGBGREEN);
  glBegin(GL_LINES);
  glVertex2f(0, FLOORHEIGHT);
  glVertex2f(WINDOW_WIDTH, FLOORHEIGHT);
  glEnd();
}

/*
 Draw the ball, its traces and the floor if needed
 */
void DrawBall(int collision){
  
  int i;
  
  glClear(GL_COLOR_BUFFER_BIT);
  
  if(Trace){
    for(i = 0; i < NSteps && i < MAXSTEPS; i++){
      if(Collision[i])
        glColor3f(RGBRED);
      else
        glColor3f(RGBYELLOW);
      
      OutlineCirclef(OldBall[i].x, OldBall[i].y, RADIUS);
    }
  }
  
  if(Floor)
    DrawFloor();
  else
    EraseFloor();
  
  if(collision)
    glColor3f(RGBRED);
  else
    glColor3f(RGBYELLOW);
  
  Circlef(Ball.x, Ball.y, RADIUS);
  glutSwapBuffers();
  
  if(NSteps < MAXSTEPS){
    OldBall[NSteps] = Ball;
    Collision[NSteps] = collision;
    NSteps++;
  }
  else{
    cerr << "Ball position table overflow, restarting!!" << endl;
    Time = 0;
    NSteps = 0;
  }
}

/*
 Compute acceleration on Ball
 */
Vector2d Accel(){
  
  Vector2d acceleration;
  double mass;
  
  switch(WeightMatters){
    case NOAIR:
      mass = Mass;
      break;
    case LIGHT:
      mass = Mass / 3;
      break;
    case MEDIUM:
      mass = Mass;
      break;
    case HEAVY:
      mass = Mass * 3;
      break;
  }
  
  acceleration = G;
  if(WeightMatters != NOAIR){
    if(HaveWind)
      acceleration = acceleration + Viscosity * (Wind - Velocity) / mass;
    else
      acceleration = acceleration - Viscosity * Velocity / mass;
  }
  
  return acceleration;
}

/*
 Run a single time step in the simulation
 */
void Simulate(){
  
  Vector2d acceleration;
  Vector2d newvelocity, newball;
  int resting;
  
  if(Stopped)
    return;
  
  // ball in resting contact if its vertical velocity is nearly zero, and the ball's
  // height is the floor height
  resting = Abs(TimeStep * Velocity.y) < EPS && Abs(Ball.y - (RADIUS + FLOORHEIGHT)) < EPS;
  
  // get the new acceleration
  acceleration = Accel();
  
  // if ball in resting contact, and its acceleration is zero or down, then
  // cancel the vertical velocity and acceleration and place the ball exactly
  // on the floor. Clear resting contact if acceleration is up.
  if(resting && acceleration.y < EPS){
    Velocity.y = 0;
    Ball.y = RADIUS + FLOORHEIGHT;
    acceleration.y = 0;
  }
  else
    resting = false;
  
  // evil Euler integration to get velocity and position at next timestep
  newvelocity = Velocity + TimeStep * acceleration;
  newball = Ball + TimeStep * Velocity;
  
  // if ball not in resting contact, and the floor is turned on, check for
  // collision with the floor in the timestep
  if(!resting && Floor &&
     newball.y - RADIUS <= FLOORHEIGHT && Ball.y - RADIUS > FLOORHEIGHT){
    // have collision, get fraction of timestep at which collision will occur
    double f = (FLOORHEIGHT - (Ball.y - RADIUS)) / (newball.y - Ball.y);
    // and compute the velocity and position of the ball at collision time
    newvelocity = Velocity + f * TimeStep * acceleration;
    newball = Ball + f * TimeStep * Velocity;
    
    // reflect the velocity from the floor, and scale the vertical component
    // by the coefficient of restitution
    Velocity = newvelocity;
    Velocity.y *= -CoeffofRestitution;
    
    // draw the collision point for reference
    Ball = newball;
    DrawBall(1);
    
    // now finish by integrating over remainder of the timestep
    acceleration = Accel();
    newvelocity = Velocity + (1 - f) * TimeStep * acceleration;
    newball = newball + (1 - f) * TimeStep * Velocity;
  }
  
  // advance the timestep and set the velocity and position to their new values
  Time += TimeStep;
  NTimeSteps++;
  Velocity = newvelocity;
  Ball = newball;
  
  /////////////////////////////
  
  // draw only if we are at a display time
  if(NTimeSteps % TimeStepsPerDisplay == 0)
    DrawBall(0);
  
  // set up time for next timestep if in continuous mode
  glutIdleFunc(NULL);
  if(Step)
    Stopped = true;
  else{
    Stopped = false;
    glutTimerFunc(TimerDelay, TimerCallback, 0);
  }
}

/*
 Run a single time step in the simulation
 */
void TimerCallback(int timertype){
  switch(timertype){
    case 0:
      Simulate();
      break;

    case MenuContinuous:
      if(Step)
        glutChangeToMenuEntry(MenuContinuous, "Continuous", MenuContinuous);
      else
        glutChangeToMenuEntry(MenuContinuous, "Step", MenuContinuous);
      break;
      
    case MenuThrow:
      if(Throw)
        glutChangeToMenuEntry(MenuThrow, "Drop", MenuThrow);
      else
        glutChangeToMenuEntry(MenuThrow, "Throw", MenuThrow);
      break;
      
    case MenuTrace:
      if(Trace)
        glutChangeToMenuEntry(MenuTrace, "No Trace", MenuTrace);
      else
        glutChangeToMenuEntry(MenuTrace, "Trace", MenuTrace);
      break;
      
    case MenuWeight:
      switch(WeightMatters){
        case NOAIR:
          glutChangeToMenuEntry(MenuWeight, "Light", MenuWeight);
          break;
        case LIGHT:
          glutChangeToMenuEntry(MenuWeight, "Medium", MenuWeight);
          break;
        case MEDIUM:
          glutChangeToMenuEntry(MenuWeight, "Heavy", MenuWeight);
          break;
        case HEAVY:
          glutChangeToMenuEntry(MenuWeight, "No Air", MenuWeight);
          break;
      }
      break;
      
    case MenuWind:
      if(HaveWind)
        glutChangeToMenuEntry(MenuWind, "No Wind", MenuWind);
      else
        glutChangeToMenuEntry(MenuWind, "Wind", MenuWind);
      break;
      
    case MenuFloor:
      if(Floor)
        glutChangeToMenuEntry(MenuFloor, "No Floor", MenuFloor);
      else
        glutChangeToMenuEntry(MenuFloor, "Floor", MenuFloor);
      break;
  }
}

/*
 Adjust mouse coordinates to match window coordinates
 */
void AdjustMouse(int& x, int& y){
  
  /* reverse y, so zero at bottom, and max at top */
  y = int(WinHeight - y);
  
  /* rescale x, y to match current window size (may have been rescaled) */
  y = int(y * WINDOW_HEIGHT / WinHeight);
  x = int(x * WINDOW_WIDTH / WinWidth);
}

/*
 Watch mouse button presses and handle them
*/
void handleButton(int button, int state, int x, int y){
  
  if(button != GLUT_LEFT_BUTTON)
    return;
  
  AdjustMouse(x, y);	/* adjust mouse coords to current window size */
  
  if(state == GLUT_UP){
    if(Start){
      Start = false;
      Stopped = false;
      Ball.set(STARTX, STARTY);
      if(Throw)
        Velocity = V0;
      else
        Velocity.set(0, 0);
      DrawBall(0);
      glutIdleFunc(Simulate);
    }
    else if(Stopped){
      Stopped = false;
      glutIdleFunc(Simulate);
    }
    else{
      Stopped = true;
      glutIdleFunc(NULL);
    }
  }
}

/*
 On Redraw request, erase the window and redraw everything
 */
void drawDisplay(){
  
  glLoadIdentity();
  
  DrawBall(0);
}

/*
 On Reshape request, reshape viewing coordinates to keep the viewport set
 to the original window proportions and to keep the window coordinates fixed
 */
void doReshape(int w, int h){
  int vpw, vph;
  
  float aspect = float(WINDOW_WIDTH) / float(WINDOW_HEIGHT);
  if(float(w) / float(h) > aspect){
    vph = h;
    vpw = int(aspect * h + 0.5);
  }
  else{
    vpw = w;
    vph = int(w / aspect + 0.5);
  }
  
  glViewport(0, 0, vpw, vph);
  WinWidth = w;
  WinHeight = h;
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
  
  glMatrixMode(GL_MODELVIEW);
}

/*
 Load parameter file and reinitialize global parameters
 */
void LoadParameters(char *filename){
  
  FILE *paramfile;
  
  if((paramfile = fopen(filename, "r")) == NULL){
    fprintf(stderr, "error opening parameter file %s\n", filename);
    exit(1);
  }
  
  ParamFilename = filename;
  
  if(fscanf(paramfile, "%lf %lf %lf %lf %lf %lf %lf %lf %lf",
            &Mass, &(V0.x), &(V0.y), &Viscosity, &CoeffofRestitution,
            &TimeStep, &DispTime, &(Wind.x), &(Wind.y)) != 9){
    fprintf(stderr, "error reading parameter file %s\n", filename);
    exit(1);
  }
  
  fclose(paramfile);
  
  TimeStepsPerDisplay = Max(1, int(DispTime / TimeStep + 0.5));
  TimerDelay = int(0.5 * TimeStep * 1000);
}

/*
 Initialize the Simulation
 */
void InitSimulation(int argc, char* argv[]){
  
  if(argc != 2){
    fprintf(stderr, "usage: bounce paramfile\n");
    exit(1);
  }
  
  LoadParameters(argv[1]);
  
  NSteps = 0;
  NTimeSteps = -1;
  Time = 0;
}

/*
 Routine to restart the ball at the top
 */
void RestartBall(){
  
  LoadParameters(ParamFilename); // reload parameters in case changed
  
  Start = true;
  Stopped = true;
  NTimeSteps = -1;
  glutIdleFunc(NULL);
  Ball.set(STARTX, STARTY);
  Time = 0;
  if(Throw)
    Velocity = V0;
  else
    Velocity.set(0, 0);
  DrawBall(0);
}

/*
 Menu callback
 */
void HandleMenu(int index){
  
  switch(index){
    
    // some of these cases change the menu entry itself. In these cases, we
    // set a fast timer, and change the menu entry in the timer callback
    case MenuContinuous:
      if(Step)
        Step = false;
      else
        Step = true;
      glutTimerFunc(1, TimerCallback ,index);
      break;
      
    case MenuThrow:
      if(Throw)
        Throw = false;
      else
        Throw = true;
      glutTimerFunc(1, TimerCallback ,index);
      break;
      
    case MenuTrace:
      if(Trace)
        Trace = false;
      else
        Trace = true;
      glutTimerFunc(1, TimerCallback ,index);
      break;
      
    case MenuWeight:
      switch(WeightMatters){
        case NOAIR:
          WeightMatters = LIGHT;
          break;
        case LIGHT:
          WeightMatters = MEDIUM;
          break;
        case MEDIUM:
          WeightMatters = HEAVY;
          break;
        case HEAVY:
          WeightMatters = NOAIR;
          break;
      }
      glutTimerFunc(1, TimerCallback ,index);
      break;
      
    case MenuWind:
      if(HaveWind){
        HaveWind = false;
        Wind.set(0, 0);
      }
      else
        HaveWind = true;
      glutTimerFunc(1, TimerCallback ,index);
      break;
      
    case MenuFloor:
      if(Floor){
        Floor = false;
        DrawBall(0);
      }
      else{
        Floor = true;
        DrawBall(0);
      }
      glutTimerFunc(1, TimerCallback ,index);
      break;
      
    case MenuReset:
      RestartBall();
      break;
      
    case MenuClean:
      NSteps = 0;
      RestartBall();
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
  glutAddMenuEntry("Throw", MenuThrow);
  glutAddMenuEntry("No Trace", MenuTrace);
  glutAddMenuEntry("Light", MenuWeight);
  glutAddMenuEntry("Wind", MenuWind);
  glutAddMenuEntry("Floor", MenuFloor);
  glutAddMenuEntry("Reset", MenuReset);
  glutAddMenuEntry("Clean", MenuClean);
  glutAddMenuEntry("Quit", MenuQuit);
  
  glutSetMenu(id);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}

//
// Keypress handling
//
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
 Main program to set up display
*/
int main(int argc, char* argv[]){
  
  glutInit(&argc, argv);
  
  InitSimulation(argc, argv);
  
  /* open window and establish coordinate system on it */
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  glutCreateWindow("Bouncing Ball Demo");
  
  /* register display and mouse-button callback routines */
  glutReshapeFunc(doReshape);
  glutDisplayFunc(drawDisplay);
  glutMouseFunc(handleButton);
  glutKeyboardFunc(handleKey);
  
  /* Set up to clear screen to black */
  glClearColor(RGBBLACK, 0);
  
  /* Set shading to flat shading */
  glShadeModel(GL_FLAT);
  
  /* Set up the menu to control the process */
  MakeMenu();
  
  glutMainLoop();
  return 0;
}
