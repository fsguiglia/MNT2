#pragma once
#include "ofMain.h"
#include "../points/rgbPoint.h"
#include "BaseMap.h"

class RGBMap : public BaseMap<RGBPoint>
{
public:
	RGBMap();
	void setup(int width, int height);
	void update();
	void draw(int x, int y, ofTrueTypeFont& font);
	void draw(int x, int y, int w, int h, ofTrueTypeFont& font);

	int addPoint(ofVec2f position, ofImage img);
	int addPoint(ofVec2f position, ofImage img, string path);
	int addPoint(RGBPoint point);
	void resizePoint(int index, int w, int h);
	
	void setTrigger(int index, bool isTrigger);
	void setRadius(float radius);
	void setCursors(vector<ofVec2f> cursors);
	void setCursor(ofVec2f cursor, int index);
	vector<ofVec2f> getCursors();
	vector<pair<string, float>> getOutput();

	void selectFeatures(string xFeature, string yFeature);
private:
	void updateFbo();
	void updateOutput();

	float _radius;
	string _sizeText;
	vector<ofVec2f> _cursors;
	ofFbo _fbo;
};