#include "node.h"

Node::Node()
{
	_color = ofColor(80, 80, 80);
	_isInput = false;
	_isOutput = false;
	_selectedIn = -1;
	_selectedOut = -1;
	_id = 0;
}

void Node::setup(int x, int y, int w, int h, int inputs, int outputs, string name, ofColor color)
{
	setup(x, y, w, h);
	setInputs(inputs);
	setOutputs(outputs);
	setName(name, true);
	setColor(color);
}

void Node::setup(int x, int y, int w, int h)
{
	_rect.setPosition(x, y);
	_rect.setWidth(w);
	_rect.setHeight(h);
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

void Node::draw(ofTrueTypeFont & font)
{
	ofSetColor(_color);
	ofDrawRectangle(_rect);
	ofRectangle bb = font.getStringBoundingBox(_name, 0, 0);
	if (bb.getWidth() > _rect.getWidth()) _rect.setWidth(bb.getWidth() * 1.2);
	ofSetColor(255);
	font.drawString(_name, _rect.x + (_rect.width - bb.width) * 0.5, _rect.y + bb.height + (_rect.height - bb.height) * 0.5);
	for (int i = 0; i < _inputs; i++)
	{
		if (i == _selectedIn) ofSetColor(150, 0, 0);
		else ofSetColor(150);
		ofDrawCircle(_rect.x, _rect.y + _inPositions[i].y * _rect.height, _rect.height * _inPositions[i].width);
	}
	for (int i = 0; i < _outputs; i++)
	{
		if (i == _selectedOut) ofSetColor(150, 0, 0);
		else ofSetColor(150);
		ofDrawCircle(_rect.x + _rect.getWidth(), _rect.y + _outPositions[i].y * _rect.height, _rect.height * _outPositions[i].width);
	}
}

void Node::setSize(int w, int h)
{
	_rect.setWidth(w);
	_rect.setHeight(h);
}

void Node::setPosition(int x, int y)
{
	_rect.setPosition(x, y);
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
	connector.x += _rect.x;
	connector.y *= _rect.height;
	connector.y += _rect.y;
	connector.width *= _rect.width;
	connector.height *= _rect.height;
	return connector;
}

ofRectangle Node::getOutputConnector(int index)
{
	ofRectangle connector = _outPositions[index];
	connector.x *= _rect.width;
	connector.x += _rect.x;
	connector.y *= _rect.height;
	connector.y += _rect.y;
	connector.width *= _rect.width;
	connector.height *= _rect.height;
	return connector;
}

void Node::setName(string name, bool addId)
{
	_name = name;
	if (addId) _name += "(" + ofToString(_id) + ")";
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
	bool in = _rect.inside(x, y);
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
