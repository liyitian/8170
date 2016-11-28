//
// Final.cpp
//
// 
// 
// Christopher Root, 2006
// Minor Modifications by Donald House, 2009
// Minor Modifications by Yujie Shu, 2012
// Minor Modifications by Yitian Li, 2016
//

#include "Camera.h"
#include "Gauss.h"
#include "StateVector.h"
#include <vector>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif


#define STARTX -2.0
#define STARTY 8.0
#define STARTZ -2.0
#define radius 1.0
#define MAXSTEPS  10000
#define PRINTTEST cout<<"here"<<endl
int WIDTH = 1024;
int HEIGHT = 765;

int persp_win;

Camera *camera;

bool showGrid = true;
bool Stopped =true;


static Vector3d Wind;
static Vector3d StartPoint;
static Vector3d AddForce;
static Vector3d Stone;
static double StoneRadius;
static vector<StateVector> State;
static std::vector<int> Sindex;
static std::vector<Vector3d> LeavesColor;
static std::vector<double> LeavesMass;
static std::vector<double> LeavesBorntime;

static char *ParamFilename = NULL;
static int TotalNum = 0;
static double Mass;
static double TimeStep;
static double DispTime;
static double Kij;
static double Dtheta;
static double Time = 0;
static double epsilon;
static double l0 = 1;
static double theta0 = 180;
static int numofleaves=0;
static int TimerDelay;
static int TimeStepsPerDisplay;
static int NSteps = 0;
static int NTimeSteps = -1;
static int Collision[MAXSTEPS];
bool resting=false;
bool pushAforce = false;
bool restingflag[6]={true,true,true,true,true,true},totalresting=false;
static int back[4]={0,1,2,3};
static int leftface[4]={4,0,3,7};
static int front[4]={5,6,7,4};
static int rightface[4]={1,5,6,2};
static int top[4]={6,7,3,2};
static int bot[4]={0,1,5,4};
//back,left,front,right,top,bot

GLuint Texture;

void TimerCallback(int value);

// draws a simple grid
void makeGrid() {
	glColor3f(0.0, 0.0, 0.0);

	glLineWidth(1.0);

	for (float i=-12; i<12; i+=2) {
		for (float j=-12; j<12; j+=2) {
			
			glBegin(GL_LINES);
			glVertex3f(i, j, -12);
			glVertex3f(i, j+2, -12);
			glEnd();
			glBegin(GL_LINES);
			glVertex3f(i, j, -12);
			glVertex3f(i+2, j, -12);
			glEnd();
			
			
			glBegin(GL_LINES);
			glVertex3f(-12, i, j);
			glVertex3f(-12, i, j+2);
			glEnd();
			glBegin(GL_LINES);
			glVertex3f(-12, i, j);
			glVertex3f(-12, i+2, j);
			glEnd();

			
			glBegin(GL_LINES);
			glVertex3f(i, -12, j);
			glVertex3f(i, -12, j+2);
			glEnd();
			glBegin(GL_LINES);
			glVertex3f(i, -12, j);
			glVertex3f(i+2, -12, j);
			glEnd();



			if (j == -10){
				glBegin(GL_LINES);
				glVertex3f(i, 12, j-2);
				glVertex3f(i+2, 12, j-2);
				glEnd();

				glBegin(GL_LINES);
				glVertex3f(12, i, j-2);
				glVertex3f(12, i+2, j-2);
				glEnd();
				
				glBegin(GL_LINES);
				glVertex3f(i, -12, -j+2);
				glVertex3f(i+2,-12, -j+2);
				glEnd();
			}
			
			if (i == -10){		    
		 		glBegin(GL_LINES);
				glVertex3f(i-2, j, 12);
				glVertex3f(i-2, j+2, 12);
				glEnd();
				
				glBegin(GL_LINES);
				glVertex3f(i-2, 12, j);
				glVertex3f(i-2, 12, j+2);
				glEnd();

        glBegin(GL_LINES);
        glVertex3f(-i+2, -12, j);
        glVertex3f(-i+2, -12, j+2);
        glEnd();
	    	}
    	}
	}

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
  
  if(fscanf(paramfile, "%d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
            &TotalNum,&LifeTime, &Mass, &Kij,&Dtheta, &TimeStep, &DispTime, &(AddForce.x), &(AddForce.y), &(AddForce.z),&(StartPoint.x),&(StartPoint.y),&(StartPoint.z),&epsilon) != 14){
    fprintf(stderr, "error reading parameter file %s\n", filename);
    exit(1);
  }
  
  fclose(paramfile);
  
  TimeStepsPerDisplay = Max(1, int(DispTime / TimeStep + 0.5));
  TimerDelay = int(0.5 * TimeStep * 1000);
}

