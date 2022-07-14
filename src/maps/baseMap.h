#pragma once
#include "ofMain.h"

template<typename T> class BaseMap {
public:
	BaseMap();

	virtual void setup(int width, int height) = 0;
	virtual void update() = 0;
	virtual void draw(int x, int y, int w, int h, ofTrueTypeFont& font) = 0;
	virtual vector<pair<string, float>> getOutput() = 0;

	void setColorPallete(vector<ofColor> colorPallete);
	void removePoint(int index);
	void removePoint(ofVec2f pos);
	void clearPoints();
	void movePoint(int index, ofVec2f pos);
	T getPoint(int index);
	vector<T> getPoints();
	void setRandomize(float randomize);
	void randomize();
	array<float, 2> getClosest(ofVec2f pos, bool select=false);
	void addGlobalParameter(string parameter, float value);
	void setGlobalParameter(string parameter, float value);
	void removeGlobalParameter(string parameter);
	void clearGlobalParameters();
	void addPointParameter(int index, string parameter, float value);
	void setPointParameter(int index, string parameter, float value);
	void removePointParameter(int index, string parameter);
	map<string, float> getParameters();
	void setActive(bool active);
	bool getActive();
	int getWidth();
	int getHeight();
	void setLastSelected(int index, int time=0);
	int getLastSelected();
	void setDrawSelected(bool drawSelected);
	int size();

protected:
	int addPoint(T point);
	void drawSelected(int x, int y, int w, int h, ofTrueTypeFont& font, int opacity=100);

	typename vector<T> _points;
	map<string, float> _parameters;
	vector<pair<string, float>> _output;
	bool _positionChanged, _drawSelected, _active;
	float _randomSpeed;
	int _width, _height, _lastSelectedMs , _lastSelected;
	vector<ofColor> _colorPallete;
};

template<typename T>
BaseMap<T>::BaseMap()
{
	_positionChanged = false;
	_lastSelected = -1;
	_width = 100;
	_height = 100;

	//default colors
	_colorPallete = {
		ofColor(207,42,42),
		ofColor(255,77,77),
		ofColor(231,61,61),
		ofColor(164,35,35),
		ofColor(116,18,18),
		ofColor(207,117,42),
		ofColor(255,158,77),
		ofColor(231,138,61),
		ofColor(164,93,35),
		ofColor(116,63,18),
		ofColor(25,124,124),
		ofColor(48,157,157),
		ofColor(37,139,139),
		ofColor(21,98,98),
		ofColor(11,70,70),
		ofColor(34,165,34),
		ofColor(62,206,62),
		ofColor(49,185,49),
		ofColor(28,131,28),
		ofColor(14,93,14)
	};
}

template<typename T>
int BaseMap<T>::addPoint(T point)
{
	_points.push_back(point);
	_positionChanged = true;
	int index = _points.size() - 1;
	return index;
}

template<typename T>
inline void BaseMap<T>::setColorPallete(vector<ofColor> colorPallete)
{
	_colorPallete = colorPallete;
}

template<typename T>
void BaseMap<T>::removePoint(int index)
{
	if (index < _points.size())
	{
		_points.erase(_points.begin() + index);
		_positionChanged = true;
	}
}

template<typename T>
void BaseMap<T>::removePoint(ofVec2f pos)
{
	removePoint((int)getClosest(pos)[0]);
}

template<typename T>
void BaseMap<T>::clearPoints()
{
	_points.clear();
	_positionChanged = true;
}

template<typename T>
void BaseMap<T>::movePoint(int index, ofVec2f pos)
{
	if (index < _points.size())
	{
		_points[index].setPosition(pos);
		_positionChanged = true;
		_lastSelected = index;
		_lastSelectedMs = ofGetElapsedTimeMillis();
	}
}

template<typename T>
T BaseMap<T>::getPoint(int index)
{
	return _points[index];
}

template<typename T>
vector<T> BaseMap<T>::getPoints()
{
	return _points;
}

template<typename T>
void BaseMap<T>::setRandomize(float randomSpeed)
{
	_randomSpeed = randomSpeed;
}

