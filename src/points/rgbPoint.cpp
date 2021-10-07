#include "rgbPoint.h"

RGBPoint::RGBPoint()
{
	setPosition(0, 0);
	_isTrigger = false;
	_state = false;
}

RGBPoint::RGBPoint(int width, int height, int maxAnalysisPixels)
{
	setPosition(0,0);
	_width = width;
	_height = height;
	_img.allocate(_width, _height, ofImageType::OF_IMAGE_COLOR_ALPHA);
	_img.setColor(200);
	_isTrigger = false;
	_state = false;
	_maxSamples = maxAnalysisPixels;
}

void RGBPoint::setImage(ofImage img)
{
	_img = img;
	_width = img.getWidth();
	_height = img.getHeight();
}

void RGBPoint::setImage(ofImage img, string path)
{
	_imgPath = path;
	setImage(img);
}

ofImage RGBPoint::getImage()
{
	return _img;
}

string RGBPoint::getImagePath()
{
	return _imgPath;
}

float RGBPoint::getAverageColor(ofVec2f position, float radius)
{
	float average = 0;
	int step = 1;
	if (radius > _maxSamples) step = int(radius / _maxSamples);
	if (_img.isAllocated())
	{
		int n = 0;
		int x0 = position.x - radius;
		int x1 = position.x + radius;
		int y0 = position.y - radius;
		int y1 = position.y + radius;
		for (int y = y0; y < y1; y+= step)
		{
			for (int x = x0; x < x1; x+= step)
			{
				bool inX = x >= 0 && x < _width;
				bool inY = y >= 0 && y < _height;
				if (inX && inY) {
					float r = (float)_img.getColor(x, y).r;
					float a = (float)_img.getColor(x, y).a / 255.;
					average += r * a + 255. * (1 - a);
				}
				else average += 255.;
				n++;
			}
		}
		average /= (float)n;
		average /= 255.;
		average = 1. - average;
		
		if (_isTrigger)
		{
			if (!_state && average > 0.75) _state = true;
			if (_state && average < 0.25) _state = false;
		}
	}
	
	return average;
}

int RGBPoint::getHeight()
{
	return _height;
}

int RGBPoint::getWidth()
{
	return _width;
}

void RGBPoint::setSize(int w, int h)
{
	if (_img.isAllocated())
	{
		_img.resize(w, h);
		_width = w;
		_height = h;
	}
}

void RGBPoint::setTrigger(bool isTrigger)
{
	_isTrigger = isTrigger;
}

bool RGBPoint::getTrigger()
{
	return _isTrigger;
}

bool RGBPoint::getState()
{
	return _state;
}

void RGBPoint::draw(int x, int y, int w, int h)
{
	if (_img.isAllocated())
	{
		ofPushStyle();
		ofSetColor(255);
		ofDrawRectangle(x, y, w, h);
		_img.getTexture().draw(x, y, w, h);
		ofSetColor(255, 0, 0);
		ofNoFill();
		ofDrawRectangle(x, y, w, h);
		ofFill();
		ofDrawRectangle(x, y, 10, 10);
		ofPopStyle();
	}
}

void RGBPoint::draw(int x, int y)
{
	draw(x, y, _width, _height);
}
