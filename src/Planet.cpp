#include <Windows.h>
#include <glut.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "Planet.h"

extern GLfloat EyeEl,EyeAz,EyeDist;
extern SpiceDouble dt;
extern SpiceDouble et;


int StringToNumber(const char *buffer)
{
   int result = 0;
   int startIndex = 0;
   bool negativeNumber = false;
   if(buffer[0] == '-')
   {
         negativeNumber = true;
         startIndex = 1;
   }
   for(int i = startIndex; i < strlen(buffer); i++)
   {
      if(buffer[i] >= '0' && buffer[i] <= '9')
      {
         int digit = buffer[i] - '0';
         result = result * 10 + digit;
      }
      else
         return 0;
   }
   if(negativeNumber == true)
      result *= -1;
   return result;
}





Planet::Planet(int planet_id){
	SpiceInt dim;
	char text[20];
	if(planet_id!=0)sprintf(text,"%d99",planet_id);
	else sprintf(text,"10");
	bodc2s_c ( StringToNumber(text), 100, name );
	bodvcd_c ( StringToNumber(text), "RADII", 3, &dim, radius );
	radius[0]=pow(radius[0],0.5)/300;
	radius[1]=pow(radius[1],0.5)/300;
	radius[2]=pow(radius[2],0.5)/300;
		scale=sqrt(radius[0]*radius[0]+radius[1]*radius[1]+radius[2]*radius[2]);
}
void Planet::Translate(){
	glTranslatef(x,y,z);
}
void Planet::Draw(){

	glPushMatrix();
  glRotatef(270+180.0*colatitude/M_PI,1,0,0);
  glRotatef(180.0*longitude/M_PI,0,1,0);   
  glTranslatef(x,y,z);
  glScalef(radius[0],radius[2],radius[1]);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,texture);
    if(parent->id==0)glCallList(INV_NORMAL_SPHERE);
		else glCallList(SPHERE);
   glDisable(GL_TEXTURE_2D);
   glPopMatrix();

}

void Planet::UpdateOrient(){
	char text[100];
	SpiceDouble mat[3][3],vin[3],r;
	sprintf(text,"IAU_%s",this->name);
	  pxform_c ( "J2000", text, et, mat );
	  vin[0]=mat[0][2];vin[1]=mat[1][2];vin[2]=mat[2][2];
	  reclat_c (vin,&r,&longitude,&colatitude);
	
}
void Planet::UpdatePos(baryCentre *parent){
	this->parent=parent;
	SpiceDouble pos[3],lt;
   	spkpos_c ( name,    et,   "J2000", "NONE",parent->name,  pos,  &lt );
   	SpiceDouble r,colat,lon;
	recsph_c(pos,&r,&colat,&lon);
	r=50.0*r/AU;
	x=r*cos(lon)*cos(colat);
	y=r*sin(colat);
	z=r*sin(lon)*cos(colat);


}

char *names1[]={"IAU_SUN","IAU_MERCURY","IAU_VENUS","IAU_EARTH","IAU_MARS","IAU_JUPITER","IAU_SATURN","IAU_URANUS","IAU_NEPTUNE","IAU_MOON"};
baryCentre::baryCentre(int id,int num_moons,Planet *p){
	this->id=id;
	sprintf(name,"%d",id);
	this->num_moons=num_moons;
	for(int i=0;i<num_moons;i++){
		printf("%d:",i);
		moons[i]=new Moon(id,i,100.0,0.01);

	}
	this->planet=p;
	bt=0.0;
	scale=30.0;
}
baryCentre::~baryCentre(){

		for(int i=0;i<num_moons;i++)delete moons[i];

}

