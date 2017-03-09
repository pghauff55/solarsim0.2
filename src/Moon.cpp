
#include <glut.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "Planet.h"

extern GLuint asteroidtex;
extern GLfloat EyeEl,EyeAz,EyeDist;
extern SpiceDouble dt;
extern SpiceDouble et;

Moon::Moon(int p,int i,SpiceDouble Ra,SpiceDouble rd){
	planet=p;SpiceInt dim;
	moon_id=new char[101];
	int ret=sprintf(moon_id,"%i%02i",p,i+1);
	if(ret>=0){
	
	bodvcd_c ( StringToNumber(moon_id), "RADII", 3, &dim, rad );
	
	if ( !failed_c() && dim==3)
             {

	rad[0]=pow(abs(rad[0]),0.5)/300.0;
	rad[1]=pow(abs(rad[1]),0.5)/300.0;
	rad[2]=pow(abs(rad[2]),0.5)/300.0;

	scale=sqrt(rad[0]*rad[0]+rad[1]*rad[1]+rad[2]*rad[2]);
	}
	else {

		scale=0.0;
	}
	bodc2s_c ( StringToNumber(moon_id), 100, moon_id);
	}
	mt=0.0;
	this->Ra=Ra;
	this->rd=rd;

}

Moon::~Moon(){
	delete moon_id;
}

void Moon::Draw(){
	
	glPushMatrix();
//	glRotatef(90.0,1,0,0);
		glTranslatef(x,y,z);
		
		if(scale!=0.0){
			renderBitmapString(0,scale,0,moon_id,0.0005) ;
			glScalef(0.5*scale,0.5*scale,0.5*scale);
			}
		else {
			renderBitmapString(0,0.1,0,moon_id,0.0005) ;
			glScalef(0.05,0.05,0.05);
			}
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,asteroidtex);
		glCallList(SPHERE);
		glDisable(GL_TEXTURE_2D); 
	glPopMatrix();



}


bool Moon::inSight(baryCentre *centre){
	SpiceDouble ex,ey,ez;
	SpiceDouble st=sin(EyeAz*M_PI/180.0),sp=sin(EyeEl*M_PI/180.0),ct=cos(EyeAz*M_PI/180.0),cp=cos(EyeEl*M_PI/180.0);
			ex=centre->cx+EyeDist*ct*cp;
			ey=centre->cy+EyeDist*sp;
			ez=centre->cz+EyeDist*st*cp;
			SpiceDouble vx=parent->cx+x-ex,vy=parent->cy+y-ey,vz=parent->cz+z-ez;
			SpiceDouble dv=sqrt(vx*vx+vy*vy+vz*vz);
			if(dv>100.0) return false;
	float dotprod=ex*vx+ey*vy+ez*vz;
	if(dotprod>0.0)return false;
	else return true;
}

void Moon::UpdatePosition(baryCentre *parent){
	this->parent=parent;
	SpiceDouble pos[3],lt;
   	spkpos_c ( moon_id,    et,   "J2000", "NONE",parent->planet->name,  pos,  &lt );
   	
	reclat_c(pos,&r,&lon,&colat);
	r=pow(r/AU,0.9);
	x=Ra*(r+rd)*cos(lon)*cos(colat);
	y=Ra*(r+rd)*sin(colat);
	z=Ra*(r+rd)*sin(lon)*cos(colat);
	
	
}

void Moon::DrawOrbitMarkers(){
	glPushMatrix();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glRotatef(90.0,1,0,0);
	glBegin(GL_POINTS);
	int N=20;
	for(int i=0;i<N;i++){
		SpiceDouble r,colat,lon;
			reclat_c(marker_pos[i],&r,&lon,&colat);
			r=r/AU;
			r=pow(r,0.9);
			glVertex3f(Ra*(r+rd)*cos(lon)*cos(colat),Ra*(r+rd)*sin(colat),Ra*(r+rd)*sin(lon)*cos(colat));
	}
	glEnd();

	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}
