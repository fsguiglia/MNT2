#include "triggerMap.h"

TriggerMap::TriggerMap()
{
}

void TriggerMap::setup(int width, int height)
{
	_width = width;
	_height = height;
	_fbo.allocate(_width, _height);
	ofPushStyle();
	_fbo.begin();
	ofClear(255);
	ofSetColor(255);
	ofDrawRectangle(0, 0, _width, _height);
	_fbo.end();
	ofPopStyle();
	_triggered.assign(_points.size(), 0);
}

void TriggerMap::update()
{
	if (_randomSpeed != 0) randomize();
	if (_active) updateTriggers();
	updateFbo();
}

void TriggerMap::draw(int x, int y, ofTrueTypeFont & font)
{
	draw(x, y, _width, _height, font);
}

void TriggerMap::draw(int x, int y, int w, int h, ofTrueTypeFont & font)
{
	ofPushStyle();
	ofSetColor(255);
	_fbo.draw(x, y, w, h);
	ofSetColor(0);
	for (int i = 0; i < _points.size(); i++)
	{
		ofVec2f pos = _points[i].getPosition() * ofVec2f(w, h);
		ofDrawBitmapString(ofToString(i), pos + ofVec2f(x, y));
	}

	if (_lastSelected >= 0 && _lastSelected < _points.size() && _drawSelected)
	{
		if (ofGetElapsedTimeMillis() - _lastSelectedMs < 1000)
		{
			drawSelected(x, y, w, h, font, 200);
		}
	}
	ofPopStyle();
}

int TriggerMap::addPoint(ofVec2f position, float radius, float threshold, ofColor color)
{
	Trigger trigger;
	trigger.setPosition(position);
	trigger.setRadius(radius);
	trigger.setThreshold(threshold);
	trigger.setColor(color);
	trigger.setSwitch(false);
	BaseMap::addPoint(trigger);
	return _points.size() - 1;
}

int TriggerMap::addPoint(ofVec2f position, float radius, float threshold)
{
	int colorIndex = _points.size() % _colorPallete.size();
	int size = addPoint(position, radius, threshold, _colorPallete[colorIndex]);
	return size;
}

void TriggerMap::setRadius(int index, float radius)
{
	_points[index].setRadius(radius);
}

void TriggerMap::setThreshold(int index, float threshold)
{
	_points[index].setThreshold(threshold);
}

void TriggerMap::setSwitch(int index, bool isSwitch)
{
	_points[index].setSwitch(isSwitch);
}

void TriggerMap::setCursors(vector<ofVec2f> cursors)
{
	_cursors = cursors;
}

void TriggerMap::setCursor(ofVec2f cursor, int index)
{
	if (index >= 0 && index < _cursors.size()) _cursors[index] = cursor;
}

vector<ofVec2f> TriggerMap::getCursors()
{
	return _cursors;
}

vector<int> TriggerMap::getTriggered()
{
	return _triggered;
}

vector<pair<string, float>> TriggerMap::getOutput()
{
	return _output;
}

void TriggerMap::updateFbo()
{
	ofPushStyle();
	_fbo.begin();
	ofClear(80);
	ofSetColor(80);
	ofSetCircleResolution(100);
	ofDrawRectangle(0, 0, _width, _height);
	for (auto point : _points)
	{
		ofColor inColor, outColor;
		ofVec2f position = point.getPosition() * ofVec2f(_width, _height);
		float radius = point.getRadius() * _width;
		float threshold = point.getThreshold();

		if (point.getState()) inColor.set(255, 50, 50, 0);
		else
		{
			inColor = point.getColor();
			inColor.r = inColor.r * 0.4 + 150;
			inColor.g = inColor.g * 0.4 + 150;
			inColor.b = inColor.b * 0.4 + 150;
		}
		
		outColor = ofColor(200);

		ofSetColor(0);
		ofDrawCircle(position, radius + 2);
		ofSetColor(outColor);
		ofDrawCircle(position, radius);
		ofSetColor(0);
		ofDrawCircle(position, radius * threshold + 2);
		ofSetColor(inColor);
		ofDrawCircle(position, radius * threshold);
	}

	for (auto cursor : _cursors)
	{
		cursor *= ofVec2f(_width, _height);
		ofSetRectMode(ofRectMode::OF_RECTMODE_CENTER);
		ofSetColor(80, 50);
		ofDrawRectangle(cursor.x, cursor.y, 20, _height * 2);
		ofDrawRectangle(cursor.x, cursor.y, _width * 2, 20);
		/*
		//ofSetLineWidth doesnt seem to work, so we draw rectangles...
		const int thickness = 4;
		ofDrawRectangle(0, cursor.y, _width, thickness);
		ofDrawRectangle(cursor.x, 0, thickness, _height);
		*/
	}
	_fbo.end();
	ofPopStyle();
}

void TriggerMap::updateTriggers()
{
	_triggered.clear();
	_triggered.assign(_points.size(), 0);
	for (int i = 0; i < _points.size(); i++)
	{
		bool curState = _points[i].getState();
		_points[i].setState(_cursors);
		if (curState != _points[i].getState())
		{
			if(curState) _triggered[i] = -1;
			else _triggered[i] = 1;
		}
	}

	_output.clear();
	for (int i = 0; i < _triggered.size(); i++)
	{
		pair<string, float> curOutput;
		if (_triggered[i] != 0)
		{
			if (_triggered[i] == 1)
			{
				for (auto value : _points[i].getValues())
				{
					curOutput.first = value.first;
					curOutput.second = value.second;
				}
			}
			if (_triggered[i] == -1 && !_points[i].getSwitch())
			{
				for (auto value : _points[i].getValues())
				{
					curOutput.first = value.first;
					curOutput.second = 0;
				}
			}
		}
	}
}
