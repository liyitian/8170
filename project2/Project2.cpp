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


#define STARTX -5.0
#define STARTY 10.0
#define STARTZ -5.0
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
static int Rate;
static double Mass;
static double Viscosity;
static double TimeStep;
static double DispTime;
static double CoeffofRestitution;
static double Fraction;
static double Time = 0;
static double epsilon;
static double Lifespan;
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
			// glBegin(GL_LINES);
			// glVertex3f(i, -12, j);
			// glVertex3f(i, -12, j+2);
			// glEnd();
			// glBegin(GL_LINES);
			// glVertex3f(i, -12, j);
			// glVertex3f(i+2, -12, j);
			// glEnd();

			glBegin(GL_LINES);
			glVertex3f(i, j, -12);
			glVertex3f(i, j+2, -12);
			glEnd();
			glBegin(GL_LINES);
			glVertex3f(i, j, -12);
			glVertex3f(i+2, j, -12);
			glEnd();
			
			// glBegin(GL_LINES);
			// glVertex3f(i, j, 12);
			// glVertex3f(i, j+2, 12);
			// glEnd();
			// glBegin(GL_LINES);
			// glVertex3f(i, j, 12);
			// glVertex3f(i+2, j, 12);
			// glEnd();

			glBegin(GL_LINES);
			glVertex3f(-12, i, j);
			glVertex3f(-12, i, j+2);
			glEnd();
			glBegin(GL_LINES);
			glVertex3f(-12, i, j);
			glVertex3f(-12, i+2, j);
			glEnd();

			// glBegin(GL_LINES);
			// glVertex3f(12, i, j);
			// glVertex3f(12, i, j+2);
			// glEnd();
			// glBegin(GL_LINES);
			// glVertex3f(12, i, j);
			// glVertex3f(12, i+2, j);
			// glEnd();

			// glBegin(GL_LINES);
			// glVertex3f(i, 12, j);
			// glVertex3f(i, 12, j+2);
			// glEnd();
			// glBegin(GL_LINES);
			// glVertex3f(i, 12, j);
			// glVertex3f(i+2, 12, j);
			// glEnd();

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
  
  if(fscanf(paramfile, "%d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
            &Rate, &Lifespan, &Mass, &(V0.x), &(V0.y), &(V0.z), &Viscosity, &CoeffofRestitution,&Fraction,
            &TimeStep, &DispTime, &(Wind.x), &(Wind.y), &(Wind.z),&(G.x),&(G.y),&(G.z),&(Stone.x),&(Stone.y),&(Stone.z),&StoneRadius,&epsilon) != 22){
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
  glLoadIdentity();
  for(int i =0; i< Particles.size();i++){
    glPointSize(2);
    glBegin(GL_POINTS);
    glColor3f(ParticlesColor[i].x,ParticlesColor[i].y,ParticlesColor[i].z);
    glVertex3f(Particles[i].x, Particles[i].y, Particles[i].z);
    glEnd();
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

  Particles.reserve((int)(TimeStepsPerDisplay*Rate*2));
  ParticlesLife.reserve((int)(TimeStepsPerDisplay*Rate*2));
  ParticlesVelocity.reserve((int)(TimeStepsPerDisplay*Rate*2));
  ParticlesAcceleration.reserve((int)(TimeStepsPerDisplay*Rate*2));
  ParticlesColor.reserve((int)(TimeStepsPerDisplay*Rate*2));
  ParticlesMass.reserve((int)(TimeStepsPerDisplay*Rate*2));
  ParticlesBorntime.reserve((int)(TimeStepsPerDisplay*Rate*2));
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
  
  Particles.reserve((int)(TimeStepsPerDisplay*Rate*2));
  ParticlesLife.reserve((int)(TimeStepsPerDisplay*Rate*2));
  ParticlesVelocity.reserve((int)(TimeStepsPerDisplay*Rate*2));
  ParticlesAcceleration.reserve((int)(TimeStepsPerDisplay*Rate*2));
  ParticlesColor.reserve((int)(TimeStepsPerDisplay*Rate*2));
  ParticlesMass.reserve((int)(TimeStepsPerDisplay*Rate*2));
  ParticlesBorntime.reserve((int)(TimeStepsPerDisplay*Rate*2));
}

/*
 Compute acceleration on each particle
 */
Vector3d Accel(Vector3d currentVelocity, double mass){
  Vector3d acceleration;
  acceleration = G - (Viscosity* currentVelocity) / mass + Wind/mass ;
  //acceleration = G;
  return acceleration;
}

void Simulate()
{
  Vector3d p[6]={Vector3d(0,-12,0),Vector3d(0,12,0),
                Vector3d(12,0,0),Vector3d(-12,0,0),
  				      Vector3d(0,0,12),Vector3d(0,0,-12)};

  Vector3d nhab[6]={Vector3d(0,1,0),Vector3d(0,-1,0),
  					Vector3d(-1,0,0),Vector3d(1,0,0),
  					Vector3d(0,0,-1),Vector3d(0,0,1)};

  Vector3d acceleration;
  

  bool collisionflag[6]={false,false,false,false,false,false},totalflag=false;
  double d,bigD,s[6];
  int index[6];


  Vector3d Velocity;
  Vector3d Position;
  Vector3d Color;
  /////ParticlesGenerator
  //if (Time==0){
  //ParticlesVelocity
  //ParticlesColor;
  //<double> ParticlesLife;
  //<double> ParticlesMass;
  for(int i=0;i<Rate;i++){

    ParticlesBorntime.push_back(Time);

    Position.x=STARTX+((double)rand()/(RAND_MAX)*epsilon*2);
    Position.y=STARTY+((double)rand()/(RAND_MAX)*epsilon/100);
    Position.z=STARTZ+((double)rand()/(RAND_MAX)*epsilon*2);
    Particles.push_back(Position);
    
    Velocity.x=gauss(V0.x,epsilon/2,Time);
    Velocity.y=gauss(V0.y,epsilon/2,Time);
    Velocity.z=gauss(V0.z,epsilon/2,Time);
    ParticlesVelocity.push_back(Velocity);
    
    ParticlesLife.push_back(abs(gauss(Lifespan,epsilon,Time)));
    
    Color.x=abs(gauss(1,epsilon/100,Time));
    Color.y=abs(gauss(1,epsilon/100,Time));
    Color.z=1;//abs(gauss(1,epsilon/10,Time));
    ParticlesColor.push_back(Color);

    ParticlesMass.push_back(abs(gauss(Mass,epsilon,Time)));


    acceleration=G;//Accel(ParticlesVelocity[i],ParticlesMass[i]);
    //cout<<acceleration<<endl;
    ParticlesAcceleration.push_back(acceleration);
  }

  //ParticleKiller

  for(int i=0;i<Particles.size();i++){
    if (Time-ParticlesBorntime[i]>=ParticlesLife[i]){
      Particles.erase(Particles.begin()+i);
      ParticlesLife.erase(ParticlesLife.begin()+i);
      ParticlesVelocity.erase(ParticlesVelocity.begin()+i);
      ParticlesAcceleration.erase(ParticlesAcceleration.begin()+i);
      ParticlesColor.erase(ParticlesColor.begin()+i);
      ParticlesMass.erase(ParticlesMass.begin()+i);
      ParticlesBorntime.erase(ParticlesBorntime.begin()+i);
    }
    else{
      //double colordistribution=1-((Time-ParticlesBorntime[i])/ParticlesLife[i]);
      ParticlesColor[i].x*=1-1e-2;
      ParticlesColor[i].y*=1-1e-2;
      //cout<<ParticlesColor[i]<<endl;
    }
  }

  // for(int i=0;i<Particles.size();i++){
  //   ParticlesAcceleration[i]=Accel(ParticlesVelocity[i],ParticlesMass[i]);
  // }

  //ParticleVelecity Updater
  ///some magic acceleration
  Vector3d PreviousVelocity;
  Vector3d PreviousPosition;
  for(int i=0;i<Particles.size();i++){

    ParticlesAcceleration[i]=G - (Viscosity* ParticlesVelocity[i]) / ParticlesMass[i] + Wind/ParticlesMass[i] ;
    
    //cout<<ParticlesAcceleration[i]<<endl;

    PreviousVelocity=ParticlesVelocity[i];
    PreviousPosition=Particles[i];
    ParticlesVelocity[i]=PreviousVelocity+TimeStep*ParticlesAcceleration[i];
    Particles[i]=PreviousPosition+TimeStep*PreviousVelocity;

    // if (Particles[i].y<=-12){
    //   double s;
    //   Vector3d Vc,VcPrime,Pc;
    //   s=(Particles[i].y+12)/(Particles[i].y-PreviousPosition.y);
    //   if (Pc.y<-12) Pc.y=-12;
    //   VcPrime=Vector3d(Vc.x*(1-Fraction),-Vc.y*CoeffofRestitution,Vc.z*(1-Fraction));
    //   //VcPrime=Vc
    //   ParticlesVelocity[i]=VcPrime+TimeStep*(1-s)*ParticlesAcceleration[i];
    //   Particles[i]=Pc+TimeStep*(1-s)*VcPrime;
    //   ParticlesColor[i]=Vector3d(1,1,1);
    //   continue;
    // }
CHECK:
    //Sphere collision
    Vector3d PretoNow=Particles[i]-PreviousPosition;
    Vector3d PretoStone=Stone-PreviousPosition;
    Vector3d NowtoStone=Stone-Particles[i];
    Vector3d Project=PretoNow*Stone;
    double Distance=PretoStone.normsqr()-Project*Project;
    if (sqrt(Distance)<StoneRadius&&NowtoStone.norm()<StoneRadius){
      Vector3d PretoCollision=(Project-sqrt(StoneRadius*StoneRadius-Distance))*PretoNow;
      double s = PretoCollision.norm()/PretoNow.norm();
      Vector3d Vc,Pc;
      Vc=PreviousVelocity+TimeStep*s*ParticlesAcceleration[i];
      Pc=PreviousPosition+TimeStep*s*PreviousVelocity;
      
      Vector3d Vr,Vt,StonetoCollision,Vs,VcPrime;
      
      StonetoCollision=(Pc-Stone).normalize();
      Vr=StonetoCollision*(Vc*StonetoCollision);
      Vt=Vc-Vr;
      //VcPrime=(1-Fraction)*Vt;
      VcPrime =(1-Fraction)*Vt-CoeffofRestitution*Vr;
      //cout<<Vc<<"   "<<VcPrime<<endl;

      ParticlesVelocity[i]=VcPrime+TimeStep*(1-s)*ParticlesAcceleration[i];
      Particles[i]=Pc+TimeStep*(1-s)*VcPrime;
      ParticlesColor[i]=Vector3d(1,1,1);

      PreviousVelocity=ParticlesVelocity[i];
      PreviousPosition=Particles[i];

      goto CHECK;

      // Particles.erase(Particles.begin()+i);
      // ParticlesLife.erase(ParticlesLife.begin()+i);
      // ParticlesVelocity.erase(ParticlesVelocity.begin()+i);
      // ParticlesAcceleration.erase(ParticlesAcceleration.begin()+i);
      // ParticlesColor.erase(ParticlesColor.begin()+i);
      // ParticlesMass.erase(ParticlesMass.begin()+i);
      // ParticlesBorntime.erase(ParticlesBorntime.begin()+i);
    }

    if (Particles[i].y<=-12){
      double s;
      Vector3d Vc,VcPrime,Pc;
      s=(Particles[i].y+12)/(Particles[i].y-PreviousPosition.y);
      Vc=PreviousVelocity+TimeStep*s*ParticlesAcceleration[i];
      Pc=PreviousPosition+TimeStep*s*PreviousVelocity;
      if (Pc.y<-12) Pc.y=-12;
      VcPrime=Vector3d(Vc.x*(1-Fraction),-Vc.y*CoeffofRestitution,Vc.z*(1-Fraction));
      //VcPrime=Vc
      ParticlesVelocity[i]=VcPrime+TimeStep*(1-s)*ParticlesAcceleration[i];
      Particles[i]=Pc+TimeStep*(1-s)*VcPrime;
      ParticlesColor[i]=Vector3d(1,1,1);
      continue;
    }
  }

//}


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
  //glLoadIdentity();

  glBegin(GL_POINTS);
  //glColor4f(0.0, 0.0, 0.0,1.0);
  for(int i =0; i< Particles.size();i++){
    glColor3f(ParticlesColor[i].x,ParticlesColor[i].y,ParticlesColor[i].z);
    glVertex3f(Particles[i].x, Particles[i].y, Particles[i].z);
  }
  glEnd();
   if (showGrid) 
    makeGrid();
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
  //glDepthRange(0.0, 1.0);
  //glEnable(GL_LIGHTING);
  //glEnable(GL_PROGRAM_POINT_SIZE);
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

  glutMainLoop();
  return(0);
}

