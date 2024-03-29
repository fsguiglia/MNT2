#include "rgbPoint.h"

RGBPoint::RGBPoint()
{
	setPosition(0, 0);
	_isTrigger = false;
	_state = false;
}

RGBPoint::RGBPoint(int width, int height, int maxPixels)
{
	setPosition(0,0);
	_width = width;
	_height = height;
	_isTrigger = false;
	_state = false;
	_maxPixels = maxPixels;
}

void RGBPoint::setImage(ofImage img)
{
	_img = img;
	_img.setImageType(ofImageType::OF_IMAGE_GRAYSCALE);
	_width = _img.getWidth();
	_height = _img.getHeight();
	_fbo.allocate(_width, _height);
	_fbo.begin();
	ofSetColor(255);
	ofDrawRectangle(0, 0, _width, _height);
	_img.draw(0, 0);
	_fbo.end();
	_pixels = _img.getPixels();
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
	if (radius > _maxPixels) step = ceil(radius / _maxPixels);
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
					average += (float)_pixels.getColor(x, y).r;
					//float a = (float)_pixels.getColor(x, y).a / 255.;
					//average += r * a + 255. * (1 - a);
					n++;
				}
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
	_width = w;
	_height = h;
	_pixels.resize(_width, _height);
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
		_fbo.draw(x, y, w, h);
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
