#include "gesture.h"

Gesture::Gesture()
{
	_lengthMs = 0;
}

void Gesture::addPoint(float time, Point point)
{
	point.setValue("t", time);
	_points.push_back(point);
	_polyline.addVertex(point.getPosition().x, point.getPosition().y);
}

Point Gesture::getPoint(int index)
{
	Point point;
	if (index < _points.size()) point = _points[index];
	return point;
}

vector<Point> Gesture::getPoints()
{
	return _points;
}

void Gesture::deletePoint(int index)
{
	_points.erase(_points.begin() + index);
}

ofPolyline Gesture::getPolyline()
{
	return _polyline;
}

void Gesture::draw(int x, int y, ofColor color)
{
	ofPushStyle();
	ofPushMatrix();
	ofTranslate(x, y);
	ofSetColor(color);
	_polyline.draw();
	ofPopMatrix();
	ofPopStyle();
}

void Gesture::sort()
{
	if (_points.size() > 0)
	{
		vector<pair<int, int>> timeStamps;
		for (int i = 0; i < _points.size(); i++)
		{
			pair<int, int> curValue;
			curValue.first = _points[i].getValue("t");
			curValue.second = i;
			timeStamps.push_back(curValue);
		}
	
		std::sort(timeStamps.begin(), timeStamps.end());
		vector<Point> sorted;
		for (auto time : timeStamps) sorted.push_back(_points[time.second]);
		int firstValue = timeStamps[0].first;
		for (auto& point : _points)	point.setValue("t", point.getValue("t") - firstValue);
		_lengthMs = _points[_points.size() - 1].getValue("t");
	}
}

void Gesture::normalizeTimes()
{
	float min = NULL;
	float max = NULL;
	for (auto point : _points)
	{
		float curTime = point.getValue("t");
		if (min == NULL || curTime < min) min = curTime;
		if (max == NULL || curTime > max) max = curTime;
	}
	for (auto& point : _points)
	{
		float normalized = (point.getValue("t") - min) / (max - min);
		point.setValue("t", normalized);
	}
}

ofJson Gesture::save()
{
	ofJson jSave;
	for (auto point : _points)
	{
		ofJson jPoint, parameters;
		jPoint["time"] = point.getValue("t");
		jPoint["x"] = point.getPosition().x;
		jPoint["y"] = point.getPosition().y;
		/*
		//in case i want to save some other parameter
		for (auto parameter : point.getValues())
		{
			if(parameter.first != "t") parameters[parameter.first] = parameter.second;
		}
		jPoint["parameters"] = parameters;
		*/
		jSave.push_back(jPoint);
	}
	return jSave;
}

void Gesture::load(ofJson loadFile, bool sortPoints, bool normalizeTimeStamps)
{
	for (auto curPoint : loadFile)
	{
		int time = curPoint["time"];
		Point point;
		point.setPosition(curPoint["x"], curPoint["y"]);
		/*
		//in case we want to load some other parameter
		auto obj = curPoint["parameters"].get<ofJson::object_t>();
		for (auto parameter : obj)
		{
			point.setValue(parameter.first, parameter.second);
		}
		*/
		addPoint(time, point);
	}
}

void Gesture::clear()
{
	_polyline.clear();
	_points.clear();
}
