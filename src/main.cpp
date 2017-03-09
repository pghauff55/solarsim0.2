
#include <Windows.h>
#include <glut.h>

#include <stdio.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include <string>
#include "Planet.h"
#include "agviewer.h"
#include "GLToMovie.h"
#include "wmvFile.h"


SPICEINT_CELL           ( ids,   1000 );

CGLToMovie *g_MovieRecorder;

extern GLfloat EyeEl,EyeAz,EyeDist;

void renderScreenString(int x,int y,char *string);


clock_t timer1,timer2;
float rate,diff;


char *names[]={"10","199","299","399","499","599","699","799","899","301"};

double R[]={2.0,0.2,0.9,1.0,0.7,1.3,1.2,1.16,1.02,0.1};

double Rb=200.0;

int DrawAxes = 0;

#define ROTATEINC 2;

GLfloat Rotation = 0;  /* start ring flat and not spinning */
int     Rotating = 1;


GLuint asteroidtex;

SpiceDouble dt=0.01;


SpiceDouble    et=0.0;

int I=3;

GLfloat light0_position[] = { 0.0, 0.0, 0.0, 1.0 }; 
SpiceDouble pos_lista[400][3];
SpiceDouble pos_listb[400][3];
SpiceDouble max_diffab;

#define SW 640
#define SH 480

Planet *jupiter,*earth,*venus,*mercury,*mars;
Planet *sun,*saturn,*neptune,*uranus;
baryCentre *centres[20];

/////////////////////////////////////////////////////////////////////////////////
#define PATH "C:\\Users\\Pamela\\Desktop\\SolarSim3D-0.2\\RawTextures-SolarSim3D\\"

void myGLInit(void)
{
  GLfloat mat_ambient[] = { 0.4, 0.4, 0.4, 0.5 };
  GLfloat mat_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
  GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
 // glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.1f);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); 
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialf(GL_FRONT, GL_SHININESS, 2.0);
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glEnable(GL_NORMALIZE);

  glDepthFunc(GL_LESS);
  glEnable(GL_DEPTH_TEST);

  glShadeModel(GL_SMOOTH);

  glPointSize(0.50);
  glEnable(GL_POINT_SMOOTH);


  g_MovieRecorder=new CGLToMovie("c:\\Users\\Pamela\\Desktop\\Output.avi", SW, SH,24,mmioFOURCC('X','V','I','D'),30);

  mercury=new Planet(1);
  venus=new Planet(2);
  earth=new Planet(3);
  mars=new Planet(4);
  jupiter=new Planet(5);
  sun=new Planet(0);
  saturn=new Planet(6);
  neptune=new Planet(8);
  uranus=new Planet(7);

   uranus->LoadTexture("\\RawData\\uranus.raw",GL_REPEAT,GL_MODULATE);
  neptune->LoadTexture("\\RawData\\neptune.raw",GL_REPEAT,GL_MODULATE);
 
  saturn->LoadTexture("\\RawData\\saturnmap.raw",GL_REPEAT,GL_MODULATE);
  sun->LoadTexture("\\RawData\\SunTexture_2048.raw",GL_REPEAT,GL_MODULATE);
  jupiter->LoadTexture("\\RawData\\jupiter.raw",GL_REPEAT,GL_MODULATE);
  mars->LoadTexture("\\RawData\\mars_tex.raw",GL_REPEAT,GL_MODULATE);
  venus->LoadTexture("\\RawData\\Venus_tex.raw",GL_REPEAT,GL_MODULATE);
  earth->LoadTexture("\\RawData\\earth_tex.raw",GL_REPEAT,GL_MODULATE);
  mercury->LoadTexture("\\RawData\\mercurymap.raw",GL_REPEAT,GL_MODULATE);
 
  centres[0]=new baryCentre(0,0,sun);
  centres[1]=new baryCentre(1,0,mercury);
  centres[2]=new baryCentre(2,0,venus);
  centres[3]=new baryCentre(3,1,earth);
  centres[4]=new baryCentre(4,2,mars);
  centres[5]=new baryCentre(5,50,jupiter);
  centres[6]=new baryCentre(6,10,saturn);
  centres[7]=new baryCentre(7,6,uranus);
  centres[8]=new baryCentre(8,3,neptune);

   asteroidtex=LoadTextureRAW("\\RawData\\ast.raw",GL_CLAMP,GL_MODULATE);

  /*
  
  tex[9]=LoadTextureRAW("RawData\\moon_tex.raw",GL_REPEAT,GL_MODULATE);
 
  */





  glFlush();
} 


