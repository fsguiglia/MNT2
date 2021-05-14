#include "triggerMap.h"

TriggerMap::TriggerMap()
{
}

void TriggerMap::setup(int width, int height)
{
	_width = width;
	_height = height;
}

void TriggerMap::update()
{
	_triggered.clear();
	for (int i = 0; i < _points.size(); i++)
	{
		bool curState = _points[i].getState();
		_points[i].setState(_cursors);
		if (!curState && _points[i].getState()) _triggered.push_back(i);
	}
}

void TriggerMap::draw(int x, int y, ofTrueTypeFont & font)
{
	draw(x, y, _width, _height, font);
}

void TriggerMap::draw(int x, int y, int w, int h, ofTrueTypeFont & font)
{
	ofPushStyle();
	for (auto point : _points)
	{
		ofColor inColor, outColor;
		ofVec2f position = point.getPosition();
		float radius = point.getRadius();
		float threshold = point.getThreshold();
		
		if (point.getState()) inColor = ofColor(180, 0, 0);
		else inColor = _color;
		outColor = inColor;
		outColor.a = 100;
		
		ofSetColor(outColor);
		ofDrawCircle(position, point.getRadius());
		ofSetColor(inColor);
		ofDrawCircle(position, point.getRadius() * threshold);

		if (_lastSelected >= 0 && _lastSelected < _points.size() && _drawSelected)
		{
			if (ofGetElapsedTimeMillis() - _lastSelectedMs < 1000)
			{
				drawSelected(x, y, w, h, font);
			}
		}
	}
	ofPopStyle();
}

void TriggerMap::addTrigger(ofVec2f position, float radius, float threshold, ofColor color)
{
	Trigger trigger;
	trigger.setPosition(position);
	trigger.setRadius(radius);
	trigger.setThreshold(threshold);
	trigger.setColor(color);
	addPoint(trigger);
}

void TriggerMap::addTrigger(ofVec2f position, float radius, float threshold)
{
	addTrigger(position, radius, threshold, _color);
}

void TriggerMap::setRadius(int index, float radius)
{
	_points[index].setRadius(radius);
}

void TriggerMap::setThreshold(int index, float threshold)
{
	_points[index].setThreshold(threshold);
}

vector<map<string, float>> TriggerMap::getTriggeredValues()
{
	vector<map<string, float>> triggeredValues;
	for (auto index : _triggered) triggeredValues.push_back(_points[index].getValues());
	return triggeredValues;
}
