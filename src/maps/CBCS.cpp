#include "CBCS.h"

CBCS::CBCS() :
	_hash(_positions)
{
	_selectedFeatures.first = "";
	_selectedFeatures.second = "";
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
			_selection = getKnn(_cursor, _maxN);
			for (auto index : _selection)
			{
				_output[_points[index].getName()] = _points[index].getValue("position");
			}
		}
		_prevCursor = _cursor;
	}
}

void CBCS::draw(int x, int y, ofTrueTypeFont & font)
{
	_fbo.draw(0, 0);
	for (int i = 0; i < _selection.size(); i++)
	{
		ofSetColor(_colorPallete[i % _colorPallete.size()]);
		ofDrawEllipse(_points[_selection[i]].getPosition(), 10, 10);
	}
}

void CBCS::addPoint(Point point)
{
	BaseMap::addPoint(point);
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
	_radius;
}

void CBCS::build()
{
	_hash.buildIndex();
	_mesh.clear();
	for (auto point : _points)
	{
		ofVec3f meshPoint = point.getPosition();
		meshPoint.x *= _width;
		meshPoint.y *= _height;
		_mesh.addVertex(meshPoint);
	}
	_fbo.begin();
	ofPushStyle();
	ofSetColor(0);
	ofPopStyle();
	_mesh.draw();
	_fbo.end();
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
		for (int i = sorted.size() - 1; i >= 0; i--) _points.erase(_points.begin() + i);
		_selection.clear();
		build();
	}
}
