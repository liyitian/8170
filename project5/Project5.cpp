//
// Project5.cpp
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
static StateVector Particles;
static StateVector Snew;
static StateVector Sdot;
static std::vector<int> Sindex;
static std::vector<Vector3d> ParticlesColor;
static std::vector<double> ParticlesMass;
static std::vector<double> ParticlesBorntime;

static char *ParamFilename = NULL;
static int TotalNum = 1;
static double Mass;
static double TimeStep;
static double DispTime;
static double Kij;
static double Dtheta;
static double Time = 0;
static double epsilon;
static double l0 = 1;
static double theta0 = 180;
static int TimerDelay;
static int TimeStepsPerDisplay;
static int NSteps = 0;
static int NTimeSteps = -1;
static int Collision[MAXSTEPS];
bool resting=false;
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
  
  if(fscanf(paramfile, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
            &Mass, &Kij,&Dtheta, &TimeStep, &DispTime, &(AddForce.x), &(AddForce.y), &(AddForce.z),&(StartPoint.x),&(StartPoint.y),&(StartPoint.z),&epsilon) != 12){
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

void DrawParticles()
{
  
  struct point front[4]={{0.0,0.0,4.0},{4.0,0.0,4.0},{4.0,4.0,4.0},{0.0,4.0,4.0}};
  struct point back[4]={{0.0,0.0,0.0},{0.0,4.0,0.0},{4.0,4.0,0.0},{4.0,0.0,0.0}};
  struct point leftface[4]={{0.0,0.0,0.0},{0.0,0.0,4.0},{0.0,4.0,4.0},{0.0,4.0,0.0}};
  struct point rightface[4]={{4.0,0.0,0.0},{4.0,4.0,0.0},{4.0,4.0,4.0},{4.0,0.0,4.0}};
  struct point top[4]={{0.0,4.0,0.0},{0.0,4.0,4.0},{4.0,4.0,4.0},{4.0,4.0,0.0}};
  struct point bottom[4]={{0.0,0.0,0.0},{0.0,0.0,4.0},{4.0,0.0,4.0},{4.0,0.0,0.0}};

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE_ARB);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  //TODO:Draw the CUBE
  glLoadIdentity();
//back,left,front,right,top,bot
  glTranslatef(Particles.x.x,Particles.x.y,Particles.x.z);
  // Vector3d Zaxis;
  // Zaxis.x=0.0;Zaxis.y=0.0;Zaxis.z=1.0;
  Quaternion rotate=Particles.q;
  glRotatef(rotate.angle(),rotate.axis().x,rotate.axis().y,rotate.axis().z);
  //cout<<rotate<<endl;
  
  // Vector3d RotateAxis=(rotate%Zaxis).normalize();
  int i;
  glBegin(GL_QUADS); 
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

//TODO:Vertex clear,& reserve the room;
  Particles.setsize(TotalNum);
  Particles.setn(0);
  Sdot.setsize(TotalNum);
  Snew.setsize(TotalNum);
  ParticlesMass.clear();
  ParticlesMass.reserve(2*TotalNum);
  
  DrawParticles();
}

/*
 Initialize the Simulation
 */
void InitSimulation(int argc, char* argv[]){
  
  if(argc != 2){
    fprintf(stderr, "usage: Project5 paramfile\n");
    exit(1);
  }
  
  LoadParameters(argv[1]);
  Particles.setsize(TotalNum);
  Particles.setn(0);
  //Particles.setn(TotalNum);

  NSteps = 0;
  NTimeSteps = -1;
  Time = 0;
  
  //TODO:Vertex clear
}

// //TODO: Acceleration
// StateVector Acceleration(StateVector S, double t)
// {
//   StateVector Sdot(S.getn());
//   Vector3d fij;
//   Vector3d tmp(0,-0.1,0);
//   for(int i=0;i<S.getn();i++){
//     //cout<<S.getn()+i<<endl;
//     Sdot[S.getn()+i]=tmp/ParticlesMass[i];
//   }
//   // for(int i=0;i<S.getn()-1;i++){
//   //   for(int j=i+1;j<S.getn();j++){
//   //     Sdot[i+S.getn()]=Sdot[i+S.getn()];
//   //     Sdot[j+S.getn()]=Sdot[j+S.getn()];
//   //   }
//   // }
//   //Springy force;
//   return Sdot;
// }


