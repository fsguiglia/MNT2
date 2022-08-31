#include "CBCS.h"

CBCS::CBCS() :
	_hash(_positions)
{
	_selectedFeatures.first = "";
	_selectedFeatures.second = "";
	_maxN = 1;
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
	
	_analysisFbo.allocate(_width, _height);
	_analysisFbo.begin();
	ofPushStyle();
	ofClear(255);
	ofSetColor(255);
	ofDrawRectangle(0, 0, _width, _height);
	ofPopStyle();
	_analysisFbo.end();

	_drawFbo.allocate(_width, _height);
	_drawFbo.begin();
	ofClear(255);
	_drawFbo.end();
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
				_output.clear();
				for (int i = 0; i < _selection.size(); i++)
				{
					pair<string, float> multipleFiles, singleFile;
					multipleFiles.first = ofToString(i) + ";" + _points[_selection[i]].getName();
					multipleFiles.second = _points[_selection[i]].getValue("position");
					singleFile.first = "single-file-position";
					singleFile.second = _points[_selection[i]].getValue("single-file-position");
					_output.push_back(multipleFiles);
					_output.push_back(singleFile);
				}
			}
		}
		_prevCursor = _cursor;
	}
	updateDrawFbo();
}

void CBCS::draw(int x, int y, int w, int h, ofTrueTypeFont& font)
{
	ofPushStyle();
	ofSetColor(255);
	_drawFbo.draw(x, y, w, h);
	ofSetColor(50);
	if (_selection.size() > 0) font.drawString(_points[_selection[0]].getName(), x + 10, y + w - 10);
	ofPopStyle();
}

void CBCS::updateAnalysisFbo()
{
	_analysisFbo.begin();
	ofPushStyle();
	ofSetCircleResolution(100);
	ofSetColor(255);
	ofDrawRectangle(0, 0, _analysisFbo.getWidth(), _analysisFbo.getHeight());
	
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
	_analysisFbo.end();
}

void CBCS::updateDrawFbo()
{
	ofPushStyle();
	_drawFbo.begin();
	ofSetColor(255);
	ofDrawRectangle(0, 0, _width, _height);
	_analysisFbo.draw(0, 0, _width, _height);
	ofSetColor(150, 150, 255, 100);
	ofSetCircleResolution(100);
	ofVec2f scaledCursor = _cursor * ofVec2f(_width, _height);
	//scaledCursor.x += x;
	//scaledCursor.y += y;
	if(_active) ofDrawEllipse(scaledCursor, _radius * _width * 2, _radius * _height * 2);

	for (int i = 0; i < _selection.size(); i++)
	{
		ofSetColor(150, 150, 255);
		ofVec2f curPos = _points[_selection[i]].getPosition() * ofVec2f(_width, _height);
		//curPos.x += x;
		//curPos.y += y;
		ofDrawEllipse(curPos, 10, 10);
	}
	_drawFbo.end();
	ofPopStyle();
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
	_positions.clear();
	for (auto& point : _points)
	{
		ofVec2f curPos;
		curPos.x = point.getValue(xFeature);
		curPos.y = point.getValue(yFeature);
		_positions.push_back(curPos);
		point.setPosition(curPos);
	}
	build();
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
	_positions.clear();
	for (auto& point : _points) _positions.push_back(point.getPosition());
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
	updateAnalysisFbo();
	_hash.buildIndex();
}

vector<int> CBCS::getKnn(ofVec2f position, int max_n)
{
	vector<int> selection;
	int n = max_n;
	_searchResults.clear();
	_searchResults.resize(max_n);
	_hash.findPointsWithinRadius(position, _radius, _searchResults);
	if (_searchResults.size() < n) n = _searchResults.size();
	for (int i = 0; i < n; i++)	selection.push_back(_searchResults[i].first);
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

vector<pair<string, float>> CBCS::getOutput()
{
	return _output;
}