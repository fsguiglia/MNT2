#include "NNI.h"

NNI::NNI()
{
}

void NNI::setup(int width, int height)
{
	_width = width;
	_height = height;

	_colorFbo.allocate(_width, _height);
	_colorFbo.begin();
	ofPushStyle();
	ofClear(255);
	ofSetColor(255);
	ofDrawRectangle(0, 0, _width, _height);
	ofPopStyle();
	_colorFbo.end();
	_idFbo.allocate(idFboResolution, idFboResolution);
	_interpolateFbo.allocate(idFboResolution, idFboResolution);
	_curState.allocate(idFboResolution, idFboResolution, ofImageType::OF_IMAGE_COLOR);
	_nniPixels.allocate(idFboResolution, idFboResolution, ofImageType::OF_IMAGE_COLOR);

	_matrices.resize(maxSize);

	_buffer.allocate();
	_buffer.bind(GL_TEXTURE_BUFFER);
	_buffer.setData(_matrices, GL_STREAM_DRAW);

	_tex.allocateAsBufferTexture(_buffer, GL_RGBA32F);

	_voronoi.load("shaders/voronoi/vert.glsl", "shaders/voronoi/frag.glsl");
	_voronoi.begin();
	_voronoi.setUniformTexture("tex", _tex, 0);
	_voronoi.end();

	_mesh = ofMesh::cone(_width * 1.5, 25, 64, 6);
	_mesh.setUsage(GL_STATIC_DRAW);
	_mesh.getColors().resize(_matrices.size());
	for (size_t i = 0; i < _mesh.getColors().size(); i++) {
		float colorIndex = i * 255 / 16;
		_mesh.getColors()[i] = _colorPallete[i % _colorPallete.size()];
	}

	_mesh.getVbo().setAttributeDivisor(ofShader::COLOR_ATTRIBUTE, 1);
}

void NNI::setColorPallete(vector<ofColor> colorPallete)
{
	_mesh.getColors().resize(_matrices.size());
	for (size_t i = 0; i < _mesh.getColors().size(); i++) {
		float colorIndex = i * 255 / 16;
		_mesh.getColors()[i] = colorPallete[i % colorPallete.size()];
	}
	_mesh.getVbo().setAttributeDivisor(ofShader::COLOR_ATTRIBUTE, 1);
	BaseMap::setColorPallete(colorPallete);
}

void NNI::update()
{
	_voronoi.begin();
	_voronoi.setUniformTexture("tex", _tex, 0);
	_voronoi.end();
	if (_randomSpeed != 0) randomize();
	if (_positionChanged)
	{
		_updateIdFbo = true;
		update(_colorFbo, 1, -1, _points);
		_positionChanged = false;
	}
	if (_active)
	{
		_output = interpolate(_cursor, _drawInterpolation);
	}
}

void NNI::draw(int x, int y, ofTrueTypeFont& font)
{
	draw(x, y, _width, _height, font);
}

void NNI::draw(int x, int y, int w, int h, ofTrueTypeFont& font)
{
	ofPushStyle();
	ofSetColor(255);
	_colorFbo.draw(x, y, w, h);

	for (int i = 0; i < _points.size(); i++)
	{
		ofVec2f pos = _points[i].getPosition() * ofVec2f(w, h);
		ofDrawBitmapString(ofToString(i), pos + ofVec2f(x, y));
	}
	if (_lastSelected >= 0 && _lastSelected < _points.size() && _drawSelected)
	{
		if (ofGetElapsedTimeMillis() - _lastSelectedMs < 1000)
		{
			drawSelected(x, y, w, h, font);
		}
	}
	ofPopStyle();
}

void NNI::drawIdFbo(int x, int y)
{
	updateIdMap();
	ofPushStyle();
	ofSetColor(255);
	_idFbo.draw(x, y);
	ofPopStyle();
}

void NNI::drawInterpolationFbo(int x, int y)
{
	ofPushStyle();
	ofSetColor(255);
	_interpolateFbo.draw(x, y);
	ofPopStyle();
	ofSetColor(255);
}

int NNI::addPoint(ofVec2f pos)
{
	Point point;
	point.setPosition(pos);
	point.setValues(_parameters);
	BaseMap::addPoint(point);
	return _points.size();
}

void NNI::setCursor(ofVec2f cursor)
{
	_cursor = cursor;
}

ofVec2f NNI::getCursor()
{
	return _cursor;
}

void NNI::setDrawInterpolation(bool drawInterpolation)
{
	_drawInterpolation = drawInterpolation;
}

map<string, float> NNI::getOutput()
{
	return _output;
}

void NNI::update(ofFbo& fbo, int mode, int interpolate, vector<Point>& sites)
{
	ofVec2f resolution(fbo.getWidth(), fbo.getHeight());
	for (int i = 0; i < sites.size(); i++)
	{
		ofNode node;
		node.setPosition(ofVec3f(sites[i].getPosition() * resolution));
		node.setOrientation(ofVec3f(270, 0, 0));
		_matrices[i] = node.getLocalTransformMatrix();
	}

	_buffer.updateData(0, _matrices);
	
	ofPushStyle();
	fbo.begin();
	ofClear(255);
	ofSetColor(255);
	ofDrawRectangle(0, 0, resolution.x, resolution.y);
	if (_points.size() > 0)
	{
		ofEnableDepthTest();
		ofSetColor(0);	
		_voronoi.begin();
		_voronoi.setUniform2f("u_resolution", resolution);
		_voronoi.setUniform1i("mode", mode);
		_voronoi.setUniform1i("interpolate", interpolate);
		_mesh.drawInstanced(OF_MESH_FILL, sites.size());
		_voronoi.end();
		ofDisableDepthTest();
	}
	fbo.end();
	ofPopStyle();
}

void NNI::updateIdMap()
{
	update(_idFbo, 0, -1, _points);
	_positionChanged = false;
	_idFbo.readToPixels(_curState);
}

map<string, float> NNI::interpolate(ofVec2f pos, bool renderNewZone)
{
	if(_updateIdFbo) updateIdMap();

	vector<Point> curSites = _points;
	curSites.push_back(Point(pos));
	vector<float> weights;
	weights.assign(_points.size(), 0);

	update(_interpolateFbo, 0, curSites.size() - 1, curSites);
	if(renderNewZone) update(_colorFbo, 1, curSites.size() - 1, curSites);
	_interpolateFbo.readToPixels(_nniPixels);
	
	int sum = 0;
	for (int i = 0; i < _nniPixels.size(); i+=4) {
		//si es rojo
		if (_nniPixels[i] > 200 & _nniPixels[i + 1] < 10) {
			if ((int)_curState[i] < weights.size()) weights[(int)_curState[i]]++;
			sum++;
		}
	}
	for (float& weight : weights) weight = weight / sum;

	map<string, float> values;
	for (auto parameter : _parameters)
	{
		float value = 0;
		for (int i = 0; i < _points.size(); i++)
		{
			if(_points[i].hasValue(parameter.first)) value += _points[i].getValue(parameter.first) * weights[i];
		}
		values[parameter.first] = value;
	}
	return values;
}