void makenormal(const Vector3d &v0, const Vector3d &v1, const Vector3d &v2){
  Vector3d normal = ((v1 - v0) % (v2 - v0)).normalize();
  glNormal3f(normal.x, normal.y, normal.z);
}
struct point {
  float x, y, z;
  };

void DrawLeaves()
{
  
  struct point front[4]={{-2.0,-2.0,2.0},{2.0,-2.0,2.0},{2.0,2.0,2.0},{-2.0,2.0,2.0}};
  struct point back[4]={{-2.0,-2.0,-2.0},{-2.0,2.0,-2.0},{2.0,2.0,-2.0},{2.0,-2.0,-2.0}};
  struct point leftface[4]={{-2.0,-2.0,-2.0},{-2.0,-2.0,2.0},{-2.0,2.0,2.0},{-2.0,2.0,-2.0}};
  struct point rightface[4]={{2.0,-2.0,-2.0},{2.0,2.0,-2.0},{2.0,2.0,2.0},{2.0,-2.0,2.0}};
  struct point top[4]={{-2.0,2.0,-2.0},{-2.0,2.0,2.0},{2.0,2.0,2.0},{2.0,2.0,-2.0}};
  struct point bottom[4]={{-2.0,-2.0,-2.0},{-2.0,-2.0,2.0},{2.0,-2.0,2.0},{2.0,-2.0,-2.0}};

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE_ARB);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  //TODO:Draw the CUBE
  glLoadIdentity();
//back,left,front,right,top,bot
  for(int i=0;i<TotalNum;i++){
    if(Time-State[i].Borntime>1e-6){
      glTranslatef(State[i].x.x,State[i].x.y,State[i].x.z);

      Quaternion rotate=State[i].q;
      rotate.GLrotate();
     
      glBegin(GL_QUADS); 
      glColor3f(1,1,1);
      glNormal3f(0.0,0.0,1.0);
      for(i=0;i<4;i++) glVertex3f(front[i].x,front[i].y,front[i].z);
      glNormal3f(0.0,0.0,-1.0);
      for(i=0;i<4;i++) glVertex3f(back[i].x,back[i].y,back[i].z);
      glNormal3f(-1.0,0.0,0.0);
      for(i=0;i<4;i++) glVertex3f(leftface[i].x,leftface[i].y,leftface[i].z);
      glNormal3f(1.0,0.0,0.0);
      for(i=0;i<4;i++) glVertex3f(rightface[i].x,rightface[i].y,rightface[i].z);
      glNormal3f(0.0,1.0,0.0);
      for(i=0;i<4;i++) glVertex3f(top[i].x,top[i].y,top[i].z);
      glNormal3f(0.0,-1.0,0.0);
      for(i=0;i<4;i++) glVertex3f(bottom[i].x,bottom[i].y,bottom[i].z);
      glEnd();
      
      glLoadIdentity();
      glBegin(GL_LINES);

      glVertex3f(StartPoint.x,StartPoint.y,StartPoint.z);
      Vector3d Pi(-2,-2,-2);
      Vector3d Rot(State[i].q.q.x,State[i].q.q.y,State[i].q.q.z);
      Pi=State[i].x+State[i].q.rotation()*Pi;
      glVertex3f(Pi.x,Pi.y,Pi.z);
      glEnd();
    }
  }
  glutPostRedisplay();
  
  
  //????what is this?
  if(NSteps > MAXSTEPS){
    cerr << "Particle position table overflow, restarting!!" << endl;
    Time = 0;
    NSteps = 0;
  }
}


