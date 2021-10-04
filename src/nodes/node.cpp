#include "node.h"

Node::Node()
{
	_name = "node";
	_rect.width = 50;
	_rect.height = 30;
	_color = ofColor(80, 80, 80);
	_isInput = false;
	_isOutput = false;
	_selectedIn = -1;
	_selectedOut = -1;
	_id = 0;
	setName(_name, false);
}

void Node::setup(float x, float y, float h, int inputs, int outputs, ofTrueTypeFont font, ofColor color)
{
	_rect.height = h;
	setPosition(x,y);
	setInputs(inputs);
	setOutputs(outputs);
	setColor(color);
	_font = font;
}

void Node::setInputs(int inputs)
{
	_inputs = inputs;
	_inPositions.clear();
	for (int i = 0; i < inputs; i++)
	{
		float x = 0;
		float y = float(i + 1) / (inputs + 1);
		float r = 0.1;
		_inPositions.push_back(ofRectangle(x, y, r, r));
	}
}

void Node::setOutputs(int outputs)
{
	_outputs = outputs;
	_outPositions.clear();
	for (int i = 0; i < outputs; i++)
	{
		float x = 0.95;
		float y = float(i + 1) / (outputs + 1);
		float r = 0.1;
		_outPositions.push_back(ofRectangle(x, y, r, r));
	}
}

void Node::setColor(ofColor color)
{
	_color = color;
}

void Node::draw()
{
	ofPushStyle();
	ofSetColor(_color);
	ofRectangle r = _rect;
	r.x *= ofGetWidth();
	r.y *= ofGetHeight();
	ofDrawRectangle(r);
	ofSetColor(255);
	// a ojo...
	_font.drawString(_name, r.x + r.width * 0.08, r.y + (r.height + _font.getLineHeight() * 0.75) * 0.5);
	for (int i = 0; i < _inputs; i++)
	{
		if (i == _selectedIn) ofSetColor(150, 0, 0);
		else ofSetColor(150);
		ofDrawCircle(r.x, r.y + _inPositions[i].y * r.height, r.height * _inPositions[i].width);
	}
	for (int i = 0; i < _outputs; i++)
	{
		if (i == _selectedOut) ofSetColor(150, 0, 0);
		else ofSetColor(150);
		ofDrawCircle(r.x + r.getWidth(), r.y + _outPositions[i].y * r.height, r.height * _outPositions[i].width);
	}
	ofPopStyle();
}

void Node::setSize(int w, int h)
{
	_rect.setWidth(w);
	_rect.setHeight(h);
}

void Node::setPosition(float x, float y)
{
	_rect.setPosition(x, y);
}

ofVec2f Node::getPosition()
{
	return _rect.getPosition();
}

int Node::getWidth()
{
	return _rect.width;
}

int Node::getHeight()
{
	return _rect.height;
}

int Node::getInputs()
{
	return _inputs;
}

int Node::getOutputs()
{
	return _outputs;
}

ofRectangle Node::getInputConnector(int index)
{
	ofRectangle connector = _inPositions[index];
	connector.x *= _rect.width;
	connector.x += _rect.x * ofGetWidth();
	connector.y *= _rect.height;
	connector.y += _rect.y * ofGetHeight();
	connector.width *= _rect.width;
	connector.height *= _rect.height;
	return connector;
}

ofRectangle Node::getOutputConnector(int index)
{
	ofRectangle connector = _outPositions[index];
	connector.x *= _rect.width;
	connector.x += _rect.x * ofGetWidth();
	connector.y *= _rect.height;
	connector.y += _rect.y * ofGetHeight();
	connector.width *= _rect.width;
	connector.height *= _rect.height;
	return connector;
}

void Node::setName(string name, bool addId)
{
	_name = name;
	if (addId) _name += "/" + ofToString(_id);
	ofRectangle bb = _font.getStringBoundingBox(_name, 0, 0);
	_rect.setWidth(bb.width * 1.2);
}

string Node::getName()
{
	return _name;
}

void Node::setId(int id)
{
	_id = id;
}

int Node::getId()
{
	return _id;
}

bool Node::inside(int x, int y)
{
	ofRectangle translated = _rect;
	translated.x *= ofGetWidth();
	translated.y *= ofGetHeight();
	bool in = translated.inside(x, y);
	return in;
}

void Node::setAsInput(bool isInput)
{
	_isInput = isInput;
	setInputs(0);
	setOutputs(1);
}

void Node::setAsOutput(bool isOutput)
{
	_isOutput = isOutput;
	setInputs(1);
	setOutputs(0);
}

bool Node::isInput()
{
	return _isInput;
}

bool Node::isOutput()
{
	return _isOutput;
}

ofRectangle Node::getBox()
{
	return _rect;
}
