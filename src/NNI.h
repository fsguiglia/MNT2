#ifndef _NNI
#define _NNI
#include "ofMain.h"
#include "point.h"
#include "map.h"

/*
random walk, en cpu o gpu? optimizar
rendimiento -> cuanto pierdo con el render de la zona a interpolar, en que gpu es mejor
colores
manejar parametros
*/

class NNI : private Map
{
public:
	NNI();
	void setup(int width, int height);
	void add(ofVec2f position);
	void addParameter(string parameter, float value);
	void setParameter(string parameter, float value);
	void setParameter(int site, string parameter, float value);
	void removeParameter(string parameter);
	void move(int index, ofVec2f pos);
	void randomize(float speed);
	void remove(int index);
	void remove(ofVec2f pos);
	void clear();
	array<float, 2> getClosest(ofVec2f pos);
	vector<Point> getSites();
	Point getSite(int index);
	int getWidth();
	int getHeight();
	map<string, float> getParameters();
	void draw(int x, int y);
	void draw(int x, int y, int w, int h);
	void drawId(int x, int y);
	void drawInterpolation(int x, int y);
	map<string, float> interpolate(ofVec2f pos, bool renderNewZone = false);
private:
	void update(ofFbo& fbo, int mode, int interpolate, vector<Point>& sites);
	void updateIdMap();
	
	const int maxSize = 128;
	const int idFboResolution = 200;

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
	map<string, float> _parameters;
};

#endif // !_NNI
