#include "RGBMap.h"

RGBMap::RGBMap()
{
}

void RGBMap::setup(int width, int height)
{
	_width = width;
	_height = height;
	_radius = 0;
	_fbo.allocate(_width, _height);
	ofPushStyle();
	_fbo.begin();
	ofClear(255);
	ofSetColor(255);
	ofDrawRectangle(0, 0, _width, _height);
	_fbo.end();
	ofPopStyle();
}

void RGBMap::update()
{
	if(_active) updateOutput();
	updateFbo();
}

void RGBMap::draw(int x, int y, ofTrueTypeFont & font)
{
	draw(x, y, _width, _height, font);
}

void RGBMap::draw(int x, int y, int w, int h, ofTrueTypeFont & font)
{
	ofPushStyle();
	ofSetColor(255);
	_fbo.draw(x, y, w, h);
	if (_lastSelected >= 0 && _lastSelected < _points.size() && _drawSelected)
	{
		if (ofGetElapsedTimeMillis() - _lastSelectedMs < 1000)
		{
			drawSelected(x, y, w, h, font, 200);
		}
	}
	ofPopStyle();
}

void RGBMap::addPoint(ofVec2f position, ofImage img)
{
	RGBPoint point(img.getWidth(), img.getHeight());
	point.setPosition(position.x, position.y);
	point.setTrigger(false);
	point.setImage(img);
	Map::addPoint(point);
}

void RGBMap::resizePoint(int index, int w, int h)
{
	_points[index].setSize(w, h);
}

void RGBMap::setTrigger(int index, bool isTrigger)
{
	_points[index].setTrigger(isTrigger);
}

void RGBMap::setRadius(float radius)
{
	_radius = radius;
}

void RGBMap::setCursors(vector<ofVec2f> cursors)
{
	_cursors = cursors;
}

void RGBMap::setCursor(ofVec2f cursor, int index)
{
	if (index >= 0 && index < _cursors.size()) _cursors[index] = cursor;
}

vector<ofVec2f> RGBMap::getCursors()
{
	return _cursors;
}

map<string, float> RGBMap::getOutput()
{
	return _output;
}

void RGBMap::updateFbo()
{
	ofPushStyle();
	_fbo.begin();
	ofClear(255);
	ofSetColor(255);
	ofDrawRectangle(0, 0, _width, _height);
	for (auto point : _points) {
		ofVec2f pos = point.getPosition();
		pos.x *= _width;
		pos.y *= _height;
		point.draw(pos.x, pos.y);
	}
	if (_active)
	{
		ofSetCircleResolution(100);
		for (auto cursor : _cursors)
		{
			int radius = _radius * _width;
			cursor *= ofVec2f(_width, _height);
			ofSetColor(0);
			ofDrawLine(0, cursor.y, cursor.x - radius, cursor.y);
			ofDrawLine(cursor.x + radius, cursor.y, _width, cursor.y);
			ofDrawLine(cursor.x, 0, cursor.x, cursor.y - radius);
			ofDrawLine(cursor.x, cursor.y + radius, cursor.x, _height);
			ofNoFill();
			ofDrawRectangle(cursor.x - radius, cursor.y - radius, radius * 2, radius * 2);
		}
	}
	_fbo.end();
	ofPopStyle();
}

void RGBMap::updateOutput()
{
	_output.clear();
	for (auto point : _points)
	{
		bool prevState = point.getState();
		int radius = _radius * _width;
		float maxWeight = NULL;
		for (auto cursor : _cursors)
		{
			cursor -= point.getPosition();
			cursor *= ofVec2f(_width, _height);
			float curWeight = point.getAverageColor(cursor, radius);
			if (maxWeight == NULL || curWeight > maxWeight) maxWeight = curWeight;
		}
		if (point.getTrigger())
		{
			bool curState = point.getState();
			if (!prevState && curState)
			{
				for (auto value : point.getValues()) _output[value.first] = value.second;
			}
			else if (prevState && !curState)
			{
				for (auto value : point.getValues()) _output[value.first] = 0;
			}
		}
		else
		{
			for (auto value : point.getValues()) _output[value.first] = value.second * maxWeight;
		}
	}
}
