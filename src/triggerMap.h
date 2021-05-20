#ifndef _TRIGGERMAP
#define _TRIGGERMAP
#include "ofMain.h"
#include "point.h"
#include "trigger.h"
#include "map.h"

class TriggerMap : public Map<Trigger>{

public:
	TriggerMap();
	void setup(int width, int height);
	void setColor(ofColor color);
	void update();
	void draw(int x, int y, ofTrueTypeFont& font);
	void draw(int x, int y, int w, int h, ofTrueTypeFont& font);

	void addPoint(ofVec2f position, float radius, float threshold, ofColor color);
	void addPoint(ofVec2f position, float radius, float threshold);
	void setRadius(int index, float radius);
	void setThreshold(int index, float threshold);
	void setSwitch(int index, bool isSwitch);
	void setCursors(vector<ofVec2f> cursors);
	void setCursor(ofVec2f cursor, int index);
	vector<ofVec2f> getCursors();
	vector<int> getTriggered();
	map<string, float> getOutput();
private:
	void updateFbo();
	void updateTriggers();

	ofColor _color;
	vector<ofVec2f> _cursors;
	vector<int> _triggered;
	ofFbo _fbo;
};
#endif