void Restart(){
  
  LoadParameters(ParamFilename); // reload parameters in case changed
  NTimeSteps = -1;
  glutIdleFunc(NULL);
  Time = 0;
  int i=0;
//TODO:Vertex clear,& reserve the room;
  State.clear();
  State.reserve(TotalNum);  
  DrawLeaves();
}

/*
 Initialize the Simulation
 */
void InitSimulation(int argc, char* argv[]){
  
  if(argc != 2){
    fprintf(stderr, "usage: Final paramfile\n");
    exit(1);
  }
  int i=0;
  LoadParameters(argv[1]);
  State.reserve(TotalNum);

  NSteps = 0;
  NTimeSteps = -1;
  Time = 0;
  
  //TODO:Vertex clear
}


double min(double a, double b){
  if (a<b)return a; else return b;
}

StateVector Force(StateVector S)
{
  double m= S.m;
  Matrix3x3 I0T= S.I0;
  StateVector Sdot(m,I0T);

  
  Sdot.x= S.P/m;
  Matrix3x3 R= S.q.rotation();
  I0T=I0T.inv();
  Matrix3x3 It= R * I0T * R.transpose();
  Vector3d w = S.L*I0T;

  Quaternion wq= Quaternion(w);
  Sdot.q=0.5* wq * S.q;

  Vector3d zero(0,0,0);
  Sdot.P=zero;
  Sdot.L=zero;
  
  Vector3d g(0,-10,0);
  Sdot.P=g*Mass;
  Vector3d Pi(-2,-2,-2);
  Vector3d Rot(S.q.q.x,S.q.q.y,S.q.q.z);
  Pi=S.x+S.q.rotation()*Pi;

  if ((StartPoint-Pi).norm()>2){
      Vector3d springforce=Kij*(StartPoint-Pi).normalize()*(StartPoint-Pi).norm();
      Vector3d damper=Dtheta*S.velocity;
      Vector3d ForceonP=springforce+damper;
      Sdot.P=Sdot.P+ForceonP;
      Sdot.L=Sdot.L+ ((Pi-S.x) % ForceonP);
  }

  return Sdot;
}

void Simulate()
{

  
  //Leaves Killer....
  for(int i=0;i<TotalNum;i++){
    if (Time-State[i].Borntime-State[i].lifetime<1e-6){
      State[i].erase(State.begin()+i);
    }
  }
  
  //Leaves Generator
  Matrix3x3 BigI(Mass/12*32,0,0,0,Mass/12*32,0,0,0,Mass/12*32);
  if (numofleaves<TotalNum){
    for(int i=numofleaves;i<TotalNum;i++){
      Vector3d P0=StartPoint;
      Vector3d zero(0,0,0);
      Vector3d V0(0,0,0);
      Vector3d Orientation(1e-6,1e-6,1e-6);
      Quaternion q=Quaternion(Orientation);
      double btime=Time+(double)rand()/(RAND_MAX)*epsilon*2;
      double ltime=abs(gauss(LifeTime,epsilon/100,Time));
      btime=Time;
      ltime=LifeTime;
      StateVector NewLeaf(P0,Mass,V0,V0*Mass,zero,q,BigI,btime,ltime);
      State[i].push_back(NewLeaf);
    }
  }


  for(int i=0;i<numofleaves;i++){
    StateVector K1,K2,K3,K4,Snew;
    K1=Force(State[i]);
    K2=Force(State[i]+TimeStep/2*K1);
    K3=Force(State[i]+TimeStep/2*K2);
    K4=Force(State[i]+TimeStep*K3);

    Snew=State[i]+TimeStep/6.0*(K1+2*K2+2*K3+K4);
    Snew.q=Snew.q.normalize();
    State[i]=Snew;
  }
  
  Time += TimeStep;
  NTimeSteps++;
  DrawLeaves();
  if (!Stopped)
    glutTimerFunc(TimerDelay, TimerCallback, 0);
}


void TimerCallback(int timertype)
{
  switch(timertype){
    case 0:
      Simulate();
      break;
  }
}

