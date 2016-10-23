//
// Project1.cpp
//
// 
// 
// Christopher Root, 2006
// Minor Modifications by Donald House, 2009
// Minor Modifications by Yujie Shu, 2012
// Minor Modifications by Yitian Li, 2016
//

#include "Camera.h"

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#define STARTX -5.0
#define STARTY 0.0
#define STARTZ -10.0
#define radius 1.0
#define MAXSTEPS  10000
int WIDTH = 800;
int HEIGHT = 600;

int persp_win;

Camera *camera;

bool showGrid = true;
bool Stopped =true;
bool Status;

static Vector3d V0;
static Vector3d Wind;
static Vector3d G(0, -10.0, 0);
static Vector3d Ball(STARTX, STARTY, STARTZ);
static Vector3d Velocity(0, 0, 0);
static Vector3d OldBall[MAXSTEPS];


static char *ParamFilename = NULL;
static double Mass;
static double Viscosity;
static double TimeStep;
static double DispTime;
static double CoeffofRestitution;
static double Fraction;
static double Time = 0;
static double epsilon;
static int TimerDelay;
static int TimeStepsPerDisplay;
static int NSteps = 0;
static int NTimeSteps = -1;
static int Collision[MAXSTEPS];
static int mysphereID;
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
			glVertex3f(i, -12, j);
			glVertex3f(i, -12, j+2);
			glEnd();
			glBegin(GL_LINES);
			glVertex3f(i, -12, j);
			glVertex3f(i+2, -12, j);
			glEnd();

			//if (j>=0){
				glBegin(GL_LINES);
				glVertex3f(i, j, -12);
				glVertex3f(i, j+2, -12);
				glEnd();
				glBegin(GL_LINES);
				glVertex3f(i, j, -12);
				glVertex3f(i+2, j, -12);
				glEnd();
				
				glBegin(GL_LINES);
				glVertex3f(i, j, 12);
				glVertex3f(i, j+2, 12);
				glEnd();
				glBegin(GL_LINES);
				glVertex3f(i, j, 12);
				glVertex3f(i+2, j, 12);
				glEnd();

			//}

			//if (i>=0){
				glBegin(GL_LINES);
				glVertex3f(-12, i, j);
				glVertex3f(-12, i, j+2);
				glEnd();
				glBegin(GL_LINES);
				glVertex3f(-12, i, j);
				glVertex3f(-12, i+2, j);
				glEnd();

				glBegin(GL_LINES);
				glVertex3f(12, i, j);
				glVertex3f(12, i, j+2);
				glEnd();
				glBegin(GL_LINES);
				glVertex3f(12, i, j);
				glVertex3f(12, i+2, j);
				glEnd();
			//}


			glBegin(GL_LINES);
			glVertex3f(i, 12, j);
			glVertex3f(i, 12, j+2);
			glEnd();
			glBegin(GL_LINES);
			glVertex3f(i, 12, j);
			glVertex3f(i+2, 12, j);
			glEnd();

			glBegin(GL_LINES);
			glVertex3f(i, -12, j);
			glVertex3f(i, -12, j+2);
			glEnd();
			glBegin(GL_LINES);
			glVertex3f(i, -12, j);
			glVertex3f(i+2, -12, j);
			glEnd();



			if (j == 10){
				glBegin(GL_LINES);
				glVertex3f(i, 0, j+2);
				glVertex3f(i+2, 0, j+2);
				glEnd();

					glBegin(GL_LINES);
					glVertex3f(12, i, j+2);
					glVertex3f(12, i+2, j+2);
					glEnd();
				
				glBegin(GL_LINES);
				glVertex3f(i, 12, j+2);
				glVertex3f(i+2, 12, j+2);
				glEnd();
			}
			
			if (i == 10){		    
			 		glBegin(GL_LINES);
					glVertex3f(i+2, j, 12);
					glVertex3f(i+2, j+2, 12);
					glEnd();
				
				glBegin(GL_LINES);
				glVertex3f(i+2, 12, j);
				glVertex3f(i+2, 12, j+2);
				glEnd();
	    	}
    	}
	}

	// glLineWidth(2.0);
	// glBegin(GL_LINES);
	// glVertex3f(-12, 0, 0);
	// glVertex3f(12, 0, 0);
	// glEnd();

	// glBegin(GL_LINES);
	// glVertex3f(-12, 0, -12);
	// glVertex3f(12, 0, -12);
	// glEnd();

	// glBegin(GL_LINES);
	// glVertex3f(0, -12, -12);
	// glVertex3f(0, 12, -12);
	// glEnd();

	// glBegin(GL_LINES);
	// glVertex3f(-12, 12, 0);
	// glVertex3f(12, 12, 0);
	// glEnd();

	// glBegin(GL_LINES);
	// glVertex3f(0, 0, -12);
	// glVertex3f(0, 0, 12);
	// glEnd();

	// glBegin(GL_LINES);
	// glVertex3f(0, 12, -12);
	// glVertex3f(0, 12, 12);
	// glEnd();

	// glLineWidth(1.0);
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
  
  if(fscanf(paramfile, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
            &Mass, &(V0.x), &(V0.y), &(V0.z), &Viscosity, &CoeffofRestitution,&Fraction,
            &TimeStep, &DispTime, &(Wind.x), &(Wind.y), &(Wind.z), &epsilon) != 13){
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

void DrawBall(int collision)
{
  
  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();
  glTranslatef(Ball.x, Ball.y, Ball.z);
  

  //glColor3f( 1.0f, 1.0f, 1.0f );
  glCallList(mysphereID);
  glutSolidSphere(radius, 100, 100 );
  //glTranslatef(-Ball.x,-Ball.y,-Ball.z);
  //makeGrid();
  //glutSwapBuffers();
  glutPostRedisplay();
  

  //????what is this?
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
Vector3d Accel(Vector3d currentVelocity){
  Vector3d acceleration;
  double mass=Mass;
  /*
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
  }*/  
  acceleration = G;
  acceleration = acceleration + Viscosity * (Wind-currentVelocity) / mass;
    
  /*if(WeightMatters != NOAIR){
    if(HaveWind)
      acceleration = acceleration + Viscosity * (Wind - Velocity) / mass;
    else
      acceleration = acceleration - Viscosity * Velocity / mass;
  }*/
  
  return acceleration;
}

void Simulate()
{
  int i;
  Status = true;

  Vector3d p[6]={Vector3d(0,-12,0),Vector3d(0,12,0),
                Vector3d(12,0,0),Vector3d(-12,0,0),
  				      Vector3d(0,0,12),Vector3d(0,0,-12)};

  Vector3d nhab[6]={Vector3d(0,1,0),Vector3d(0,-1,0),
  					Vector3d(-1,0,0),Vector3d(1,0,0),
  					Vector3d(0,0,-1),Vector3d(0,0,1)};

  Vector3d acceleration;
  Vector3d newvelocity, newball,vcollision,xcollision,vn,vt,vtan,vnPrime,vtanPrime,vcPrime;
  Vector3d finalvelocity,finalposition;


  bool collisionflag[6]={false,false,false,false,false,false},totalflag=false;
  double d,bigD,s[6];
  int index[6];

// if (resting) {
//           std::cout<<"the ball is resting\nV:"<<newvelocity<<"\nBall:"<<newball<<"\n";
//           newvelocity=Vector3d(0,0,0);
//           newball.y=-11.0;
//           acceleration=Vector3d(0.0,0.0,0.0);
//           resting=true;
//           goto JUMP;
//         }
  

  //resting  processing to go!
  for(int i=0;i<6;i++){ 
   // if (restingflag[i])   {
      std::cout<<abs((Ball-p[i])*nhab[i]-radius)<<"  &&  "<<abs(Velocity*nhab[i])<<"  E:  "<<epsilon<<std::endl;
      bool t1=abs((Ball-p[i])*nhab[i]-radius) < epsilon;
      bool t2=abs(Velocity*nhab[i]) < epsilon;
      //std::cout<<t1<<" "<<t2<<" i:"<<i<<std::endl;
      if (t1==1&&t2==1 ){
        restingflag[i]=true;
      } else
        restingflag[i]=false;
  }

  acceleration = Accel(Velocity);

std::cout<<"restingflag!!!!\n";
for (int i=0;i<6;i++){
  std::cout<<restingflag[i]<<" "<<acceleration*nhab[i]<<std::endl;
}

  for(i=0;i<6;i++){
    if (abs((acceleration-G)*nhab[i])< epsilon&&restingflag[i]){
      std::cout<<"flag!!!!\n"<<i<<std::endl;
      acceleration=acceleration-((acceleration*nhab[i])*nhab[i]);
      restingflag[i]=true;
    }
    else{
      restingflag[i]=false;
    }         
  }
  // evil Euler integration to get velocity and position at next timestep
  newvelocity = Velocity + TimeStep * acceleration;
  newball = Ball + TimeStep * Velocity;



  for(int i=0;i<6;i++){
    s[i]=0.0;        
    if (restingflag[i]){
      newvelocity=newvelocity-(newvelocity*nhab[i])*nhab[i];
      newball=newball-((newball-p[i])*nhab[i]-radius)*nhab[i];
      s[i]=10;
      goto SKIP;    
    }

  	bigD=(Ball-newball)*nhab[i];
    d=(Ball-p[i])*nhab[i];
    s[i]=(d-radius)/bigD;//(someproblem)
    if (s[i]>0&&s[i]<1){
      collisionflag[i]=true;
      totalflag=true;
    }
SKIP:
    index[i]=i;
  }

std::cout<<std::endl;
std::cout<<"CollisionFlag!!!!\n";
for (int i=0;i<6;i++){
  std::cout<<collisionflag[i]<<" ";
}
std::cout<<std::endl;
  std::cout<<totalflag<<std::endl;
  if (totalflag){
      for(int i=0;i<5;i++){
        for(int j=i+1;j<6;j++){
          double temp;
          int t;
          if (s[i]<s[j]){
            temp=s[i];
            s[i]=s[j];
            s[j]=temp;
            t=index[i];
            index[i]=index[j];
            index[j]=t;
          }
        }
      }

      //std::cout<<s[0]<<" "<<s[1]<<" "<<s[2]<<" "<<s[3]<<" "<<s[4]<<" "<<s[5]<<" "<<std::endl;
      //std::cout<<collisionflag[index[0]]<<" "<<collisionflag[index[1]]<<" "<<collisionflag[index[2]]<<" "<<collisionflag[index[3]]<<" "<<collisionflag[index[4]]<<" "<<collisionflag[index[5]]<<"\n";
      double uptonow=0.0;
      for(int i=0;i<6;i++){
         
        if (collisionflag[index[i]]&&(!restingflag[index[i]])){
      		//Collision here;
          std::cout<<"collision!!\n";
          if (i>0&&collisionflag[index[i-1]]){
            s[i]=s[i]-s[i-1];
          }
          uptonow+=s[i];
          std::cout<<uptonow<<"!!!!"<<endl;
          vcollision=Velocity+s[i]*TimeStep*acceleration;
      		xcollision=Ball+s[i]*TimeStep*Velocity;
      		vn=(vcollision*nhab[index[i]])*nhab[index[i]];
      		vt=vcollision-vn;
      		vnPrime=-CoeffofRestitution*vn;
      		vtanPrime=(1-Fraction)*vt;
      		vcPrime=vtanPrime+vnPrime;
          Velocity = vcPrime;
          Ball = xcollision;
          acceleration= Accel(vcPrime);
      	}
        
      }
      
      //acceleration= Accel(vcPrime);
      newvelocity=vcPrime+(1-uptonow)*TimeStep*acceleration;
      newball= xcollision+(1-uptonow)*TimeStep*vcPrime;
  }
  


//            vcollision=Velocity+s[i]*TimeStep*acceleration;
//           xcollision=Ball+s[i]*TimeStep*Velocity;
//           vn=(vcollision*nhab[index[i]])*nhab[index[i]];
//           vt=vcollision-vn;
//           vnPrime=-CoeffofRestitution*vn;
//           vtanPrime=(1-Fraction)*vt;
//           vcPrime=vtanPrime+vnPrime;
//           acceleration= Accel(vcPrime);
//           newvelocity=vcPrime+(1-s[i])*TimeStep*acceleration;
//           newball= xcollision+(1-s[i])*TimeStep*vcPrime;
  //JUMP:
JUMP:
  //if (newball-Ball )
std::cout<<newball.y<<std::endl;
  Time += TimeStep;
  NTimeSteps++;
  Velocity = newvelocity;
  Ball = newball;
//SKIP:
  // draw only if we are at a display time
  //if(NTimeSteps % TimeStepsPerDisplay == 0)
  DrawBall(0);
  /*if(Stopped)
    Stopped = true;
  else{
    Stopped = false;*/
  if (!Stopped)
    glutTimerFunc(TimerDelay, TimerCallback, 0);
  //}
  //glutSwapBuffers();
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

  GLUquadricObj *sphere=NULL;
  sphere = gluNewQuadric();
  gluQuadricDrawStyle(sphere, GLU_FILL);
  gluQuadricTexture(sphere, TRUE);
  gluQuadricNormals(sphere, GLU_SMOOTH);
  //Making a display list
  mysphereID = glGenLists(1);
  glNewList(mysphereID, GL_COMPILE);
  gluSphere(sphere, 1.0, 100, 100);
  glEndList();
  gluDeleteQuadric(sphere);
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
}

void PerspDisplay() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // draw the camera created in perspective
  camera->PerspectiveDisplay(WIDTH, HEIGHT);

 
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  do_lights();



  if (showGrid) 
    makeGrid();

  
  //
  // here is where you would draw your scene!
  //
  glTranslatef(Ball.x, Ball.y, Ball.z);
  //glutWireTeapot(5);
  //glColor3f( 1.0f, 1.0f, 1.0f );
//  glCallList(mysphereID);
  glutSolidSphere( radius, 100, 100 );
  //do_material();

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


/*
 Routine to restart the ball at the top
 */
void RestartBall(){
  
  LoadParameters(ParamFilename); // reload parameters in case changed
  
  //Start = true;
  //Stopped = true;
  NTimeSteps = -1;
  glutIdleFunc(NULL);
  Ball.set(STARTX, STARTY,STARTZ);
  Time = 0;
  Velocity = V0;
  resting=false;
  //acceleration=G;
  for(int i=0;i<6;i++){
    restingflag[i]=true;
  }
  DrawBall(0);
}

void keyboardEventHandler(unsigned char key, int x, int y) {
  switch (key) {
  case 'r': case 'R':
    // reset the camera to its initial position
    //camera->Reset();
    RestartBall();
    break;
  case 'f': case 'F':
    camera->Reset();
    break;
  case 'g': case 'G':
    showGrid = !showGrid;
    break;
  case 's': case 'S':
    RestartBall();
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
  glEnable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_NORMALIZE);

}

int main(int argc, char *argv[]) {

  // set up opengl window
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(WIDTH, HEIGHT);
  glutInitWindowPosition(50, 50);
  persp_win = glutCreateWindow("Project1");


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

