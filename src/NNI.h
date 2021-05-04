#ifndef _NNI
#define _NNI
#include "ofMain.h"
#include "point.h"
#include "map.h"
#define MAX_SITES 128
#define ID_FBO_RESOLUTION 600

/*
random walk, en cpu o gpu? optimizar
rendimiento -> cuanto pierdo con el render de la zona a interpolar, en que gpu es mejor
colores
manejar parametros
*/

class NNI : protected Map
{
public:
	NNI();
	void setup(int width, int height);
	void add(map<string, float> values, ofVec2f position);
	void addParameter(string parameter, float value);
	void move(int index, ofVec2f pos);
	void randomize(float speed);
	void remove(int index);
	void remove(ofVec2f pos);
	array<float, 2> getClosest(ofVec2f pos);
	vector<Point> getSites();
	int getWidth();
	int getHeight();
	set<string> getParameters();
	void draw(int x, int y);
	void drawId(int x, int y);
	void drawInterpolation(int x, int y);
	map<string, float> interpolate(ofVec2f pos, bool renderNewZone = false);
private:
	void update(ofFbo& fbo, int mode, int interpolate, vector<Point>& sites);
	void updateIdMap();

	int _width, _height;
	bool _updateIdMap;
	ofShader _voronoi;
	ofTexture _tex;
	ofBufferObject _buffer;
	ofVboMesh _mesh;
	vector<glm::mat4> _matrices;
	ofFbo _colorFbo, _idFbo, _interpolateFbo;
	ofPixels _curState, _nniPixels;

	vector<Point> _sites;
	set<string> _parameters;
};

#endif // !_NNI
