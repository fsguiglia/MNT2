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
	void update();
	void draw(int x, int y, ofTrueTypeFont& font);
	void draw(int x, int y, int w, int h, ofTrueTypeFont& font);

	void addTrigger(ofVec2f position, float radius, float threshold, ofColor color);
	void addTrigger(ofVec2f position, float radius, float threshold);
	void setRadius(int index, float radius);
	void setThreshold(int index, float threshold);
	vector<map<string, float>> getTriggeredValues();
private:
	ofColor _color;
	vector<ofVec2f> _cursors;
	vector<int> _triggered;
};
#endif