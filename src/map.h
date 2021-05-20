#ifndef _MAP
#define _MAP
#include "ofMain.h"

template<typename T> class Map {
public:
	Map();

	virtual void setup(int width, int height) = 0;
	virtual void update() = 0;
	virtual void draw(int x, int y, int w, int h, ofTrueTypeFont& font) = 0;

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

protected:
	int addPoint(T point);
	void drawSelected(int x, int y, int w, int h, ofTrueTypeFont& font, int opacity=100);

	typename vector<T> _points;
	map<string, float> _parameters, _output;
	bool _positionChanged, _drawSelected, _active;
	float _randomSpeed;
	int _width, _height, _lastSelectedMs , _lastSelected;
};

template<typename T>
Map<T>::Map()
{
	_positionChanged = false;
	_lastSelected = -1;
	_width = 100;
	_height = 100;
}

template<typename T>
int Map<T>::addPoint(T point)
{
	_points.push_back(point);
	_positionChanged = true;
	int index = _points.size() - 1;
	return index;
}

template<typename T>
void Map<T>::removePoint(int index)
{
	if (index < _points.size())
	{
		_points.erase(_points.begin() + index);
		_positionChanged = true;
	}
}

template<typename T>
void Map<T>::removePoint(ofVec2f pos)
{
	removePoint((int)getClosest(pos)[0]);
}

template<typename T>
void Map<T>::clearPoints()
{
	_points.clear();
	_positionChanged = true;
}

template<typename T>
void Map<T>::movePoint(int index, ofVec2f pos)
{
	_points[index].setPosition(pos);
	_positionChanged = true;
	_lastSelected = index;
	_lastSelectedMs = ofGetElapsedTimeMillis();
}

template<typename T>
T Map<T>::getPoint(int index)
{
	return _points[index];
}

template<typename T>
vector<T> Map<T>::getPoints()
{
	return _points;
}

template<typename T>
void Map<T>::setRandomize(float randomSpeed)
{
	_randomSpeed = randomSpeed;
}

template<typename T>
void Map<T>::randomize()
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
array<float, 2> Map<T>::getClosest(ofVec2f pos, bool select)
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
	if (select)
	{
		_lastSelected = closest;
		_lastSelectedMs = ofGetElapsedTimeMillis();
	}
	return array<float, 2>({ (float)closest, minDist });
}

template<typename T>
void Map<T>::addGlobalParameter(string parameter, float value)
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
void Map<T>::setGlobalParameter(string parameter, float value)
{
	if (_parameters.find(parameter) != _parameters.end()) _parameters[parameter] = value;
}

template<typename T>
void Map<T>::removeGlobalParameter(string parameter)
{
	if (_parameters.find(parameter) != _parameters.end()) _parameters.erase(parameter);
	for (auto& point : _points) point.deleteValue(parameter);
}

template<typename T>
void Map<T>::addPointParameter(int index, string parameter, float value)
{
	if (!_points[index].hasValue(parameter)) _points[index].setValue(parameter, index);
}


template<typename T>
void Map<T>::setPointParameter(int index, string parameter, float value)
{
	if (_points[index].hasValue(parameter)) _points[index].setValue(parameter, value);
}

template<typename T>
void Map<T>::removePointParameter(int index, string parameter)
{
	_points[index].deleteValue(parameter);
}

template<typename T>
map<string, float> Map<T>::getParameters()
{
	return _parameters;
}

template<typename T>
void Map<T>::setActive(bool active)
{
	_active = active;
}

template<typename T>
bool Map<T>::getActive()
{
	return _active;
}

template<typename T>
int Map<T>::getWidth()
{
	return _width;
}

template<typename T>
int Map<T>::getHeight()
{
	return _height;
}

template<typename T>
inline void Map<T>::setLastSelected(int index, int time)
{
	_lastSelected = index;
	_lastSelectedMs = time;
}

template<typename T>
int Map<T>::getLastSelected()
{
	return _lastSelected;
}

template<typename T>
void Map<T>::setDrawSelected(bool drawSelected)
{
	_drawSelected = drawSelected;
}

template<typename T>
void Map<T>::drawSelected(int x, int y, int w, int h, ofTrueTypeFont& font, int opacity)
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

#endif