int frame=0;
SpiceDouble diffab[400];
void display(void)
{

	


	SpiceDouble    lt;
  SpiceDouble    pos [3],pos1[3],pos2[3],pos3[3],norm1[3],D2,D3,D4;
        SpiceDouble   mat[3][3];
		SpiceDouble vin[3],vout[3];
		SpiceDouble r,colat,lon,ra,dec;
		float angle1,angle2;



int width=glutGet(GLUT_WINDOW_WIDTH);
	int height=glutGet(GLUT_WINDOW_HEIGHT);
	float fieldOfView=75.0;
	char *centers[]={"0","1","2","3","4","5","6","7","8"};
	char *CENTRE=centers[I];
//	Sleep(10);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);


    glLoadIdentity(); 
	
    glOrtho(0, width, 0, height, -1, 1);   // Map abstract coords directly to window coords.
	
	glMatrixMode(GL_MODELVIEW);
	
	timer2=timer1;
	timer1=clock();
	diff=1000.0*float(timer1-timer2)/CLOCKS_PER_SEC;
	rate=1.0/diff;

  et+=dt;
  char ephemeral_time[60];
  char utcstr[60];
  utcstr[0]=0;
  sprintf(ephemeral_time,"%.2f",et);
 et2utc_c ( et, "ISOC", 2, 59, utcstr );

  //mktime 

  renderScreenString(0,20,utcstr);
 
	char ratestr[100];
	sprintf(ratestr,"Rate:%.3f frames/sec : %.3f millisecs/frame",1000.0*rate,diff);
	renderScreenString(0,0,ratestr);
	
  glMatrixMode(GL_PROJECTION);


    glLoadIdentity(); 
 
 GLfloat zNear = 0.1f;
GLfloat zFar = 5000.0f;
GLfloat aspect = float(width)/float(height);
GLfloat fH = tan( float(fieldOfView / 360.0f * 3.14159f) ) * zNear;
GLfloat fW = fH * aspect;
glFrustum( -fW, fW, -fH, fH, zNear, zFar );


    /* so this replaces gluLookAt or equiv */
  agvViewTransform();

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  
  centres[0]->UpdatePos(centres[I]);

  //sun
  spkpos_c ( "10",    et,   "J2000", "NONE","0",  pos,  &lt );
  double Ra=50.0;
  light0_position[0]=centres[0]->cx+centres[0]->scale*pos[0]/AU;
  light0_position[1]=centres[0]->cy+centres[0]->scale*pos[2]/AU;
  light0_position[2]=centres[0]->cz+centres[0]->scale*pos[1]/AU;
  
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

  glDisable(GL_LIGHTING);
  glColor3f(0.05,0.05,0.05);
  glBegin(GL_LINES);
	for(int i=0;i<9;i++){
		for(int j=0;j<9;j++){
			if(i!=j){
				glVertex3f(centres[i]->cx,centres[i]->cy,centres[i]->cz);
				glVertex3f(centres[j]->cx,centres[j]->cy,centres[j]->cz);
			}
		}
	}
	glEnd();
	
	 glColor3f(1.0,1.0,1.0);
	 glPushMatrix();
	glDisable(GL_TEXTURE_2D);
	
	
	 glPointSize(2.0);
	 
	 	  SpiceInt                obj;
		
	if(frame==0){
		max_diffab=0.0;
		  for ( int i = 0;  i < card_c( &ids );  i++  ) {
				obj  =  SPICE_CELL_ELEM_I( &ids, i );
				char name[200];
				bodc2s_c ( obj, 200, name);
				spkpos_c ( name,    et,   "J2000", "NONE",centres[I]->name,  pos_lista[i],  &lt );
				
		}
	 }
	if(frame==1){

		  for ( int i = 0;  i < card_c( &ids );  i++  ) {
				obj  =  SPICE_CELL_ELEM_I( &ids, i );
				char name[200];
				bodc2s_c ( obj, 200, name);
				spkpos_c ( name,    et,   "J2000", "NONE",centres[I]->name,  pos_listb[i],  &lt );
			diffab[i]=sqrt((float)(pos_lista[i][0]-pos_listb[i][0])*(pos_lista[i][0]-pos_listb[i][0])+(pos_lista[i][1]-pos_listb[i][1])*(pos_lista[i][1]-pos_listb[i][1])+(pos_lista[i][2]-pos_listb[i][2])*(pos_lista[i][2]-pos_listb[i][2]) );	
			if(diffab[i]>max_diffab)max_diffab=diffab[i];
		  }
	 }
	 if(frame>1){
		
		
		  for ( int i = 0;  i < card_c( &ids );  i++  ) {
			
			int dframe=300*diffab[i]/max_diffab;
			//printf("(%d,%d)\n",i,dframe);
				if(frame%dframe==0){	
			  obj  =  SPICE_CELL_ELEM_I( &ids, i );
				char name[200];
				pos_listb[i][0]=pos_lista[i][0];
				pos_listb[i][1]=pos_lista[i][1];
				pos_listb[i][2]=pos_lista[i][2];
				bodc2s_c ( obj, 200, name);
				spkpos_c ( name,    et,   "J2000", "NONE",centres[I]->name,  pos_lista[i],  &lt );
			//	diffab[i]=sqrt((float)(pos_lista[i][0]-pos_listb[i][0])*(pos_lista[i][0]-pos_listb[i][0])+(pos_lista[i][1]-pos_listb[i][1])*(pos_lista[i][1]-pos_listb[i][1])+(pos_lista[i][2]-pos_listb[i][2])*(pos_lista[i][2]-pos_listb[i][2]) );	
		//	if(diffab[i]>max_diffab)max_diffab=diffab[i];
			  }
				if(frame>300)frame=2;
		}
	 }
		
	 
	 glBegin(GL_POINTS);
	 for ( int i = 0;  i < card_c( &ids );  i++  ) {
		   float xa,ya,za;
		   xa=30.0*pos_lista[i][0]/AU;
		   ya=30.0*pos_lista[i][2]/AU;
		   za=30.0*pos_lista[i][1]/AU;
		   glVertex3f(xa,ya,za);
	  }
	  glEnd();
	  
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glPopMatrix();
	glPointSize(1.0);
	 
for(int i=0;i<9;i++){
	glPushMatrix();
	centres[i]->UpdatePos(centres[I]);
	if(i!=0){
	centres[i]->CalcOrbitMarkers();
	centres[i]->DrawOrbitMarkers(centres[I]);
	}

	centres[i]->Translate();
	centres[i]->Draw(centres[I]);
	glPopMatrix();

}

g_MovieRecorder->RecordFrame();

  glutSwapBuffers();
 frame++;
}


