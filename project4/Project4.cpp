//
// Project4.cpp
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


#define STARTX -10.0
#define STARTY -10.0
#define STARTZ -10.0
#define radius 1.0
#define MAXSTEPS  10000
#define PRINTTEST cout<<"here"<<endl
int WIDTH = 1024;
int HEIGHT = 765;

int persp_win;

Camera *camera;

bool showGrid = false;
bool Stopped =true;

static Vector3d V0;
static Vector3d Wind;
static Vector3d G;
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
static int TotalNum;
static double Mass;
static double TimeStep;
static double DispTime;
static double Kij;
static double Time = 0;
static double epsilon;
static double l0 = 1;
static int TimerDelay;
static int TimeStepsPerDisplay;
static int NSteps = 0;
static int NTimeSteps = -1;
static int Collision[MAXSTEPS];
bool resting=false;
bool restingflag[6]={true,true,true,true,true,true},totalresting=false;

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
  
  if(fscanf(paramfile, "%d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
            &TotalNum, &Mass, &Kij, &TimeStep, &DispTime, &(Wind.x), &(Wind.y), &(Wind.z),&(G.x),&(G.y),&(G.z),&epsilon) != 12){
    fprintf(stderr, "error reading parameter file %s\n", filename);
    exit(1);
  }
  
  fclose(paramfile);
  
  TimeStepsPerDisplay = Max(1, int(DispTime / TimeStep + 0.5));
  TimerDelay = int(0.5 * TimeStep * 1000);
}