void baryCentre::UpdatePos(baryCentre *centre){
	SpiceDouble pos[3],lt;
	
	   spkpos_c ( this->name,    et,   "J2000", "NONE",centre->name,  pos,  &lt );
	   cx=scale*pos[0]/AU;
	   cy=scale*pos[2]/AU;
	   cz=scale*pos[1]/AU;
	   r=sqrt(cx*cx+cy*cy+cz*cz);
	   

}
void baryCentre::DrawOrbitMarkers(baryCentre *centre){
	glPushMatrix();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	SpiceDouble pos1[3],lt;
	spkpos_c ( centre->name,    et,   "J2000", "NONE","0", pos1,  &lt );
	pos1[0]=scale*pos1[0]/AU;
	pos1[2]=scale*pos1[2]/AU;
	pos1[1]=scale*pos1[1]/AU;
	glTranslatef(-pos1[0],-pos1[2],-pos1[1]);
	glBegin(GL_POINTS);
	int N=150;
	for(int i=0;i<N;i++){
		SpiceDouble mx=scale*marker_pos[i][0]/AU,my=scale*marker_pos[i][2]/AU,mz=scale*marker_pos[i][1]/AU;
			glVertex3f(mx,my,mz);
	}
	glEnd();

	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}
void baryCentre::CalcOrbitMarkers(){
	int N=150;	
	if(this->r/scale<1.0)this->r=30.0;
	SpiceDouble tFactor=abs(1800000.0*M_PI*pow(this->r/scale,1.9)*dt);
	if(abs(et-bt)>tFactor/20.0){
		bt=et;
		
	  for(int i=0;i<N;i++){
			SpiceDouble pt=et+float(i-N/2)*tFactor,lt;
			spkpos_c ( this->name,    pt,   "J2000", "NONE","0", marker_pos[i],  &lt );

			
			}
	} 

}
void baryCentre::Translate(){
	glTranslatef(cx,cy,cz);
}

void baryCentre::Draw(baryCentre *centre){
	glPushMatrix();
	planet->UpdatePos(this);
	planet->UpdateOrient();
	planet->Draw();
	glPopMatrix();
	glPushMatrix();
	planet->Translate();
	if(this==centre){
		for(int i=0;i<num_moons;i++){
		this->moons[i]->UpdatePosition(this);
		if(moons[i]->inSight(centre)){
				this->moons[i]->CalcOrbitMarkers(this);
				this->moons[i]->DrawOrbitMarkers();
				this->moons[i]->Draw();
			}
		}
	}
	glPopMatrix();
}


GLuint Planet::LoadTexture( const char * filename, GLuint wrap ,GLuint modulate)
{ 
	char path[100];
	GetCurrentDirectory(100,path);
	strcat(path,filename);
	this->texture=LoadTextureRAW( path,  wrap , modulate);
	return texture;
}

GLuint LoadTextureRAW( const char * filename, GLuint wrap ,GLuint modulate)

{
	// load a 256x256 RGB .RAW file as a texture
	GLuint texture;
    int width, height;
    char *data;
    FILE *file;

    // open texture data
    int ret = fopen_s(&file, filename, "rb" );
	if ( ret != 0 ) {texture=0;return texture;}

    // allocate buffer
    width = 256;
    height = 256;
    data = new char[ width * height * 3 ];

    // read texture data
    fread( data, width * height * 3, 1, file );
    fclose( file );

    // allocate a texture name
    glGenTextures( 1, &texture );

    // select our current texture
    glBindTexture( GL_TEXTURE_2D, texture );

    // select modulate to mix texture with color for shading
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, modulate );

    // when texture area is small, bilinear filter the closest mipmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                     GL_LINEAR_MIPMAP_NEAREST );
    // when texture area is large, bilinear filter the first mipmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    // if wrap is true, the texture wraps over at the edges (repeat)
    //       ... false, the texture ends at the edges (clamp)
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                     wrap);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                     wrap);

    // build our texture mipmaps
    gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height,
                       GL_RGB, GL_UNSIGNED_BYTE, data );

    // free buffer
    delete( data );
	
    
	return texture;
	
	
}