void calc(int i,int j,int N,float *x,float *y,float *z){
			if(i>=N)i=0;
			if(j>=N)j=N;
			float theta=i*M_PI*2.00/N;
			float phi=j*M_PI/N-M_PI/2.0;
			float st=sin(theta),sp=sin(phi),ct=cos(theta),cp=cos(phi);
			*x=ct*cp;
			*y=sp;
			*z=st*cp;


}


void MakeDisplayLists(void)
{


 
  //Making a display list
  int N=100;
  glNewList(SPHERE, GL_COMPILE);
  glPushMatrix();
  renderBitmapString(2.9,0,0,"X",0.001);
	renderBitmapString(0,2.9,0,"Z",0.001);
	renderBitmapString(0,0,2.9,"Y",0.001);
 
    glBegin(GL_TRIANGLES);
	for(int i=0;i<N;i++)
		for(int j=0;j<N;j++){
			float x,y,z;
			calc(i,j,N,&x,&y,&z);glNormal3f(x,y,z);glTexCoord2f(float(i)/float(N),float(j)/float(N));glVertex3f(x,y,z);
			calc(i+1,j,N,&x,&y,&z);glNormal3f(x,y,z);glTexCoord2f(float(i+1)/float(N),float(j)/float(N));glVertex3f(x,y,z);
			calc(i+1,j+1,N,&x,&y,&z);glNormal3f(x,y,z);glTexCoord2f(float(i+1)/float(N),float(j+1)/float(N));glVertex3f(x,y,z);
			
			calc(i+1,j+1,N,&x,&y,&z);glNormal3f(x,y,z);glTexCoord2f(float(i+1)/float(N),float(j+1)/float(N));glVertex3f(x,y,z);
			calc(i,j+1,N,&x,&y,&z);glNormal3f(x,y,z);glTexCoord2f(float(i)/float(N),float(j+1)/float(N));glVertex3f(x,y,z);
			calc(i,j,N,&x,&y,&z);glNormal3f(x,y,z);glTexCoord2f(float(i)/float(N),float(j)/float(N));glVertex3f(x,y,z);
		}
	glEnd();


	glBegin(GL_LINES);
		
		glVertex3f(1.5,0,0);
		glVertex3f(0,0,0);
		
		glVertex3f(0,0,1.5);
		glVertex3f(0,0,0);
		
		glVertex3f(0,-1.5,0);
		glVertex3f(0,0,0);

	glEnd();
  glPopMatrix();
  glEndList();
  

  //Making a display list
 N=80;
  glNewList(INV_NORMAL_SPHERE, GL_COMPILE);
  glPushMatrix();
    glBegin(GL_QUADS);
	for(int i=0;i<N;i++)
		for(int j=0;j<N;j++){
			float x,y,z;
			calc(i,j,N,&x,&y,&z);glNormal3f(x,y,z);glTexCoord2f(float(i)/float(N),float(j)/float(N));glVertex3f(-x,-y,-z);
			calc(i+1,j,N,&x,&y,&z);glNormal3f(x,y,z);glTexCoord2f(float(i+1)/float(N),float(j)/float(N));glVertex3f(-x,-y,-z);
			calc(i+1,j+1,N,&x,&y,&z);glNormal3f(x,y,z);glTexCoord2f(float(i+1)/float(N),float(j+1)/float(N));glVertex3f(-x,-y,-z);
			calc(i,j+1,N,&x,&y,&z);glNormal3f(x,y,z);glTexCoord2f(float(i)/float(N),float(j+1)/float(N));glVertex3f(-x,-y,-z);
		}
	glEnd();


	glBegin(GL_LINES);
		
		glVertex3f(1.5,0,0);
		glVertex3f(0,0,0);
		
		glVertex3f(0,0,1.5);
		glVertex3f(0,0,0);
		
		glVertex3f(0,-1.5,0);
		glVertex3f(0,0,0);

	glEnd();
  glPopMatrix();
  glEndList();




  //Making a display list
 N=8;
  glNewList(ASTEROID, GL_COMPILE);
  glPushMatrix();
    glBegin(GL_QUADS);
	for(int i=0;i<N;i++)
		for(int j=0;j<N;j++){
			float x,y,z;
			calc(i,j,N,&x,&y,&z);glNormal3f(x,y,z);glTexCoord2f(float(i)/float(N),float(j)/float(N));glVertex3f(x,y,z);
			calc(i+1,j,N,&x,&y,&z);glNormal3f(x,y,z);glTexCoord2f(float(i+1)/float(N),float(j)/float(N));glVertex3f(x,y,z);
			calc(i+1,j+1,N,&x,&y,&z);glNormal3f(x,y,z);glTexCoord2f(float(i+1)/float(N),float(j+1)/float(N));glVertex3f(x,y,z);
			calc(i,j+1,N,&x,&y,&z);glNormal3f(x,y,z);glTexCoord2f(float(i)/float(N),float(j+1)/float(N));glVertex3f(x,y,z);
		}
	glEnd();

	renderBitmapString(2.9,0,0,"X",0.001);
	renderBitmapString(0,2.9,0,"Z",0.001);
	renderBitmapString(0,0,2.9,"Y",0.001);
	glBegin(GL_LINES);
		
		glVertex3f(2.5,0,0);
		glVertex3f(0,0,0);
		
		glVertex3f(0,0,2.5);
		glVertex3f(0,0,0);
		
		glVertex3f(0,2.5,0);
		glVertex3f(0,0,0);

	glEnd();
  glPopMatrix();
  glEndList();



  glNewList(STUFF, GL_COMPILE);
  glPushMatrix();
  
  glutSolidSphere(1.5, 10, 10);
   glPopMatrix();
  glEndList();

  glNewList(RING, GL_COMPILE);
    glutSolidTorus(0.1, 0.5, 8, 15);
  glEndList();
}