template<typename T>
void BaseMap<T>::randomize()
{
	for (int i = 0; i < _points.size(); i++)
	{
		ofVec2f curPosition = _points[i].getPosition();
		curPosition.x += .01 * (0.5 - ofNoise(ofGetElapsedTimef() * _randomSpeed, (i * 2) * 1000));
		curPosition.y += .01 * (0.5 - ofNoise(ofGetElapsedTimef() * _randomSpeed, (i * 2 + 1) * 1000));
		if (curPosition.x > 1) curPosition.x = 1;
		if (curPosition.x < 0) curPosition.x = 0;
		if (curPosition.y > 1) curPosition.y = 1;
		if (curPosition.y < 0) curPosition.y = 0;
		_points[i].setPosition(curPosition);
	}
	_positionChanged = true;
}

template<typename T>
array<float, 2> BaseMap<T>::getClosest(ofVec2f pos, bool select)
{
	int closest = -1;
	float minDist = NULL;
	for (int i = 0; i < _points.size(); i++)
	{
		float curDist = pos.distance(_points[i].getPosition());
		if (minDist == NULL || curDist < minDist)
		{
			closest = i;
			minDist = curDist;
		}
	}
	if (select) setLastSelected(closest, ofGetElapsedTimeMillis());
	return array<float, 2>({ (float)closest, minDist });
}

template<typename T>
void BaseMap<T>::addGlobalParameter(string parameter, float value)
{
	if (_parameters.find(parameter) == _parameters.end())
	{
		_parameters[parameter] = value;
		for (auto& point : _points)
		{
			if (!point.hasValue(parameter)) point.setValue(parameter, value);
		}
	}
}

template <typename T>
void BaseMap<T>::setGlobalParameter(string parameter, float value)
{
	if (_parameters.find(parameter) != _parameters.end()) _parameters[parameter] = value;
}

template<typename T>
void BaseMap<T>::removeGlobalParameter(string parameter)
{
	if (_parameters.find(parameter) != _parameters.end()) _parameters.erase(parameter);
	for (auto& point : _points) point.deleteValue(parameter);
}

template<typename T>
inline void BaseMap<T>::clearGlobalParameters()
{
	_parameters.clear();
}

template<typename T>
void BaseMap<T>::addPointParameter(int index, string parameter, float value)
{
	if (!_points[index].hasValue(parameter)) _points[index].setValue(parameter, value);
}


template<typename T>
void BaseMap<T>::setPointParameter(int index, string parameter, float value)
{
	if (_points[index].hasValue(parameter)) _points[index].setValue(parameter, value);
}

template<typename T>
void BaseMap<T>::removePointParameter(int index, string parameter)
{
	_points[index].deleteValue(parameter);
}

template<typename T>
map<string, float> BaseMap<T>::getParameters()
{
	return _parameters;
}

template<typename T>
void BaseMap<T>::setActive(bool active)
{
	_active = active;
}

template<typename T>
bool BaseMap<T>::getActive()
{
	return _active;
}

template<typename T>
int BaseMap<T>::getWidth()
{
	return _width;
}

template<typename T>
int BaseMap<T>::getHeight()
{
	return _height;
}

template<typename T>
inline void BaseMap<T>::setLastSelected(int index, int time)
{
	_lastSelected = index;
	_lastSelectedMs = time;
}

template<typename T>
int BaseMap<T>::getLastSelected()
{
	return _lastSelected;
}

template<typename T>
void BaseMap<T>::setDrawSelected(bool drawSelected)
{
	_drawSelected = drawSelected;
}

template<typename T>
int BaseMap<T>::size()
{
	 return _points.size();
}

template<typename T>
void BaseMap<T>::drawSelected(int x, int y, int w, int h, ofTrueTypeFont& font, int opacity)
{
	ofPushStyle();
	ofVec2f position = _points[_lastSelected].getPosition();
	ofVec2f drawPos = position * ofVec2f(w, h);
	drawPos += ofVec2f(x, y);
	position *= 100.;
	position.x = int(position.x);
	position.y = int(position.y);
	position /= 100.;
	drawPos.x += 10;
	drawPos.y -= 10;
	string sPosition = ofToString(position.x) + ", " + ofToString(position.y);
	ofSetColor(0, opacity);
	ofRectangle bounding = font.getStringBoundingBox(sPosition, drawPos.x, drawPos.y);
	bounding.x -= 1;
	bounding.width += 2;
	bounding.y -= 1;
	bounding.height += 2;
	ofDrawRectangle(bounding);
	ofSetColor(180);
	font.drawString(sPosition, drawPos.x, drawPos.y);
	ofPopStyle();
}