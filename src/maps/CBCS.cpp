#include "CBCS.h"

CBCS::CBCS() :
	_hash(_positions)
{
	_selectedFeatures.first = "";
	_selectedFeatures.second = "";
	_maxN = 10;
}

void CBCS::setup(int width, int height, vector<string> features)
{
	setup(width, height);
	setFeatures(features);
}

void CBCS::setup(int width, int height)
{
	_width = width;
	_height = height;
	_fbo.allocate(_width, _height);
	_fbo.begin();
	ofPushStyle();
	ofClear(255);
	ofSetColor(255);
	ofDrawRectangle(0, 0, _width, _height);
	ofPopStyle();
	_fbo.end();
}

void CBCS::update()
{
	if (_active)
	{
		if (_cursor != _prevCursor)
		{
			if (_cursor.x > 0 && _cursor.x <= 1 && _cursor.y >= 0 && _cursor.y <= 1)
			{
				_selection = getKnn(_cursor, _maxN);
				map<string, float> curOutput;
				cout << _points.size() << endl;
				for (int i = 0; i < _selection.size(); i++)
				{
					string curValue = ofToString(i) + ";" + _points[_selection[i]].getName();
					curOutput[curValue] = _points[_selection[i]].getValue("position");
				}
				_output = curOutput;
			}
		}
		_prevCursor = _cursor;
	}
}

void CBCS::draw(int x, int y, int w, int h, ofTrueTypeFont& font)
{
	ofPushStyle();
	ofSetColor(255);
	ofDrawRectangle(x, y, w, h);
	_fbo.draw(x, y, w, h);
	ofSetColor(150, 150, 255, 100);
	ofSetCircleResolution(100);
	ofVec2f scaledCursor = _cursor * ofVec2f(w, h);
	scaledCursor.x += x;
	scaledCursor.y += y;
	ofDrawEllipse(scaledCursor, _radius * _width, _radius * _height);

	for (int i = 0; i < _selection.size(); i++)
	{
		ofSetColor(150,150,255);
		ofVec2f curPos = _points[_selection[i]].getPosition() * ofVec2f(w, h);
		curPos.x += x;
		curPos.y += y;
		ofDrawEllipse(curPos, 10, 10);
	}
	ofSetColor(50);
	if (_selection.size() > 0) font.drawString(_points[_selection[0]].getName(), x + 10, y + w - 10);
	ofPopStyle();
}

void CBCS::updateFbo()
{
	_fbo.begin();
	ofPushStyle();
	ofSetCircleResolution(100);
	ofSetColor(255);
	ofDrawRectangle(0, 0, _fbo.getWidth(), _fbo.getHeight());
	
	ofSetColor(175);
	for (auto point : _points)
	{
		ofVec2f curPos = point.getPosition() * ofVec2f(_width, _height);
		_mesh.addVertex(ofVec3f(curPos.x, curPos.y));
	}
	_mesh.drawWireframe();
	
	for (auto point : _points)
	{
		ofVec2f curPos = point.getPosition() * ofVec2f(_width, _height);
		ofSetColor(255);
		ofDrawEllipse(curPos, 12, 12);
		ofSetColor(0);
		ofDrawEllipse(curPos, 11, 11);
		ofSetColor(50, 50, 100);
		ofDrawEllipse(curPos, 10, 10);
	}
	ofPopStyle();
	_fbo.end();
}

void CBCS::addPoint(Point point)
{
	_positions.push_back(point.getPosition());
	BaseMap::addPoint(point);
}

void CBCS::clearPoints()
{
	_positions.clear();
	BaseMap::clearPoints();
}

void CBCS::selectFeatures(string xFeature, string yFeature)
{
	_selectedFeatures.first = xFeature;
	_selectedFeatures.second = yFeature;
}

void CBCS::setFeatures(vector<string> features)
{
	_features = features;
}

vector<string> CBCS::getFeatures()
{
	return _features;
}

void CBCS::setCursor(ofVec2f cursor)
{
	_cursor = cursor;
}

void CBCS::setCursor(ofVec2f cursor, int index)
{
	if (index == 0) setCursor(cursor);
}

ofVec2f CBCS::getCursor()
{
	return _cursor;
}

vector<ofVec2f> CBCS::getCursors()
{
	vector<ofVec2f> cursors;
	cursors.push_back(_cursor);
	return cursors;
}

void CBCS::setMaxSamples(int max)
{
	_maxN = max;
}

int CBCS::getMaxSamples()
{
	return _maxN;
}

void CBCS::setRadius(float radius)
{
	_radius = radius;
}

float CBCS::getRadius()
{
	return _radius;
}

void CBCS::normalize()
{
	float minX = _positions[0].x;
	float maxX = _positions[0].x;
	float minY = _positions[0].y;
	float maxY = _positions[0].y;

	for (auto position : _positions)
	{
		if (position.x < minX) minX = position.x;
		if (position.x > maxX) maxX = position.x;
		if (position.y < minY) minY = position.y;
		if (position.y > maxY) maxY = position.y;
	}

	for (int i = 0; i < _positions.size(); i++)
	{
		if (minX != maxX) _positions[i].x = (_positions[i].x - minX) / (maxX - minX);
		if (minY != maxY) _positions[i].y = (_positions[i].y - minY) / (maxY - minY);
		_points[i].setPosition(_positions[i]);
	}
	build();
}

void CBCS::build()
{
	_mesh.clear();
	updateFbo();
	_hash.buildIndex();
}

vector<int> CBCS::getKnn(ofVec2f position, int max_n)
{
	vector<int> selection;
	_searchResults.clear();
	_searchResults.resize(max_n);
	_hash.findPointsWithinRadius(position, _radius, _searchResults);
	for (auto result : _searchResults) selection.push_back(result.first);
	return selection;
}

void CBCS::removeSelection()
{
	if (_selection.size() > 0)
	{
		vector<int> sorted = _selection;
		sort(sorted.begin(), sorted.end());
		for (int i = sorted.size() - 1; i >= 0; i--)
		{
			_positions.erase(_positions.begin() + sorted[i]);
			_points.erase(_points.begin() + sorted[i]);
		}
		_selection.clear();
		_searchResults.clear();
		build();
	}
}

map<string, float> CBCS::getOutput()
{
	return _output;
}