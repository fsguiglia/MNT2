#pragma once
#include "ofMain.h"

class Point
{
public:
	Point();
	Point(ofVec2f position);
	void setPosition(ofVec2f position);
	void setPosition(float x, float y = 0);
	void setParameter(string key, float value);
	void setFeature(string key, float value);
	void deleteParameter(string key);
	void deleteFeature(string key);
	void setParameters(map<string, float> parameters);
	void setFeatures(map<string, float> features);
	void setName(string name);
	string getName();
	ofVec2f getPosition();
	bool hasParameter(string key);
	bool hasFeature(string key);
	float getParameter(string key);
	float getFeature(string key);
	map<string, float> getParameters();
	map<string, float> getFeatures();
	vector<pair<float, int>> getClosest(vector<ofVec2f>& positions, int n);
	pair<float,int> getClosest(vector<ofVec2f>& positions);
protected:
	map<string, float> _parameters;
	map<string, float> _features;
	ofVec2f _position;
	string _name;
};
