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
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
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
#define MAPLE_VERTEX_NUM 2054
#define TRIANGLE_VERTEX_NUM 1968
#define MAPLE_SCALER 0.005
#define MAPLE_TEX_ID 1
#define GROUND_TEX_ID 20
#define SKYBOX_TEX_ID_FNT 10
#define SKYBOX_TEX_ID_LFT 11
#define SKYBOX_TEX_ID_BAK 12
#define SKYBOX_TEX_ID_RHT 13
#define SKYBOX_TEX_ID_TOP 14
#define SKYBOX_TEX_ID_BOT 15

int WIDTH = 1024;
int HEIGHT = 765;

int persp_win;

Camera *camera;

bool showGrid = false;
bool Stopped =true;

struct  tri
{
  int a,b,c;
};

static Vector3d Wind;
static Vector3d StartPoint;
static Vector3d Stone;
static double StoneRadius;
static std::vector<StateVector> State;
static std::vector<Vector3d> LeavesColor;
static std::vector<double> Borntime;
static std::vector<double> LeavesLifetime;
static std::vector<Vector3d> PointsData(MAPLE_VERTEX_NUM);
static std::vector<tri> Triangles(TRIANGLE_VERTEX_NUM);

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
static double LifeTime;
static int numofleaves=0;
static int TimerDelay;
static int TimeStepsPerDisplay;
static int NSteps = 0;
static int NTimeSteps = -1;
static double MapleW;
static double MapleH;
static double MapleL;
static Matrix3x3 MapleBigI;
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

void do_lights()
{
  float light0_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
  float light0_diffuse[] = { 1, 1, 1, 1.0 };
  float light0_specular[] = { 0.1, 0.1, 0.1, 1.0 };
  float light0_position[] = { 0.0, 0.0, 0.0, 1.0 };
  float light0_direction[] = { -1.0, -1.0, -1.0, 1.0};
  float lmodel_ambient[] = { 0.2, 0.2, 0.2, 1.0 };


  glLightfv(GL_LIGHT0,GL_AMBIENT,light0_ambient); 
  glLightfv(GL_LIGHT0,GL_DIFFUSE,light0_diffuse); 
  glLightfv(GL_LIGHT0,GL_POSITION,light0_position);
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
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);

}

void load_texture(string filename,int TexID)
{
  FILE *fptr;
  char buf[512], *parse;
  int im_size, im_width, im_height, max_color;
  unsigned char *texture_bytes; 

  fptr=fopen(filename.c_str(),"r");
  fgets(buf,512,fptr);
  do{
    fgets(buf,512,fptr);
    } while(buf[0]=='#');
  parse = strtok(buf," \t");
  im_width = atoi(parse);

  parse = strtok(NULL," \n");
  im_height = atoi(parse);

  fgets(buf,512,fptr);
  parse = strtok(buf," \n");
  max_color = atoi(parse);

  im_size = im_width*im_height;
  texture_bytes = (unsigned char *)calloc(3,im_size);
  fread(texture_bytes,3,im_size,fptr);
  fclose(fptr);

  glBindTexture(GL_TEXTURE_2D,TexID);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,im_width,im_height,0,GL_RGB, 
    GL_UNSIGNED_BYTE,texture_bytes);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
  //glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
  cfree(texture_bytes);
}

