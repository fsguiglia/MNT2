#ifndef _RGBMAP
#define _RGBMAP
#include "ofMain.h"
#include "rgbPoint.h"
#include "map.h"

class RGBMap : public Map<RGBPoint>
{
public:
	RGBMap();
	void setup(int width, int height);
	void update();
	void draw(int x, int y, ofTrueTypeFont& font);
	void draw(int x, int y, int w, int h, ofTrueTypeFont& font);
	int addPoint(ofVec2f position, ofImage img);
	int addPoint(ofVec2f position, ofImage img, string path);
	void resizePoint(int index, int w, int h);
	void setTrigger(int index, bool isTrigger);
	void setRadius(float radius);
	void setCursors(vector<ofVec2f> cursors);
	void setCursor(ofVec2f cursor, int index);
	vector<ofVec2f> getCursors();
	map<string, float> getOutput();
private:
	void updateFbo();
	void updateOutput();

	float _radius;
	vector<ofVec2f> _cursors;
	ofFbo _fbo;
};

#endif