// /////////TODO: Force:
// StateVector Force(StateVector S, double t)
// {
//   StateVector Sdot(S.getn());

//   Sdot=Acceleration(Particles,t);

//   for(int i=0;i<S.getn();i++){
//     Sdot[i]=S[S.getn()+i];
//     //Sdot[i+S.getn()]=1/ParticlesMass[i]*
//   }
//   return Sdot;
// }

// StateVector Euler(StateVector S,StateVector Sdot,double t)
// {
//   StateVector K1,K2,K3,K4;
//   StateVector Snew(S.getn());
//   K1=Sdot;
//   K2=Force(S+TimeStep/2*K1,t+TimeStep/2);
//   K3=Force(S+TimeStep/2*K2,t+TimeStep/2);
//   K4=Force(S+TimeStep*K3,t+TimeStep);
//   Snew=S+TimeStep/6*(K1+2*K2+2*K3+K4);
//   //Snew=S+TimeStep*Sdot;
//   return Snew;
// }

double min(double a, double b){
  if (a<b)return a; else return b;
}

StateVector ComputeRigidDerivatives(StateVector &S, float m, Matrix3x3 I0T)
{
  StateVector Sdot;
  Sdot.setsize(TotalNum);

  Sdot.x= S.P/m;
  Matrix3x3 R= S.q.rotation();
  I0T=I0T.inv();
  //cout<<I0T<<endl;
  Matrix3x3 It= R * I0T * R.transpose();
  //cout<<"I-1\n"<<It<<endl;
  Vector3d w = S.L*I0T;

  Quaternion wq= Quaternion(w);
  //cout<<"w="<<w<<",wq="<<wq<<endl;
  Sdot.q=wq * S.q * 1/2;

  Vector3d zero(0,0,0);
  Sdot.P=zero;
  Sdot.L=zero;
  Vector3d Pi(4,4,4);
  Sdot.P=Sdot.P+AddForce;
  //Sdot.P=Sdot.P+Kij*(StartPoint-S.x);
  Vector3d r = Pi-S.x;
  Sdot.L=Sdot.L+ (r % AddForce);
  //cout<<"r:"<<r<<",AddForce:"<<AddForce<<endl;
  //cout<<"add"<<r % AddForce<<endl;

  //cout<<Sdot<<endl;
  return Sdot;
}

void Simulate()
{

//Particles Generator
  Matrix3x3 BigI(Mass/12*32,0,0,0,Mass/12*32,0,0,0,Mass/12*32);

  if (Particles.getn()==0){
    Particles.setsize(TotalNum);
    Sdot.setsize(TotalNum);
    Snew.setsize(TotalNum);
    ParticlesMass.clear();
    ParticlesMass.reserve(TotalNum);
    // Particles.add( -2, 8, -2);
    // for(int i=0;i<TotalNum;i++)Particles.add( 0, 0, 0);
    // for(int i=0;i<TotalNum;i++)
    //   ParticlesMass.push_back(Mass);
    // for(int i=0;i<2*TotalNum;i++){
    //   Sdot.add(0,0,0);
    //   Snew.add(0,0,0);
    // }
    Vector3d P0=StartPoint;
    Vector3d V0(0,0,0);
    Vector3d Orientation(1e-6,1e-6,1e-6);
    Particles.x=P0;
    Particles.velocity=V0;
    Particles.P=V0;
    Particles.L=V0;
    Particles.q=Quaternion(Orientation);
    Particles.I0=BigI;
    Particles.m=Mass;
  }
  //Sdot=Force(Particles,Time);
  //cout<<Sdot<<endl;
  //Snew=Euler(Particles,Sdot,Time);
  //cout<<Snew<<endl;
  //Particles=Snew;

  Sdot=ComputeRigidDerivatives(Particles,Particles.m,Particles.I0);
  Snew=Particles+Sdot*TimeStep;
  Snew.q=Snew.q.normalize();
  Particles=Snew;

  //cout<<Sdot<<endl;
  
  Time += TimeStep;
  NTimeSteps++;
  DrawParticles();
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
  float light0_position[] = { 0.0, 10.0, 18.0, 1.0 };
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
  DrawParticles();
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
  Vector3d tmp(0,-10,0);
  AddForce=AddForce+tmp;
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
  persp_win = glutCreateWindow("Project5");


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

