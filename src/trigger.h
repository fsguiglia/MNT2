#ifndef _TRIGGER
#define _TRIGGER
#include "ofMain.h"
#include "point.h"

class Trigger : public Point {
public:
	Trigger();
	Trigger(ofVec2f position, float radius, float threshold, bool isSwitch);
	void setRadius(float radius);
	float getRadius();
	void setThreshold(float threshold);
	float getThreshold();
	void setSwitch(bool _switch);
	bool getSwitch();
	void setState(bool state);
	bool setState(vector<ofVec2f> cursors);
	void setColor(ofColor color);
	ofColor getColor();
	void update(ofVec2f position);
	void update(vector<ofVec2f>& positions);
	bool getState();
private:
	float _radius;
	float _threshold;
	bool _state, _switch;
	ofColor _color;
};
#endif