#ifndef _GESTURE
#define _GESTURE
#include "ofMain.h"
#include "../points/point.h"

/*
cuidado con dos puntos en una misma posicion->reemplaza?
la posicion del punto esta dada por su propia posicion o mejor un map<int, point> con timetag?
o bien tiene un periodo fijo y asume que los puntos estan ordenados (index * T)
*/

class Gesture {
public:
	Gesture();
	void setup(int period = 0);
	void update();

	void stage(Point point);
	void stage(map<string, float> parameters);
	void setPoints(vector<Point> points);
	vector<Point> getPoints();

	void start();
	void end();
	
private:
	int _Tms, _startTime, _lastSample;
	bool _recording;
	map<string, float> _parameters;
	vector<Point> _points;
	Point _curPoint;
};

#endif
