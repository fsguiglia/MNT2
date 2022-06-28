#include "RGBMap.h"

RGBMap::RGBMap()
{
}

void RGBMap::setup(int width, int height)
{
	_width = width;
	_height = height;
	_sizeText = "(" + ofToString(_width) + "x" + ofToString(_height) + ")";
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
	ofSetColor(150);
	ofDrawBitmapString(_sizeText, x + w - 90, y + h - 10);
	ofPopStyle();
}

int RGBMap::addPoint(ofVec2f position, ofImage img)
{
	RGBPoint point(img.getWidth(), img.getHeight());
	point.setPosition(position.x, position.y);
	point.setTrigger(false);
	point.setImage(img);
	BaseMap::addPoint(point);
	return _points.size() - 1;
}

int RGBMap::addPoint(ofVec2f position, ofImage img, string path)
{
	RGBPoint point(img.getWidth(), img.getHeight());
	point.setPosition(position.x, position.y);
	point.setTrigger(false);
	point.setImage(img, path);
	BaseMap::addPoint(point);
	return _points.size() - 1;
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

vector<pair<string, float>> RGBMap::getOutput()
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
	for (int i = 0; i < _points.size(); i++) {
		//ofSetColor(_colorPallete[i % _colorPallete.size()]);
		ofVec2f pos = _points[i].getPosition();
		pos.x *= _width;
		pos.y *= _height;
		_points[i].draw(pos.x, pos.y);
	}
	if (_active)
	{
		for (auto cursor : _cursors)
		{
			int radius = _radius * _width;
			cursor *= ofVec2f(_width, _height);
			cursor.x = int(cursor.x);
			cursor.y = int(cursor.y);
			ofSetColor(80, 50);
			/*
			ofSetRectMode(ofRectMode::OF_RECTMODE_CENTER);
			ofDrawRectangle(cursor.x, cursor.y, 20, _height * 2);
			ofDrawRectangle(cursor.x, cursor.y, _width * 2, 20);
			*/
			ofSetRectMode(ofRectMode::OF_RECTMODE_CORNER);
			ofDrawRectangle(cursor.x - 10, 0, 20, cursor.y - radius);
			ofDrawRectangle(cursor.x - 10, cursor.y + radius, 20, _height - radius);
			ofDrawRectangle(0, cursor.y - 10, cursor.x - radius, 20);
			ofDrawRectangle(cursor.x + radius, cursor.y - 10,  _width - radius, 20);
			ofSetColor(40);
			ofSetRectMode(ofRectMode::OF_RECTMODE_CORNER);
			ofDrawRectangle(cursor.x - radius, cursor.y - radius, 5, radius * 2);
			ofDrawRectangle(cursor.x - radius, cursor.y - radius, radius * 2, 5);
			ofDrawRectangle(cursor.x + radius, cursor.y - radius, 5, radius * 2);
			ofDrawRectangle(cursor.x - radius, cursor.y + radius, radius * 2 + 5, 5);
			/*
			//ofSetLineWidth doesnt seem to work, so we draw rectangles...
			ofSetColor(80);
			const int thickness = 4;
			ofDrawRectangle(0, cursor.y, cursor.x - radius, thickness);
			ofDrawRectangle(cursor.x + radius, cursor.y, _width, thickness);
			ofDrawRectangle(cursor.x, 0, thickness, cursor.y - radius);
			ofDrawRectangle(cursor.x, cursor.y + radius, thickness, _height);
			ofDrawRectangle(cursor.x - radius, cursor.y - radius, thickness, radius * 2);
			ofDrawRectangle(cursor.x - radius, cursor.y - radius, radius * 2, thickness);
			ofDrawRectangle(cursor.x + radius, cursor.y - radius, thickness, radius * 2);
			ofDrawRectangle(cursor.x - radius, cursor.y + radius, radius * 2 + thickness, thickness);
			*/
		}
	}
	_fbo.end();
	ofPopStyle();
}

void RGBMap::updateOutput()
{
	_output.clear();

	for (auto& point : _points)
	{
		pair<string, float> curOutput;
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
				for (auto value : point.getValues())
				{
					curOutput.first = value.first; 
					curOutput.second = value.second;
				}
			}
			else if (prevState && !curState)
			{
				for (auto value : point.getValues())
				{
					curOutput.first = value.first;
					curOutput.second = 0;
				}
			}
		}
		else
		{
			for (auto value : point.getValues())
			{
				curOutput.first = value.first;
				curOutput.second = value.second;
			}
		}
		_output.push_back(curOutput);
	}
}
