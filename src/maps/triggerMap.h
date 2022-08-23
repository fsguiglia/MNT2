#pragma once
#include "ofMain.h"
#include "../points/trigger.h"
#include "BaseMap.h"

class TriggerMap : public BaseMap<Trigger>{

public:
	TriggerMap();
	void setup(int width, int height);
	void update();
	void draw(int x, int y, ofTrueTypeFont& font);
	void draw(int x, int y, int w, int h, ofTrueTypeFont& font);

	int addPoint(ofVec2f position, float radius, float threshold, ofColor color, bool isSwitch);
	int addPoint(ofVec2f position, float radius, float threshold, bool isSwitch);
	void setRadius(int index, float radius);
	void setThreshold(int index, float threshold);
	void setSwitch(int index, bool isSwitch);
	void setCursors(vector<ofVec2f> cursors);
	void setCursor(ofVec2f cursor, int index);
	vector<ofVec2f> getCursors();
	vector<int> getTriggered();
	vector<pair<string, float>> getOutput();
private:
	void updateFbo();
	void updateTriggers();

	vector<ofVec2f> _cursors;
	vector<int> _triggered;
	ofFbo _fbo;
};