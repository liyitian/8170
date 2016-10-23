//
// Project2.cpp
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
int WIDTH = 800;
int HEIGHT = 600;

int persp_win;

Camera *camera;

bool showGrid = true;
bool Stopped =true;

static Vector3d V0;
static Vector3d Wind;
static Vector3d G;
static Vector3d Stone;
static double StoneRadius;
static std::vector<Vector3d> Particles;
static std::vector<Vector3d> ParticlesVelocity;
static std::vector<Vector3d> ParticlesColor;
static std::vector<Vector3d> ParticlesAcceleration;
static std::vector<double> ParticlesLife;
static std::vector<double> ParticlesMass;
static std::vector<double> ParticlesBorntime;

static char *ParamFilename = NULL;
static int TotalNum;
static double Mass;
static double Viscosity;
static double TimeStep;
static double DispTime;
static double CoeffofRestitution;
static double Fraction;
static double Time = 0;
static double epsilon;
static double Ka;
static double Kv;
static double Kc;
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
  
  if(fscanf(paramfile, "%d %lf %lf  %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
            &TotalNum, &Ka, &Kv, &Kc, &Mass, &(V0.x), &(V0.y), &(V0.z), &Viscosity, &CoeffofRestitution,&Fraction,
            &TimeStep, &DispTime, &(Wind.x), &(Wind.y), &(Wind.z),&(G.x),&(G.y),&(G.z),&(Stone.x),&(Stone.y),&(Stone.z),&StoneRadius,&epsilon) != 24){
    fprintf(stderr, "error reading parameter file %s\n", filename);
    exit(1);
  }
  
  fclose(paramfile);
  
  TimeStepsPerDisplay = Max(1, int(DispTime / TimeStep + 0.5));
  TimerDelay = int(0.5 * TimeStep * 1000);
}
void do_lights()
{
float light0_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
float light0_diffuse[] = { 1.0, 1.0, 1.0, 1.0 }; 
float light0_specular[] = { 1.0, 1.0, 1.0, 1.0 }; 
float light0_position[] = { 15.0, 15.0, 15.0, 0.0 };
float light0_direction[] = { -1.0, -1.0, -1.0, 1.0};

// set scene default ambient 
//glLightModelfv(GL_LIGHT_MODEL_AMBIENT,light0_ambient); 

// make specular correct for spots 
//glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,1); 
glLightfv(GL_LIGHT0,GL_AMBIENT,light0_ambient); 
glLightfv(GL_LIGHT0,GL_DIFFUSE,light0_diffuse); 
glLightfv(GL_LIGHT0,GL_SPECULAR,light0_specular); 
//glLightf(GL_LIGHT0,GL_SPOT_EXPONENT,1.0); 
//glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,180.0); 
//glLightf(GL_LIGHT0,GL_CONSTANT_ATTENUATION,1.0); 
//glLightf(GL_LIGHT0,GL_LINEAR_ATTENUATION,0.2); 
//glLightf(GL_LIGHT0,GL_QUADRATIC_ATTENUATION,0.01); 
glLightfv(GL_LIGHT0,GL_POSITION,light0_position);
glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,light0_direction);
glEnable(GL_LIGHT0);
glEnable(GL_LIGHTING);
}

void DrawParticles()
{
  
  glClear(GL_COLOR_BUFFER_BIT);
  //glEnable(GL_LIGHTING);
  for(int i =0; i< Particles.size();i++){
    glLoadIdentity();
    glTranslatef(Particles[i].x,Particles[i].y,Particles[i].z);
    glColor3f(ParticlesColor[i].x, ParticlesColor[i].y, ParticlesColor[i].z);
    Vector3d Zaxis;
    Zaxis.x=0.0;Zaxis.y=0.0;Zaxis.z=1.0;
    Vector3d RotateAxis=(ParticlesVelocity[i]%Zaxis).normalize();
    glRotatef(-acos(ParticlesVelocity[i].normalize()*Zaxis)* 180.0 / 3.1415926,RotateAxis.x,RotateAxis.y,RotateAxis.z);
    //glRotatef(90,0,1,0);
    //std::cout<<RotateAxis<<endl;
    glutSolidCone(0.25, 0.5, 20, 4); 
    //glTranslatef(-Particles[i].x,-Particles[i].y,-Particles[i].z);
  }
  //glutSwapBuffers();
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
  Particles.clear();
  ParticlesLife.clear();
  ParticlesVelocity.clear();
  ParticlesAcceleration.clear();
  ParticlesColor.clear();
  ParticlesMass.clear();
  ParticlesBorntime.clear();

  Particles.reserve((int)(TimeStepsPerDisplay*TotalNum));
  ParticlesLife.reserve((int)(TimeStepsPerDisplay*TotalNum));
  ParticlesVelocity.reserve((int)(TimeStepsPerDisplay*TotalNum));
  ParticlesAcceleration.reserve((int)(TimeStepsPerDisplay*TotalNum));
  ParticlesColor.reserve((int)(TimeStepsPerDisplay*TotalNum));
  ParticlesMass.reserve((int)(TimeStepsPerDisplay*TotalNum));
  ParticlesBorntime.reserve((int)(TimeStepsPerDisplay*TotalNum));
  DrawParticles();
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
  
  Particles.reserve((int)(TimeStepsPerDisplay*TotalNum));
  ParticlesLife.reserve((int)(TimeStepsPerDisplay*TotalNum));
  ParticlesVelocity.reserve((int)(TimeStepsPerDisplay*TotalNum));
  ParticlesAcceleration.reserve((int)(TimeStepsPerDisplay*TotalNum));
  ParticlesColor.reserve((int)(TimeStepsPerDisplay*TotalNum));
  ParticlesMass.reserve((int)(TimeStepsPerDisplay*TotalNum));
  ParticlesBorntime.reserve((int)(TimeStepsPerDisplay*TotalNum));
}

