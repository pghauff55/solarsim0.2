#include "SpiceUsr.h"
int StringToNumber(const char *buffer);
void renderBitmapString(float x,float y,float z,char *string,float scale) ;

#define AU 149597870.7
typedef enum {NOTALLOWED, AXES, STUFF, RING,SPHERE,INV_NORMAL_SPHERE ,ASTEROID} DisplayLists;
class baryCentre;
class Moon {
	SpiceDouble rad[3];
	baryCentre *parent;
	SpiceDouble mt,scale;
	SpiceDouble marker_pos[20][3];
	SpiceDouble rd;
	int planet;
	char *moon_id;
	SpiceDouble r,colat,lon;
	//SpiceDouble et; 
	SpiceDouble x,y,z,Ra;
public:
	Moon::Moon(int planet,int i,SpiceDouble Ra,SpiceDouble rd);
	Moon::~Moon();

	void UpdatePosition(baryCentre *parent);
	void CalcOrbitMarkers(baryCentre *parent);
	void DrawOrbitMarkers();
	void Draw();
	bool inSight(baryCentre *centre);

};