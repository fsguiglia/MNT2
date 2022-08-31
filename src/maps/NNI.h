#pragma once
#include "ofMain.h"
#include "BaseMap.h"
#include "../points/point.h"

class NNI : public BaseMap<Point>
{
public:
	NNI();
	void setup(int width, int height);
	void setColorPallete(vector<ofColor> colorPallete);
	void update();
	void draw(int x, int y, ofTrueTypeFont& font);
	void draw(int x, int y, int w, int h, ofTrueTypeFont& font);
	void drawIdFbo(int x, int y);
	void drawInterpolationFbo(int x, int y);
	int addPoint(ofVec2f pos);
	void generatePoints();
	void setCursor(ofVec2f cursor);
	void setCursor(ofVec2f cursor, int index);
	ofVec2f getCursor();
	vector<ofVec2f> getCursors();
	void setDrawInterpolation(bool drawInterpolation);
	vector<pair<string, float>> getOutput();
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
};
