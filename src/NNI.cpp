#include "NNI.h"

NNI::NNI()
{

}

void NNI::setup(int width, int height)
{
	_width = width;
	_height = height;
	_updateIdMap = false;

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
		_mesh.getColors()[i] = ofColor(colorIndex, 50, 50);
	}

	_mesh.getVbo().setAttributeDivisor(ofShader::COLOR_ATTRIBUTE, 1);
}

void NNI::add(ofVec2f position)
{
	if (_sites.size() < maxSize)
	{
		Map::addElement(_sites, _parameters, position);
		update(_colorFbo, 1, -1, _sites);
		_updateIdMap = true;
	}
}

void NNI::addParameter(string parameter, float value)
{
	Map::addParameter(_sites, _parameters, parameter, value);
}

void NNI::setParameter(string parameter, float value)
{
	Map::setParameter(_parameters, parameter, value);
}

void NNI::setParameter(int site, string parameter, float value)
{
	Map::setParameter(_sites, site, parameter, value);
}

void NNI::move(int index, ofVec2f pos)
{
	if (index < _sites.size())
	{
		_sites[index].setPosition(pos);
		update(_colorFbo, 1, -1, _sites);
		_updateIdMap = true;
	}
}

void NNI::randomize(float speed)
{
	for (int i = 0; i < _sites.size(); i++)
	{
		ofVec2f curPosition = _sites[i].getPosition();
		curPosition.x += .01 * (0.5 - ofNoise(ofGetElapsedTimef() * speed, (i * 2) * 1000));
		curPosition.y += .01 * (0.5 - ofNoise(ofGetElapsedTimef() * speed, (i * 2 + 1) * 1000));
		if(curPosition.x > 1) curPosition.x = 1;
		if (curPosition.x < 0) curPosition.x = 0;
		if (curPosition.y > 1) curPosition.y = 1;
		if (curPosition.y < 0) curPosition.y = 0;
		_sites[i].setPosition(curPosition);
	}
	update(_colorFbo, 1, -1, _sites);
	_updateIdMap = true;
}

void NNI::remove(int index)
{
	_sites.erase(_sites.begin() + index);
	update(_colorFbo, 1, -1, _sites);
	_updateIdMap = true;
}

void NNI::remove(ofVec2f pos)
{
	_sites.erase(_sites.begin() + (int)getClosest(pos)[0]);
	update(_colorFbo, 1, -1, _sites);
	_updateIdMap = true;
}

array<float, 2> NNI::getClosest(ofVec2f pos)
{
	int closest = -1;
	float minDist = -1;
	for (int i = 0; i < _sites.size(); i++)
	{
		float curDist = pos.distance(_sites[i].getPosition());
		if (minDist == -1 || curDist < minDist)
		{
			closest = i;
			minDist = curDist;
		}
	}
	return array<float, 2>({ (float)closest, minDist });
}

vector<Point> NNI::getSites()
{
	return _sites;
}

int NNI::getWidth()
{
	return _width;
}

int NNI::getHeight()
{
	return _height;
}

map<string, float> NNI::getParameters()
{
	return _parameters;
}

void NNI::draw(int x, int y)
{
	draw(x, y, _width, _height);
}

void NNI::draw(int x, int y, int w, int h)
{
	ofPushStyle();
	ofSetColor(255);
	_colorFbo.draw(x, y, w, h);
	ofSetColor(255);


	for (int i = 0; i < _sites.size(); i++)
	{
		ofVec2f pos = _sites[i].getPosition() * ofVec2f(w, h);
		ofDrawBitmapString(ofToString(i), pos + ofVec2f(x, y));
	}
	ofPopStyle();
}

void NNI::drawId(int x, int y)
{
	updateIdMap();
	ofPushStyle();
	ofSetColor(255);
	_idFbo.draw(x, y);
	ofPopStyle();
}

void NNI::drawInterpolation(int x, int y)
{
	ofPushStyle();
	ofSetColor(255);
	_interpolateFbo.draw(x, y);
	ofPopStyle();
	ofSetColor(255);
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
	
	fbo.begin();
	ofClear(255);
	if (_sites.size() > 0)
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
}

void NNI::updateIdMap()
{
	if (_updateIdMap)
	{
		update(_idFbo, 0, -1, _sites);
		_updateIdMap = false;
		_idFbo.readToPixels(_curState);
	}
}

map<string, float> NNI::interpolate(ofVec2f pos, bool renderNewZone)
{
	updateIdMap();

	vector<Point> curSites = _sites;
	curSites.push_back(Point(pos));
	vector<float> weights;
	weights.assign(_sites.size(), 0);

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
		for (int i = 0; i < _sites.size(); i++)
		{
			if(_sites[i].hasValue(parameter.first)) value += _sites[i].getValue(parameter.first) * weights[i];
		}
		values[parameter.first] = value;
	}
	return values;
}