void Moon::CalcOrbitMarkers(baryCentre *parent){
	
	int N=20;	
	//if(this->r<1.0)this->r=1.0;
	SpiceDouble tFactor=1000.0*M_PI*pow(r,1.5)*dt/parent->planet->radius[0];
	if((et-mt)>tFactor){
		mt=et;
	  for(int i=0;i<N;i++){
			SpiceDouble pt=et+float(i-N/2)*tFactor,lt;
			spkpos_c ( moon_id,    pt,   "J2000", "NONE",parent->name, marker_pos[i],  &lt );
			
			}
	
	
	}
}

/*

 ////////////////////////////////////////////////jupiters moons
   glPushMatrix();
   
   	 pxform_c ( "J2000", names1[5], et, mat );
   vin[0]=mat[0][2];vin[1]=mat[1][2];vin[2]=mat[2][2];
   recsph_c (vin,&r,&ra,&dec);
		 spkpos_c ( "5",    et,   "J2000", "NONE",CENTRE,  pos,  &lt );
   		glTranslatef(Ra*pos[0]/AU,Ra*pos[2]/AU,Ra*pos[1]/AU);

		angle1=dec*180.0/M_PI;
		angle2=ra*180.0/M_PI;
		glRotatef(270-angle2,1,0,0);
		//glRotatef(angle1,0,1,0);    

		

   for(int i=0;i<43;i++){
	   char moon_id[100];
	   sprintf_s(moon_id,"5%02d",i+1);
	      bodc2s_c ( atoi(moon_id), 100, moon_id );
	   glPushMatrix();

		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBegin(GL_POINTS);
		SpiceDouble oN=200.0;
	  SpiceDouble oM=10.0;
	  spkpos_c ( moon_id,    et,   "J2000", "NONE","5",  pos1,  &lt );
	//  spkpos_c ( moon_id,    et+0.01*dt,   "J2000", "NONE","5",  pos2,  &lt );
	  
	  recrad_c(pos1,&r,&colat,&lon);r/=AU;
	 // SpiceDouble vdist=sqrt((pos1[0]-pos2[0])*(pos1[0]-pos2[0])+(pos1[1]-pos2[1])*(pos1[1]-pos2[1])+(pos1[2]-pos2[2])*(pos1[2]-pos2[2]));	
	  SpiceDouble Rfactor=10.0*M_PI*pow(r,1.5);
	  
		for(SpiceDouble pt=et-Rfactor*oM*oN*dt;pt<(et+Rfactor*oM*oN*dt);pt+=Rfactor*oN*dt){
			//spkpos_c ( "5",    pt,   "J2000", "NONE",CENTRE,  pos,  &lt );
			spkpos_c ( moon_id,    pt,   "J2000", "NONE","5",  pos1,  &lt );
			recrad_c(pos1,&r,&colat,&lon);
			r=r/AU;
			r=pow(r,0.9);
				glVertex3f(R1*(r+rd)*cos(lon)*cos(colat),R1*(r+rd)*sin(colat),R1*(r+rd)*sin(lon)*cos(colat));
			//glVertex3f(Ra*pos[0]/AU,Ra*pos[2]/AU,Ra*pos[1]/AU);
			}
		glEnd();
	  glDisable(GL_BLEND);
	  glEnable(GL_TEXTURE_2D);
	  glEnable(GL_LIGHTING);
		glPopMatrix();
	
		spkpos_c ( moon_id,    et,   "J2000", "NONE","5",  pos1,  &lt );
   //D2=sqrt(pos1[0]*pos1[0]+pos1[2]*pos1[2]+pos1[1]*pos1[1]);
   ///norm1[0]=pos1[0]/D2;norm1[1]=pos1[1]/D2;norm1[2]=pos1[2]/D2;
   recrad_c(pos1,&r,&colat,&lon);
   r=r/AU;
   r=pow(r,0.9);
  glPushMatrix();
    	glTranslatef(R1*(r+rd)*cos(lon)*cos(colat),R1*(r+rd)*sin(colat),R1*(r+rd)*sin(lon)*cos(colat));
   glScalef(0.05,0.05,0.05);
  glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D,tex[10]);
    glCallList(ASTEROID);
  glDisable(GL_TEXTURE_2D); 
  
	glRotatef(-270+angle2,1,0,0);
  // renderBitmapString(0.0f, 1.1f, 0.0f, moon_id,0.01);		 
   glPopMatrix();

   }
   glPopMatrix();


   */