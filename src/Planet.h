#include "Moon.h"
class Planet;

class baryCentre{
public:
	SpiceDouble bt,r,scale;
	SpiceDouble marker_pos[201][3];
	Moon *moons[70];int num_moons;
	Planet *planet;
	char name[100];
	int id;
	SpiceDouble cx,cy,cz;
	
	baryCentre::baryCentre(int id,int num_moons,Planet *p);
	baryCentre::~baryCentre();
	void DrawOrbitMarkers(baryCentre *centre);
	void CalcOrbitMarkers();
	void UpdatePos(baryCentre *centre);
	void Translate();
	void Draw(baryCentre *centre);
};




class Planet{
	public:
	baryCentre *parent;
	char name[100];
	
	GLuint texture;
	SpiceDouble x,y,z;
	SpiceDouble colatitude,longitude;//radians
	SpiceDouble radius[3];
	SpiceDouble scale;
	Planet::Planet(int id);
	void UpdateOrient();
	void UpdatePos(baryCentre *parent);
	void Draw();
	void Translate();
	GLuint LoadTexture( const char * filename, GLuint wrap ,GLuint modulate);
};
GLuint LoadTextureRAW( const char * filename, GLuint wrap ,GLuint modulate);