void axes(){

	glBegin(GL_LINES);
		
		glVertex3f(2.5,0,0);
		glVertex3f(0,0,0);
		
		glVertex3f(0,0,2.5);
		glVertex3f(0,0,0);
		
		glVertex3f(0,2.5,0);
		glVertex3f(0,0,0);

	glEnd();
}
void renderBitmapString(
		float x,
		float y,
		float z,
		char *string,float scale) {
  char *c;
  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
  glPushMatrix();
  
  glTranslatef(x,y,z);
  glRotatef(-EyeAz,0,1,0);
  glRotatef(-EyeEl,1,0,0);
  
  
  glScalef(scale,scale,scale);
  for (c=string; *c != '\0'; c++) {
    glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);
  }
  glPopMatrix();
  glEnable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);
}

void renderScreenString(int x,int y,
		char *string) {
  char *c;
  glDisable(GL_LIGHTING);
  
  glRasterPos2i(x,y);;
  for (c=string; *c != '\0'; c++) {
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
  }
  
  glEnable(GL_LIGHTING);
  
}




  /* rotate the axis and adjust position if nec. */
void rotatethering(void)
{ 
  Rotation += ROTATEINC;

  if (agvMoving)   /* we since we are the only idle function, we must */
    agvMove();     /* give AGV the chance to update the eye position */

  glutPostRedisplay();
}