void load_Repeat_texture(string filename,int TexID)
{
  FILE *fptr;
  char buf[512], *parse;
  int im_size, im_width, im_height, max_color;
  unsigned char *texture_bytes; 

  fptr=fopen(filename.c_str(),"r");
  fgets(buf,512,fptr);
  do{
    fgets(buf,512,fptr);
    } while(buf[0]=='#');
  parse = strtok(buf," \t");
  im_width = atoi(parse);

  parse = strtok(NULL," \n");
  im_height = atoi(parse);

  fgets(buf,512,fptr);
  parse = strtok(buf," \n");
  max_color = atoi(parse);

  im_size = im_width*im_height;
  texture_bytes = (unsigned char *)calloc(3,im_size);
  fread(texture_bytes,3,im_size,fptr);
  fclose(fptr);

  glBindTexture(GL_TEXTURE_2D,TexID);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,im_width,im_height,0,GL_RGB, 
    GL_UNSIGNED_BYTE,texture_bytes);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
  glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
  //glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
  cfree(texture_bytes);
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
  
  if(fscanf(paramfile, "%d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
            &TotalNum,&LifeTime, &Mass, &Kij,&Dtheta, &TimeStep, &DispTime,&(StartPoint.x),&(StartPoint.y),&(StartPoint.z),&epsilon) != 11){
    fprintf(stderr, "error reading parameter file %s\n", filename);
    exit(1);
  }
  
  fclose(paramfile);
  double maxx=0,maxy=0,maxz=0;
  double minx=0,miny=0,minz=0;

  ifstream maplepoint ("Maple-model.xml");
  if (maplepoint.is_open())
  {
    for(int i = 0;i<MAPLE_VERTEX_NUM;i++){
      maplepoint >> PointsData[i].x >> PointsData[i].y >> PointsData[i].z;
      PointsData[i].x*=MAPLE_SCALER;
      maxx=Max(maxx,PointsData[i].x);
      minx=Min(minx,PointsData[i].x);
      PointsData[i].y*=MAPLE_SCALER;
      maxy=Max(maxy,PointsData[i].y);
      miny=Min(miny,PointsData[i].y);
      PointsData[i].z*=MAPLE_SCALER;
      maxz=Max(maxz,PointsData[i].z);
      minz=Min(minz,PointsData[i].z);
       
    }
    maplepoint.close();
  }else cout << "Unable to open file"; 
  MapleL=maxx-minx;
  MapleW=maxy-miny;
  MapleH=maxz-minz;
  MapleBigI=Matrix3x3(Mass/12*(MapleW*MapleW+MapleH*MapleH),0,0,0,Mass/12*(MapleL*MapleL+MapleH*MapleH),0,0,0,Mass/12*(MapleL*MapleL+MapleW*MapleW));
  
  //load_texture("Texture/bubble_color.ppm",MAPLE_TEX_ID);
  string files[]={"./TropicalSunnyDay/Fnt.ppm",
                    "./TropicalSunnyDay/Lft.ppm",
                    "./TropicalSunnyDay/Bak.ppm",
                    "./TropicalSunnyDay/Rht.ppm",
                    "./TropicalSunnyDay/Top.ppm",
                    "./TropicalSunnyDay/Bot.ppm"};
  load_texture(files[0],SKYBOX_TEX_ID_FNT);
  load_texture(files[1],SKYBOX_TEX_ID_LFT);
  load_texture(files[2],SKYBOX_TEX_ID_BAK);
  load_texture(files[3],SKYBOX_TEX_ID_RHT);
  load_texture(files[4],SKYBOX_TEX_ID_TOP);
  load_texture(files[5],SKYBOX_TEX_ID_BOT);

  load_Repeat_texture("./Texture/Ground.ppm",GROUND_TEX_ID);


  ifstream trianglepoint ("Triangle-index.xml");
  if (trianglepoint.is_open()){
    for (int i=0;i<TRIANGLE_VERTEX_NUM;i++){
      trianglepoint >> Triangles[i].a >>Triangles[i].b >>Triangles[i].c;
    }
    trianglepoint.close();
  }else cout << "Unable to read file";
  
  TimeStepsPerDisplay = Max(1, int(DispTime / TimeStep + 1.0 / 2 ));
  TimerDelay = int(1.0 / 2 * TimeStep * 1000);
}

void makenormal(const Vector3d &v0, const Vector3d &v1, const Vector3d &v2){
  Vector3d normal = ((v1 - v0) % (v2 - v0)).normalize();
  glNormal3f(normal.x, normal.y, normal.z);
}
struct point {
  float x, y, z;
  };



// glBindTexture(GL_TEXTURE_2D,1);
// glEnable(GL_TEXTURE_2D);
// glBegin(GL_QUADS); 
// glNormal3f(0.0,0.0,1.0);
// for(i=0;i<4;i++){
//   glTexCoord2fv(mytexcoords[i]);
//   glVertex3f(front[i].x,front[i].y,front[i].z);
//   }
// glEnd();
// glDisable(GL_TEXTURE_2D);

