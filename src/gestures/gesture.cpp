#include "gesture.h"

Gesture::Gesture()
{
	_recording = false;
	_curPoint = Point();
}

void Gesture::setup(int period)
{
	_Tms = period;
}

void Gesture::update()
{
	if (_recording && ofGetElapsedTimeMillis() - _lastSample >= _Tms)
	{
		_points.push_back(_curPoint);
		_points[_points.size() - 1].setPosition(ofGetElapsedTimeMillis());
		_lastSample = ofGetElapsedTimeMillis();
	}
}

void Gesture::stage(Point point)
{
	_curPoint = point;
}

void Gesture::stage(map<string, float> parameters)
{
	Point point;
	point.setValues(parameters);
	stage(point);
}

void Gesture::setPoints(vector<Point> points)
{
	_points = points;
}

vector<Point> Gesture::getPoints()
{
	return _points;
}

void Gesture::start()
{
	_recording = true;
	_startTime = ofGetElapsedTimeMillis();
	_lastSample = _startTime;
	_points.clear();
}

void Gesture::end()
{
	_recording = false;
}
