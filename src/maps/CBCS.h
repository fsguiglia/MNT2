#pragma once
#include "ofMain.h"
#include "BaseMap.h"
#include "../points/point.h"
#include "ofxSpatialHash.h"

class CBCS : public BaseMap<Point>
{
public:
	CBCS();
	void setup(int width, int height, vector<string> features);
	void setup(int width, int height);
	void update();
	void draw(int x, int y, int w, int h, ofTrueTypeFont& font);

	void addPoint(Point point);
	void selectFeatures(string xFeature, string yFeature);
	void setFeatures(vector<string> features);
	vector<string> getFeatures();

	void setCursor(ofVec2f cursor);
	void setCursor(ofVec2f cursor, int index);
	ofVec2f getCursor();
	vector<ofVec2f> getCursors();

	void setMaxSamples(int max);
	int getMaxSamples();
	void setRadius(float radius);
	float getRadius();

	void build();
	vector<int> getKnn(ofVec2f position, int max_n);
	void removeSelection();

	map<string, float> getOutput();

private:	
	vector<string> _features;
	pair<string, string> _selectedFeatures;
	
	vector<ofVec2f> _positions;
	vector<int> _selection;
	ofx::KDTree<ofVec2f> _hash;
	ofx::KDTree<ofVec2f>::SearchResults _searchResults;

	ofFbo _fbo;
	ofVec2f _cursor, _prevCursor;
	float _radius;
	int _maxN;
};