void DrawParticles()
{
  
  glClear(GL_COLOR_BUFFER_BIT);
  
  //TODO:Draw the CUBE
  glLoadIdentity();
  


  for(int i=0;i<Particles.getn()-1;i++){
    if(i % TotalNum!=TotalNum-1&&i +TotalNum<TotalNum*TotalNum){
    //glDisable(GL_LIGHTING);

    glBegin(GL_TRIANGLES);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(Particles[i].x,Particles[i].y,Particles[i].z);
    glVertex3f(Particles[i+1].x,Particles[i+1].y,Particles[i+1].z);
    glVertex3f(Particles[i+TotalNum].x,Particles[i+TotalNum].y,Particles[i+TotalNum].z);
    glEnd();
    glBegin(GL_TRIANGLES);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(Particles[i+1+TotalNum].x,Particles[i+1+TotalNum].y,Particles[i+1+TotalNum].z);
    glVertex3f(Particles[i+1].x,Particles[i+1].y,Particles[i+1].z);
    glVertex3f(Particles[i+TotalNum].x,Particles[i+TotalNum].y,Particles[i+TotalNum].z);
    glEnd();
    //glEnable(GL_LIGHTING);
    }
    // if(i +TotalNum<TotalNum*TotalNum){
    // glBegin(GL_LINES);
    // glVertex3f(Particles[i].x,Particles[i].y,Particles[i].z);

    // glEnd();
    // }
    
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
  //TODO:Vertex clear,& reserve the room;
  Particles.setsize(TotalNum*TotalNum);
  Particles.setn(0);
  Sdot.setsize(TotalNum*TotalNum);
  Snew.setsize(TotalNum*TotalNum);
  ParticlesMass.clear();
  ParticlesMass.reserve(TotalNum*TotalNum);
  NTimeSteps = -1;

  glutIdleFunc(NULL);
  Time = 0;


  DrawParticles();
}

/*
 Initialize the Simulation
 */
void InitSimulation(int argc, char* argv[]){
  
  if(argc != 2){
    fprintf(stderr, "usage: Project4 paramfile\n");
    exit(1);
  }
  
  LoadParameters(argv[1]);
  Particles.setsize(TotalNum*TotalNum);
  Particles.setn(0);

  NSteps = 0;
  NTimeSteps = -1;
  Time = 0;
  
  //TODO:Vertex clear
}



//TODO: Acceleration
StateVector Acceleration(StateVector S, double t)
{
  StateVector Sdot(S.getn());
  Vector3d fij;
  Vector3d tmp(0,0,0);
  for(int i=0;i<S.getn();i++){
    //cout<<S.getn()+i<<endl;
    Sdot[S.getn()+i]=tmp/ParticlesMass[i];
  }

  //Springy force;
  for(int i=0;i<S.getn()-1;i++){
    if(i % TotalNum!=TotalNum-1){
      Vector3d Xijhab=(Particles[i+1]-Particles[i]).normalize();
      double dij=(Particles[i+1]-Particles[i]).norm();
      Vector3d Vij=Particles[i+1+S.getn()]-Particles[i+S.getn()];
      Vector3d fis=Kij*(dij-l0)*Xijhab;
      Vector3d fid=dij*(Vij*Xijhab)*Xijhab;
      fij=fis+fid;
      Sdot[S.getn()+i]=Sdot[S.getn()+i]+fij/ParticlesMass[i];
      Sdot[S.getn()+i+1]=Sdot[S.getn()+i+1]-fij/ParticlesMass[i];
    }
    if(i +TotalNum<TotalNum*TotalNum){
      int j=i+TotalNum;
      Vector3d Xijhab=(Particles[j]-Particles[i]).normalize();
      double dij=(Particles[j]-Particles[i]).norm();
      Vector3d Vij=Particles[j+S.getn()]-Particles[i+S.getn()];
      Vector3d fis=Kij*(dij-l0)*Xijhab;
      Vector3d fid=dij*(Vij*Xijhab)*Xijhab;
      fij=fis+fid;
      Sdot[S.getn()+i]=Sdot[S.getn()+i]+fij/ParticlesMass[i];
      Sdot[S.getn()+j]=Sdot[S.getn()+j]-fij/ParticlesMass[i];
    }
   

  }
  return Sdot;
}


/////////TODO: Force:
StateVector Force(StateVector S, double t)
{
  StateVector Sdot(S.getn());

  Sdot=Acceleration(Sdot,t);

  for(int i=0;i<S.getn();i++){
    Sdot[i]=S[S.getn()+i];
    //Sdot[i+S.getn()]=1/ParticlesMass[i]*
  }
  return Sdot;
}

StateVector Euler(StateVector S,StateVector Sdot,double t)
{
  StateVector K1,K2,K3,K4;
  StateVector Snew(S.getn());
  K1=Sdot;
  K2=Force(S+TimeStep/2*K1,t+TimeStep/2);
  K3=Force(S+TimeStep/2*K2,t+TimeStep/2);
  K4=Force(S+TimeStep*K3,t+TimeStep);
  Snew=S+TimeStep/6*(K1+2*K2+2*K3+K4);
  return Snew;
}

double min(double a, double b){
  if (a<b)return a; else return b;
}

void Simulate()
{

  //Particles Generator
  //cout<<Particles.getn()<<endl;
  if (Particles.getn()!=TotalNum*TotalNum){
    //cout<<TotalNum<<endl;
    Particles.setsize(TotalNum*TotalNum);
    //Particles.setn(0);
    Sdot.setsize(TotalNum*TotalNum);
    Snew.setsize(TotalNum*TotalNum);
    ParticlesMass.clear();
    ParticlesMass.reserve(TotalNum*TotalNum);
    for(int i=0;i<TotalNum;i++){
      for(int j=0;j<TotalNum;j++){
        
        Particles[i*TotalNum+j].set(-10+i,0,-10+j);
        ParticlesMass.push_back(Mass);
        
      }
    }
  }

  //cout<<TotalNum<<endl;
  Sdot=Force(Particles,Time);
  Snew=Euler(Particles,Sdot,Time);
  Particles=Snew;


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
  float light0_diffuse[] = { 0.3, 0.3, 0.3, 1.0 }; 
  float light0_specular[] = { 0.3, 0.3, 0.3, 1.0 }; 
  float light0_position[] = { 0.0, 1.0, 8.0, 1.0 };
  float light0_direction[] = { -1.0, -1.0, -1.0, 1.0};
  float lmodel_ambient[] = { 0.2, 0.2, 0.2, 1.0 };



  glLightfv(GL_LIGHT0,GL_AMBIENT,light0_ambient); 
  glLightfv(GL_LIGHT0,GL_DIFFUSE,light0_diffuse); 
  glLightfv(GL_LIGHT0,GL_SPECULAR,light0_specular); 
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

  glLightfv(GL_LIGHT0,GL_POSITION,light0_position);
  glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,light0_direction);
  
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
}

void do_material()
{
float mat_ambient[] = {1.0,1.0,1.0,1.0};
float mat_specular[] = {1.0,1.0,1.0,1.0};
float mat_emission[] = {0.0,0.0,0.0,1.0};
float low_shininess[] = { 128.0 };



glEnable(GL_COLOR_MATERIAL);
glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_ambient);
glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
glMaterialfv(GL_FRONT,GL_EMISSION,mat_emission);
glMaterialfv(GL_FRONT,GL_SHININESS,low_shininess);
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

void hit(){
  if (Particles.getn()==TotalNum*TotalNum){
    Particles[190+Particles.getn()].set(0,0.5,0);
  }
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
  persp_win = glutCreateWindow("Project2");


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

