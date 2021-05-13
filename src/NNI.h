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

class NNI : public Map<Point>
{
public:
	NNI();
	void setup(int width, int height);
	void update();
	void draw(int x, int y, ofTrueTypeFont& font);
	void draw(int x, int y, int w, int h, ofTrueTypeFont& font);
	void drawIdFbo(int x, int y);
	void drawInterpolationFbo(int x, int y);
	
	void setCursor(ofVec2f cursor);
	ofVec2f getCursor();
	void setDrawInterpolation(bool drawInterpolation);
	map<string, float> getWeights();
private:
	map<string, float> interpolate(ofVec2f pos, bool renderNewZone = false);
	void update(ofFbo& fbo, int mode, int interpolate, vector<Point>& sites);
	void updateIdMap();
	
	const int maxSize = 128;
	const int idFboResolution = 200;

	ofVec2f _cursor;

	bool _updateIdFbo, _drawInterpolation;
	ofShader _voronoi;
	ofTexture _tex;
	ofBufferObject _buffer;
	ofVboMesh _mesh;
	vector<glm::mat4> _matrices;
	ofFbo _colorFbo, _idFbo, _interpolateFbo;
	ofPixels _curState, _nniPixels;
	map<string, float> _weights;
};

#endif // !_NNI