void DrawLeaves()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  
  // Store the current matrix
    glPushMatrix();
    // Reset and transform the matrix.
    glLoadIdentity();
    // Enable/Disable features
    glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);
    // Just in case we set all vertices to white.
    glColor4f(1,1,1,1);
     // Render the front quad
    glBindTexture(GL_TEXTURE_2D, SKYBOX_TEX_ID_FNT);
    glBegin(GL_QUADS);
        glTexCoord2f(1, 1); glVertex3f(  100.0f, -100.0f, -100.0f );
        glTexCoord2f(0, 1); glVertex3f( -100.0f, -100.0f, -100.0f );
        glTexCoord2f(0, 0); glVertex3f( -100.0f,  100.0f, -100.0f );
        glTexCoord2f(1, 0); glVertex3f(  100.0f,  100.0f, -100.0f );
    glEnd();
 
    // Render the left quad
    glBindTexture(GL_TEXTURE_2D, SKYBOX_TEX_ID_LFT);
    glBegin(GL_QUADS);
        glTexCoord2f(1, 1); glVertex3f(  100.0f, -100.0f,  100.0f );
        glTexCoord2f(0, 1); glVertex3f(  100.0f, -100.0f, -100.0f );
        glTexCoord2f(0, 0); glVertex3f(  100.0f,  100.0f, -100.0f );
        glTexCoord2f(1, 0); glVertex3f(  100.0f,  100.0f,  100.0f );
    glEnd();
 
    // Render the back quad
    glBindTexture(GL_TEXTURE_2D, SKYBOX_TEX_ID_BAK);
    glBegin(GL_QUADS);
        glTexCoord2f(1, 1); glVertex3f( -100.0f, -100.0f,  100.0f );
        glTexCoord2f(0, 1); glVertex3f(  100.0f, -100.0f,  100.0f );
        glTexCoord2f(0, 0); glVertex3f(  100.0f,  100.0f,  100.0f );
        glTexCoord2f(1, 0); glVertex3f( -100.0f,  100.0f,  100.0f );
 
    glEnd();
 
    // Render the right quad
    glBindTexture(GL_TEXTURE_2D, SKYBOX_TEX_ID_RHT);
    glBegin(GL_QUADS);
        glTexCoord2f(1, 1); glVertex3f( -100.0f, -100.0f, -100.0f );
        glTexCoord2f(0, 1); glVertex3f( -100.0f, -100.0f,  100.0f );
        glTexCoord2f(0, 0); glVertex3f( -100.0f,  100.0f,  100.0f );
        glTexCoord2f(1, 0); glVertex3f( -100.0f,  100.0f, -100.0f );
    glEnd();
 
    // Render the top quad
    glBindTexture(GL_TEXTURE_2D, SKYBOX_TEX_ID_TOP);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 1); glVertex3f( -100.0f,  100.0f, -100.0f );
        glTexCoord2f(0, 0); glVertex3f( -100.0f,  100.0f,  100.0f );
        glTexCoord2f(1, 0); glVertex3f(  100.0f,  100.0f,  100.0f );
        glTexCoord2f(1, 1); glVertex3f(  100.0f,  100.0f, -100.0f );
    glEnd();
 
    // Render the bottom quad
    glBindTexture(GL_TEXTURE_2D, SKYBOX_TEX_ID_BOT);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( -100.0f, -100.0f, -100.0f );
        glTexCoord2f(0, 1); glVertex3f( -100.0f, -100.0f,  100.0f );
        glTexCoord2f(1, 1); glVertex3f(  100.0f, -100.0f,  100.0f );
        glTexCoord2f(1, 0); glVertex3f(  100.0f, -100.0f, -100.0f );
    glEnd();

    
 
    // Restore enable bits and matrix
    glPopAttrib();
    glPopMatrix();

  glDisable(GL_TEXTURE_2D);
  glLoadIdentity();
  glEnable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE_ARB);
  //TODO:Draw the CUBE

  for(int index=0;index<numofleaves;index++){
    if(Time-Borntime[index]>1e-6){
      glLoadIdentity();
      glTranslatef(State[index].x.x,State[index].x.y,State[index].x.z);
      Quaternion rotate=State[index].q;
      rotate.GLrotate();

      glBegin(GL_TRIANGLES); 
      glColor3f(1,0,0);

      for(int i=0;i<TRIANGLE_VERTEX_NUM;i++){
        makenormal(PointsData[Triangles[i].a],PointsData[Triangles[i].b],PointsData[Triangles[i].c]);
        glVertex3f(PointsData[Triangles[i].a].x,PointsData[Triangles[i].a].y,PointsData[Triangles[i].a].z);
        glVertex3f(PointsData[Triangles[i].b].x,PointsData[Triangles[i].b].y,PointsData[Triangles[i].c].z);
        glVertex3f(PointsData[Triangles[i].c].x,PointsData[Triangles[i].c].y,PointsData[Triangles[i].c].z);
         
      }
      glEnd();
      // glLoadIdentity();
      // glBegin(GL_LINES);

      // glVertex3f(StartPoint.x,StartPoint.y,StartPoint.z);
      // Vector3d Pi(0,-2,0);
      // Vector3d Somepoint=Pi;
      // Pi=State[index].x+State[index].q.rotation()*Pi;

      // glVertex3f(Pi.x,Pi.y,Pi.z);
      // glEnd();
    }
  }
  glLoadIdentity();
  glColor4f(1,1,1,1);
  glEnable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);
  glBindTexture(GL_TEXTURE_2D, GROUND_TEX_ID);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( -100.0f, -12.0f, -100.0f );
        glTexCoord2f(0, 30); glVertex3f( -100.0f, -12.0f,  100.0f );
        glTexCoord2f(30, 30); glVertex3f(  100.0f, -12.0f,  100.0f );
        glTexCoord2f(30, 0); glVertex3f(  100.0f, -12.0f, -100.0f );
    glEnd();
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_LIGHTING);
  
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
  numofleaves=0;
  State.clear();
  LeavesLifetime.clear();
  Borntime.clear();

  LeavesLifetime.reserve(TotalNum);
  Borntime.reserve(TotalNum);
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

  LeavesLifetime.reserve(TotalNum);
  Borntime.reserve(TotalNum);
  State.reserve(TotalNum);  

  numofleaves=0;

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
  Matrix3x3 I0T= MapleBigI;
  StateVector Sdot;

  
  Sdot.x= S.P/Mass;
  Matrix3x3 R= S.q.rotation();
  I0T=I0T.inv();
  Matrix3x3 It= R * I0T * R.transpose();
  Vector3d w = S.L*I0T;

  Quaternion wq= Quaternion(w);
  Sdot.q=1.0 / 2 * wq * S.q;

  Vector3d zero(0,0,0);
  Sdot.P=zero;
  Sdot.L=zero;
  
  Vector3d g(-5,-10,0);
  //Sdot.P=g*Mass;
  Vector3d Pi(0,-2,0);
  Pi=S.x+S.q.rotation()*Pi;

  // if ((StartPoint-Pi).norm()>2){
  //     Vector3d springforce=Kij*(StartPoint-Pi).normalize()*((StartPoint-Pi).norm()-2);
  //     Vector3d damper=-Dtheta*Sdot.x;
  //     Vector3d ForceonP=springforce+damper;
  //     Sdot.P=Sdot.P+ForceonP;
  //     Sdot.L=Sdot.L+ ((Pi-S.x) % ForceonP);
  // }
  Vector3d damper=-Dtheta*Sdot.x;
  Vector3d ForceonP=g*Mass+damper;
  Sdot.P=Sdot.P+ForceonP;
  Sdot.L=Sdot.L+ ((Pi-S.x) % ForceonP);

  return Sdot;
}