void do_lights()
{
  float light0_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
  float light0_diffuse[] = { 1, 1, 1, 1.0 };
  float light0_specular[] = { 0.1, 0.1, 0.1, 1.0 };
  float light0_position[] = { 10.0, 10.0, 18.0, 1.0 };
  float light0_direction[] = { -1.0, -1.0, -1.0, 1.0};
  float lmodel_ambient[] = { 0.2, 0.2, 0.2, 1.0 };


  glLightfv(GL_LIGHT0,GL_AMBIENT,light0_ambient); 
  glLightfv(GL_LIGHT0,GL_DIFFUSE,light0_diffuse); 
  //glLightfv(GL_LIGHT0,GL_SPECULAR,light0_specular);

  glLightfv(GL_LIGHT0,GL_POSITION,light0_position);
  //glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,light0_direction);
  
  //glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
}
void do_material()
{
float mat_ambient[] = {0.1,0.1,0.1,1.0};
float mat_diffuse[] = {0.7,0.7,0.7,1.0};
float mat_specular[] = {0.05,0.05,0.05,1.0};
float mat_emission[] = {0.0,0.0,0.0,1.0};
float low_shininess[] = { 10.0 };


glEnable(GL_COLOR_MATERIAL);
  //glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
  //glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,mat_specular);
  //glMaterialfv(GL_FRONT,GL_EMISSION,mat_emission);
  //glMaterialfv(GL_FRONT,GL_SHININESS,low_shininess);
//glMaterialfv(GL_FRONT,GL_AMBIENT,mat_ambient);
//glMaterialfv(GL_FRONT,GL_DIFFUSE,mat_diffuse);
//
//glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess);

}


void PerspDisplay() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  camera->PerspectiveDisplay(WIDTH, HEIGHT);

  glMatrixMode(GL_MODELVIEW);
  DrawLeaves();
  glLoadIdentity();
  if (showGrid){ 
    glDisable(GL_LIGHT0);
    makeGrid();
    glEnable(GL_LIGHT0);
  }

  glutSwapBuffers();
}

void mouseEventHandler(int button, int state, int x, int y) {
  // let the camera handle some specific mouse events (similar to maya)
  camera->HandleMouseEvent(button, state, x, y);
  glutPostRedisplay();
}

void motionEventHandler(int x, int y) {
  // let the camera handle some mouse motions if the camera is to be moved
  camera->HandleMouseMotion(x, y);
  glutPostRedisplay();
}

void hit()
{
  pushAforce=true;
  cout<<"true"<<endl;
}


void keyboardEventHandler(unsigned char key, int x, int y) {
  switch (key) {
  case 'r': case 'R':
    // reset the camera to its initial position
    camera->Reset();
    break;
  case 'f': case 'F':
    camera->Reset();
    break;
  case 'g': case 'G':
    showGrid = !showGrid;
    break;
  case 's': case 'S':
    Restart();
    if (Stopped){
      glutTimerFunc(TimerDelay,TimerCallback,0);
      Stopped=!Stopped;
    }
    break;
  case 'a': case 'A':
    hit();
    break;
  case 'd': case 'D':
    //hit2();
    break;
  case 't': case 'T':
    Stopped=TRUE;
    break;
      
  case 'q': case 'Q':	// q or esc - quit
  case 27:		// esc
    exit(0);
  }

  glutPostRedisplay();
}

void init() {
  // set up camera
  // parameters are eye point, aim point, up vector
  camera = new Camera(Vector3d(0, 5, 50), Vector3d(0, 0, 0), 
          Vector3d(0, 1, 0));
  // grey background for window
  glClearColor(0.62, 0.62, 0.62, 0.0);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_NORMALIZE);
  glEnable(GL_POINT_SMOOTH);
  glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

}

int main(int argc, char *argv[]) {

  // set up opengl window
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(WIDTH, HEIGHT);
  glutInitWindowPosition(200, 200);
  persp_win = glutCreateWindow("Final");


  // initialize the camera and such
  init();
  InitSimulation(argc, argv);
  
  // set up opengl callback functions
  glutDisplayFunc(PerspDisplay);
  glutMouseFunc(mouseEventHandler);
  glutMotionFunc(motionEventHandler);
  glutKeyboardFunc(keyboardEventHandler);

  do_lights();
  do_material();


  glutMainLoop();
  return(0);
}

