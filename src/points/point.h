#ifndef _POINT
#define _POINT
#include "ofMain.h"

class Point
{
public:
	Point();
	Point(ofVec2f position);
	void setPosition(ofVec2f position);
	void setPosition(float x, float y = 0);
	void setValue(string key, float value);
	void deleteValue(string key);
	void setValues(map<string, float> parameters);
	void setClass(int class_);
	ofVec2f getPosition();
	bool hasValue(string key);
	float getValue(string key);
	map<string, float> getValues();
	int getClass();
	vector<pair<float, int>> getClosest(vector<ofVec2f>& positions, int n);
	pair<float,int> getClosest(vector<ofVec2f>& positions);
protected:
	map<string, float> _parameters;
	ofVec2f _position;
	int _class;
};
#endif