void Simulate()
{


  
  //Leaves Killer....
  if (numofleaves!=0&&State.size()!=0){
    for(int i=0;i<numofleaves;i++){
      if (LeavesLifetime[i]-Time+Borntime[i]<1e-6){
        State.erase(State.begin()+i);
        Borntime.erase(Borntime.begin()+i);
        LeavesLifetime.erase(LeavesLifetime.begin()+i);
        numofleaves--;
      }
    }
  }
  //Leaves Generator

  if (numofleaves<TotalNum){
    for(int i=numofleaves;i<TotalNum;i++){
      Vector3d P0=StartPoint;
      P0.x=StartPoint.x+(double)rand()/(RAND_MAX)*20;
      P0.y=StartPoint.y+(double)rand()/(RAND_MAX)*20;
      P0.z=StartPoint.z+(double)rand()/(RAND_MAX)*20;
      Vector3d zero(0,0,0);
      Vector3d V0(0,0,0);
      Vector3d Orientation((double)rand()/(RAND_MAX),(double)rand()/(RAND_MAX),(double)rand()/(RAND_MAX));
      Quaternion q=Quaternion(Orientation);
      double btime=Time+(double)rand()/(RAND_MAX)*20-10;
      double ltime=abs(gauss(LifeTime,epsilon/100,Time));
      //btime=Time;
      ltime=LifeTime;

      StateVector NewLeaf(P0,q,V0*Mass,zero);
      State.push_back(NewLeaf);
      LeavesLifetime.push_back(ltime);
      Borntime.push_back(btime);
    }
    numofleaves=TotalNum;
  }

  for(int i=0;i<numofleaves;i++){
    if (State[i].x.y<=-11||Time-Borntime[i]<1e-6) continue;
    StateVector K1,K2,K3,K4,Snew;
    K1=Force(State[i]);
    K2=Force(State[i]+TimeStep/2*K1);
    K3=Force(State[i]+TimeStep/2*K2);
    K4=Force(State[i]+TimeStep*K3);
    Snew=State[i]+TimeStep/6.0*(K1+2*K2+2*K3+K4);
    //Snew=State[i]+TimeStep*K1;
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
    //hit();
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