/*
 Compute acceleration on each particle
 */
Vector3d Accel(Vector3d currentVelocity, double valve){
  Vector3d acceleration;
  //acceleration = G - (Viscosity* currentVelocity) / mass + Wind/mass ;
  //acceleration = G;
  if (acceleration.x>valve)acceleration.x=valve;
  if (acceleration.y>valve)acceleration.y=valve;
  if (acceleration.z>valve)acceleration.z=valve;
  if (acceleration.x<-valve)acceleration.x=-valve;
  if (acceleration.y<-valve)acceleration.y=-valve;
  if (acceleration.z<-valve)acceleration.z=-valve;
  return acceleration;
}
double min(double a, double b){
  if (a<b)return a; else return b;
}

void Simulate()
{

  Vector3d acceleration;
  

  bool collisionflag[6]={false,false,false,false,false,false},totalflag=false;
  double d,bigD,s[6];
  int index[6];


  Vector3d Velocity;
  Vector3d Position;
  Vector3d Color;
  /////ParticlesGenerator

  for(int i=0;i<TotalNum-Particles.size();i++){

    ParticlesBorntime.push_back(Time);

    Position.x=STARTX+((double)rand()/(RAND_MAX)*epsilon*30);
    Position.y=STARTY+((double)rand()/(RAND_MAX)*epsilon*30);
    Position.z=STARTZ+((double)rand()/(RAND_MAX)*epsilon*30);
    Particles.push_back(Position);
    
    Velocity.x=gauss(V0.x,epsilon*5,Time);
    Velocity.y=gauss(V0.y,epsilon*5,Time);
    Velocity.z=gauss(V0.z,epsilon*5,Time);
    ParticlesVelocity.push_back(Velocity);
    
    //ParticlesLife.push_back(abs(gauss(Lifespan,epsilon,Time)));
    
    Color.x=((double)rand()/(RAND_MAX)*epsilon/100);
    Color.y=((double)rand()/(RAND_MAX)*epsilon/100);
    Color.z=((double)rand()/(RAND_MAX)*epsilon/100);
    ParticlesColor.push_back(Color);

    ParticlesMass.push_back(abs(gauss(Mass,epsilon,Time)));


    acceleration=G;
    ParticlesAcceleration.push_back(acceleration);
  }

  //ParticleKiller

  //for(int i=0;i<Particles.size();i++){
    // if (Time-ParticlesBorntime[i]>=ParticlesLife[i]){
    //   Particles.erase(Particles.begin()+i);
    //   ParticlesLife.erase(ParticlesLife.begin()+i);
    //   ParticlesVelocity.erase(ParticlesVelocity.begin()+i);
    //   ParticlesAcceleration.erase(ParticlesAcceleration.begin()+i);
    //   ParticlesColor.erase(ParticlesColor.begin()+i);
    //   ParticlesMass.erase(ParticlesMass.begin()+i);
    //   ParticlesBorntime.erase(ParticlesBorntime.begin()+i);
    // }
    // else{
   //   ParticlesColor[i].x*=1-1e-2;
  //    ParticlesColor[i].y*=1-1e-2;
    //}
  //}

  
  //ParticleVelecity Updater
  Vector3d PreviousVelocity;
  Vector3d PreviousPosition;

  double r1=2,r2=4;

  for(int i=0;i<Particles.size();i++)
  {
    Vector3d Aavoid=G;
    Vector3d Avm=G;
    Vector3d Acenter=G;
    for(int j=0;j<Particles.size();j++){
      Vector3d dist=(Particles[j]-Particles[i]);
      if (i==j||dist.norm()<1.5)continue;

      double Kd;
      if(dist.norm()<r1)Kd=1.0;
        else if(dist.norm()<r2&&dist.norm()>=r1)Kd=(r2-dist.norm())/(r2-r1);
      else Kd=0;
      Aavoid=Aavoid-(Ka/dist.norm()*Kd)*dist.normalize();
      //if (dist.norm()<1e-2)cout<<Aavoid<<endl;
      //Aavoid=Accel(Aavoid,10.0);
      Avm=Avm+Kv*(ParticlesVelocity[j]-ParticlesVelocity[i])*Kd;
      //if (dist.norm()<r1)cout<<Avm<<endl;;
      //Avm=Accel(Avm,20.0);
      Acenter=Acenter+Kc*dist*Kd;
    }
    // double ar=50.0;
    // Vector3d ai;
    // ai=min(ar,Aavoid.norm())*Aavoid.normalize();
    // ar=50.0-ai.norm();

    // ai=ai+min(ar,Avm.norm())*Avm.normalize();
    // ar=50.0-ai.norm();

    // ai=ai+min(ar,Acenter.norm())*Acenter.normalize();
        
    ParticlesAcceleration[i]=Aavoid+Avm+Acenter;    
  }

  //cout<<ParticlesAcceleration[0]<<endl;

  for(int i=0;i<Particles.size();i++){

    //ParticlesAcceleration[i]=G - (Viscosity* ParticlesVelocity[i]) / ParticlesMass[i] + Wind/ParticlesMass[i] ;
  
    PreviousVelocity=ParticlesVelocity[i];
    PreviousPosition=Particles[i];
    ParticlesVelocity[i]=PreviousVelocity+TimeStep*ParticlesAcceleration[i];
    Particles[i]=PreviousPosition+TimeStep*PreviousVelocity;

// CHECK:
//     //Sphere collision
//     Vector3d PretoNow=Particles[i]-PreviousPosition;
//     Vector3d PretoStone=Stone-PreviousPosition;
//     Vector3d NowtoStone=Stone-Particles[i];
//     Vector3d Project=PretoNow*Stone;
//     double Distance=PretoStone.normsqr()-Project*Project;
//     if (sqrt(Distance)<StoneRadius&&NowtoStone.norm()<StoneRadius){
//       Vector3d PretoCollision=(Project-sqrt(StoneRadius*StoneRadius-Distance))*PretoNow;
//       double s = PretoCollision.norm()/PretoNow.norm();
//       Vector3d Vc,Pc;
//       Vc=PreviousVelocity+TimeStep*s*ParticlesAcceleration[i];
//       Pc=PreviousPosition+TimeStep*s*PreviousVelocity;
      
//       Vector3d Vr,Vt,StonetoCollision,Vs,VcPrime;
      
//       StonetoCollision=(Pc-Stone).normalize();
//       Vr=StonetoCollision*(Vc*StonetoCollision);
//       Vt=Vc-Vr;
//       VcPrime =(1-Fraction)*Vt-CoeffofRestitution*Vr;

//       ParticlesVelocity[i]=VcPrime+TimeStep*(1-s)*ParticlesAcceleration[i];
//       Particles[i]=Pc+TimeStep*(1-s)*VcPrime;
//       ParticlesColor[i]=Vector3d(1,1,1);

//       PreviousVelocity=ParticlesVelocity[i];
//       PreviousPosition=Particles[i];

//       goto CHECK;

//     }

    if (Particles[i].y<=-12){
      double s;
      Vector3d Vc,VcPrime,Pc;
      s=(Particles[i].y+12)/(Particles[i].y-PreviousPosition.y);
      Vc=PreviousVelocity+TimeStep*s*ParticlesAcceleration[i];
      Pc=PreviousPosition+TimeStep*s*PreviousVelocity;
      if (Pc.y<-12) Pc.y=-12;
      VcPrime=Vector3d(Vc.x*(1-Fraction),-Vc.y*CoeffofRestitution,Vc.z*(1-Fraction));
      ParticlesVelocity[i]=VcPrime+TimeStep*(1-s)*ParticlesAcceleration[i];
      Particles[i]=Pc+TimeStep*(1-s)*VcPrime;
      ParticlesColor[i]=Vector3d(1,1,1);
      //continue;
    }
    if (Particles[i].y>=12){
      double s;
      Vector3d Vc,VcPrime,Pc;
      s=(Particles[i].y-12)/(Particles[i].y-PreviousPosition.y);
      Vc=PreviousVelocity+TimeStep*s*ParticlesAcceleration[i];
      Pc=PreviousPosition+TimeStep*s*PreviousVelocity;
      if (Pc.y>12) Pc.y=12;
      VcPrime=Vector3d(Vc.x*(1-Fraction),-Vc.y*CoeffofRestitution,Vc.z*(1-Fraction));
      ParticlesVelocity[i]=VcPrime+TimeStep*(1-s)*ParticlesAcceleration[i];
      Particles[i]=Pc+TimeStep*(1-s)*VcPrime;
      ParticlesColor[i]=Vector3d(1,1,1);
    }
    // if (Particles[i].x>=12){
    //   double s;
    //   Vector3d Vc,VcPrime,Pc;
    //   s=(Particles[i].x-12)/(Particles[i].y-PreviousPosition.y);
    //   Vc=PreviousVelocity+TimeStep*s*ParticlesAcceleration[i];
    //   Pc=PreviousPosition+TimeStep*s*PreviousVelocity;
    //   if (Pc.x>12) Pc.x=12;
    //   VcPrime=Vector3d(Vc.x*(1-Fraction),-Vc.y*CoeffofRestitution,Vc.z*(1-Fraction));
    //   ParticlesVelocity[i]=VcPrime+TimeStep*(1-s)*ParticlesAcceleration[i];
    //   Particles[i]=Pc+TimeStep*(1-s)*VcPrime;
    //   ParticlesColor[i]=Vector3d(1,1,1);
    // }
    // if (Particles[i].x<=-12){
    //   double s;
    //   Vector3d Vc,VcPrime,Pc;
    //   s=(Particles[i].x+12)/(Particles[i].y-PreviousPosition.y);
    //   Vc=PreviousVelocity+TimeStep*s*ParticlesAcceleration[i];
    //   Pc=PreviousPosition+TimeStep*s*PreviousVelocity;
    //   if (Pc.x<-12) Pc.x=-12;
    //   VcPrime=Vector3d(Vc.x*(1-Fraction),-Vc.y*CoeffofRestitution,Vc.z*(1-Fraction));
    //   ParticlesVelocity[i]=VcPrime+TimeStep*(1-s)*ParticlesAcceleration[i];
    //   Particles[i]=Pc+TimeStep*(1-s)*VcPrime;
    //   ParticlesColor[i]=Vector3d(1,1,1);
    //   //continue;
    // }
    // if (Particles[i].z>=12){
    //   double s;
    //   Vector3d Vc,VcPrime,Pc;
    //   s=(Particles[i].z-12)/(Particles[i].y-PreviousPosition.y);
    //   Vc=PreviousVelocity+TimeStep*s*ParticlesAcceleration[i];
    //   Pc=PreviousPosition+TimeStep*s*PreviousVelocity;
    //   if (Pc.z>12) Pc.z=12;
    //   VcPrime=Vector3d(Vc.z*(1-Fraction),-Vc.y*CoeffofRestitution,Vc.z*(1-Fraction));
    //   ParticlesVelocity[i]=VcPrime+TimeStep*(1-s)*ParticlesAcceleration[i];
    //   Particles[i]=Pc+TimeStep*(1-s)*VcPrime;
    //   ParticlesColor[i]=Vector3d(1,1,1);
    // }
    // if (Particles[i].z<=-12){
    //   double s;
    //   Vector3d Vc,VcPrime,Pc;
    //   s=(Particles[i].z+12)/(Particles[i].y-PreviousPosition.y);
    //   Vc=PreviousVelocity+TimeStep*s*ParticlesAcceleration[i];
    //   Pc=PreviousPosition+TimeStep*s*PreviousVelocity;
    //   if (Pc.z<-12) Pc.z=-12;
    //   VcPrime=Vector3d(Vc.z*(1-Fraction),-Vc.y*CoeffofRestitution,Vc.z*(1-Fraction));
    //   ParticlesVelocity[i]=VcPrime+TimeStep*(1-s)*ParticlesAcceleration[i];
    //   Particles[i]=Pc+TimeStep*(1-s)*VcPrime;
    //   ParticlesColor[i]=Vector3d(1,1,1);
    //   //continue;
    // }
  }

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



void do_material()
{
float mat_ambient[] = {0.1,0.1,0.1,1.0}; 
float mat_diffuse[] = {0.4,0.4,0.4,1.0}; 
float mat_specular[] = {0.4,0.4,0.4,1.0};
float mat_shininess[] = {1.0}; 

glMaterialfv(GL_FRONT,GL_AMBIENT,mat_ambient);
glMaterialfv(GL_FRONT,GL_DIFFUSE,mat_diffuse);
glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess);

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
      glutTimerFunc(1,TimerCallback,0);
      Stopped=!Stopped;
    }
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
  glutInitWindowPosition(50, 50);
  persp_win = glutCreateWindow("Project2");


  // initialize the camera and such
  init();
  InitSimulation(argc, argv);
  
  // set up opengl callback functions
  glutDisplayFunc(PerspDisplay);
  glutMouseFunc(mouseEventHandler);
  glutMotionFunc(motionEventHandler);
  glutKeyboardFunc(keyboardEventHandler);

  do_material();
  do_lights();

  glutMainLoop();
  return(0);
}