typedef enum { MENU_AXES, MENU_QUIT, MENU_RING } MenuChoices;

void handlemenu(int value)
{
  switch (value) {
    case MENU_AXES:
      DrawAxes = !DrawAxes;
      break;
    case MENU_QUIT:
		delete g_MovieRecorder;
      exit(0);
      break;
    case MENU_RING:
      Rotating = !Rotating;
      if (Rotating) {
	glutIdleFunc(rotatethering);    /* install our idle function */
	agvSetAllowIdle(0);             /* and tell AGV to not */
      } else {
	glutIdleFunc(NULL);    /* uninstall our idle function      */
	agvSetAllowIdle(1);    /* and tell AGV it can mess with it */
      }
      break;
    }
  glutPostRedisplay();
}

void visible(int v)
{
  if (v == GLUT_VISIBLE) {
    if (Rotating) {
      glutIdleFunc(rotatethering);
      agvSetAllowIdle(0);
    } else {
      glutIdleFunc(NULL);
      agvSetAllowIdle(1);      
    }
  } else {
      glutIdleFunc(NULL);
      agvSetAllowIdle(0);
  }
}


void MenuInit(void)
{
  int sub2 = glutCreateMenu(agvSwitchMoveMode); 
  glutAddMenuEntry("Flying move",  FLYING);     
  glutAddMenuEntry("Polar move",   POLAR);

  glutCreateMenu(handlemenu);
  glutAddSubMenu("Movement", sub2);
  glutAddMenuEntry("Toggle Axes", MENU_AXES);
  glutAddMenuEntry("Toggle ring rotation", MENU_RING);
  glutAddMenuEntry("Quit", MENU_QUIT);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void close(){


	delete g_MovieRecorder;
}



char *kernels[]={"de424.bsp","pck00010.tpc","jup291.bsp","jup282.bsp","sat353.bsp","nep085.bsp","ura095.bsp","plu021.bsp",
	"codes_300ast_20061020.tf","codes_300ast_20061020.bsp","naif0010.tls","jup282.bsp","jup230-rocks.bsp","sat319.bsp",
	"mar085.bsp","sat341.bsp","nep081.bsp","ura083.bsp","jup230l.bsp","sat351.bsp","sat353-rocks.bsp","nep077.bsp","ura091.bsp"};


int main(int argc, char** argv)
{
	char path[100];    
  glutInit(&argc, argv);
  glutInitWindowSize(SW, SH);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutCreateWindow("3dsolarsim-0.2");
  
  strcpy(path,argv[0]);
  int len=strlen(path);
  while(len>0){
		if(path[len]=='\\')break;
			len--;
  }
  path[len]='\x0';
  printf("%s\n",path);
  
  ConstSpiceChar  * versn;
 
      versn = tkvrsn_c( "TOOLKIT" );

	  SpiceInt lenout=100;
	    /*
      Set the SPICELIB error response action
      to "RETURN":
      */
      erract_c (  "SET", lenout, "IGNORE"  );

	  for(int i=0;i<23;i++){
		  char buffer[256];
			sprintf(buffer,"%s\\data\\%s",path,kernels[i]);  
			printf("loading %s\n",buffer);
		  furnsh_c(buffer);
		  if(i==9){spkobj_c ( buffer, &ids );}
	  }
	 /* 
	  furnsh_c (    );
   
		furnsh_c ( "./data/pck00010.tpc" );
   
   furnsh_c ( "./data/jup291.bsp" );
   //furnsh_c ( "jup230l.bsp" );
   furnsh_c ( "./data/jup282.bsp" );
   furnsh_c ( "./data/jup230-rocks.bsp" );
      
   furnsh_c ( "./data/sat353.bsp" );
     furnsh_c ( "./data/sat319.bsp" );
   //furnsh_c ( "sat351.bsp" );
   furnsh_c ( "./data/sat341.bsp" );
	furnsh_c ( "./data/sat353-rocks.bsp" );

	//furnsh_c ( "mar085.bsp" );

   furnsh_c ( "./data/nep085.bsp" );
  // furnsh_c ( "nep077.bsp" );
   furnsh_c ( "./data/nep081.bsp" );
	
   furnsh_c ( "./data/ura095.bsp" );
   furnsh_c ( "./data/ura083.bsp" );
   furnsh_c ( "./data/ura091.bsp" );

   furnsh_c ( "./data/plu021.bsp" );

   furnsh_c ( "./data/codes_300ast_20061020.bsp" );
	furnsh_c ( "./data/codes_300ast_20061020.tf");
	  furnsh_c  ( "./data/naif0010.tls" );*/

      printf( "Toolkit version %s\n", versn );
 

	  //spkobj_c ( "codes_300ast_20061020.bsp", &ids );
		  


 time_t rawtime;
  struct tm * timeinfo;
  char buffer [60];

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  //et = unitim_c ( rawtime, "TDT", "ET" );
  strftime(buffer,60,"%x %X",timeinfo);
  str2et_c(buffer,&et);
  
			

  glutVisibilityFunc(visible);

  
    /*
     * let AGV know if it can mess with the idle function (if we've
     * just installed an idle function, we tell AGV it can't touch it)
     */

  agvInit(0);
    
    /* 
     * agvInit() installs mouse, motion, and keyboard handles, but 
     * we don't care for this example cause we only use right button menu
     */

  agvMakeAxesList(AXES);  /* create AGV axes */

  myGLInit(); 
  MakeDisplayLists();
  MenuInit();
 //  glutCloseFunc(close); 
  glutDisplayFunc(display);

  glutMainLoop();



  getchar();
  return 0;             /* ANSI C requires main to return int. */
}


