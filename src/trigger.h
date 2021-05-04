#ifndef _TRIGGER
#define _TRIGGER
#include "ofMain.h"
#include "point.h"

/*
tiene que haber una manera mas inteligente de armar
el trigger que replicando todas las funciones invalue, outvalue, etc
a lo mejor son dos points?
*/

class Trigger : public Point {
public:
	Trigger();
	Trigger(ofVec2f position, float radius, float threshold);
	void setRadius(float radius);
	void setThreshold(float threshold);
	void setState(bool state);
	void setColor(ofColor color);
	void setOutValue(string key, float value);
	void setOutValues(map<string, float> parameters);
	void update(ofVec2f position);
	void update(vector<ofVec2f>& positions);
	bool getState();
	void draw();
private:
	map<string, float> _outParameters;
	float _radius;
	float _threshold;
	bool _state;
	ofColor _color;
};
#endif