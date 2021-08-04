#ifndef _GESTURE
#define _GESTURE
#include "ofMain.h"
#include "../points/point.h"

class Gesture {
public:
	Gesture();
	void addPoint(float time, Point point);
	Point getPoint(int index);
	vector<Point> getPoints();
	void deletePoint(int index);
	
	ofPolyline getPolyline();
	void draw(int x, int y, ofColor color = ofColor::black);

	void sort();
	void normalizeTimes();

	ofJson save();
	void load(ofJson loadFile, bool sortPoints = true, bool normalizeTimeStamps = true);
	void clear();
private:
	vector<Point> _points;
	int _lengthMs;
	ofPolyline _polyline;
};

#endif
