#include "mntNode.h"

MNTNode::MNTNode()
{
	_name = "MNTNode";
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

void MNTNode::setup(float x, float y, float h, int inputs, int outputs, ofTrueTypeFont font, ofColor color)
{
	setName("MNTNode", false);
	_rect.height = h;
	_rect.width = 50;
	_connectorWidth = 10;
	_color = ofColor(80, 80, 80);
	
	_isInput = false;
	_isOutput = false;
	_selectedOut = -1;

	setPosition(x,y);
	setInputs(inputs);
	setOutputs(outputs);
	setColor(color);
	_font = font;
}

void MNTNode::setInputs(int inputs)
{
	_inputs = inputs;
	_inPositions.clear();
	for (int i = 0; i < _inputs; i++)
	{
		ofRectangle rect;
		rect.x = _rect.x * ofGetWidth() + _rect.width;
		rect.y = _rect.y * ofGetHeight();
		rect.height = _rect.height;
		rect.width = _connectorWidth;
		_inPositions.push_back(rect);
	}
}

void MNTNode::setOutputs(int outputs)
{
	_outputs = outputs;
	_outPositions.clear();
	for (int i = 0; i < _outputs; i++)
	{
		ofRectangle rect;
		rect.x = _rect.x + _rect.width;
		rect.y = _rect.y;
		rect.height = _rect.height;
		rect.width = _connectorWidth;
		_outPositions.push_back(rect);
	}
}

void MNTNode::setColor(ofColor color)
{
	_color = color;
}

void MNTNode::draw()
{
	ofPushStyle();
	ofSetColor(_color);
	ofRectangle r = _rect;
	r.x *= ofGetWidth();
	r.y *= ofGetHeight();
	ofDrawRectangle(r);
	ofSetColor(255);
	// a ojo...
	string drawName = _name;
	if (!_isInput && !_isOutput) drawName = "/" + drawName;
	_font.drawString(drawName, r.x + r.width * 0.08, r.y + (r.height + _font.getLineHeight() * 0.75) * 0.5);
	for (int i = 0; i < _outputs; i++)
	{
		if (i == _selectedOut) ofSetColor(180);
		else ofSetColor(150);
		_outPositions[i].x = r.x + r.width;
		_outPositions[i].y = r.y;
		ofDrawRectangle(_outPositions[i]);
	}
	if (_inputs > 0)
	{
		ofSetColor(150);
		ofDrawCircle(r.x, r.y + r.height * 0.5, r.height * 0.1);
	}
	ofPopStyle();
}

void MNTNode::setSize(int w, int h)
{
	_rect.setWidth(w);
	_rect.setHeight(h);
}

void MNTNode::setPosition(float x, float y)
{
	_rect.setPosition(x, y);
}

ofVec2f MNTNode::getPosition()
{
	return _rect.getPosition();
}

int MNTNode::getWidth()
{
	return _rect.width;
}

int MNTNode::getHeight()
{
	return _rect.height;
}

int MNTNode::getInputs()
{
	return _inputs;
}

int MNTNode::getOutputs()
{
	return _outputs;
}

ofRectangle MNTNode::getInputConnector(int index)
{
	ofRectangle connector = _rect;
	connector.width = 30;
	connector.x *= ofGetWidth();
	connector.y *= ofGetHeight();
	return connector;
}

ofRectangle MNTNode::getOutputConnector(int index)
{
	ofRectangle connector = _outPositions[index];
	return connector;
}

void MNTNode::setName(string name, bool addId)
{
	_name = name;
	if (addId) _name += "/" + ofToString(_id);
	ofRectangle bb = _font.getStringBoundingBox(_name, 0, 0);
	_rect.setWidth(bb.width * 1.2 + 5);
}

string MNTNode::getName()
{
	return _name;
}

void MNTNode::setId(int id)
{
	_id = id;
}

int MNTNode::getId()
{
	return _id;
}

int MNTNode::inside(int x, int y, bool select)
{
	int in = -1;
	_selectedOut = -1;
	ofRectangle translated = _rect;
	translated.x *= ofGetWidth();
	translated.y *= ofGetHeight();
	ofRectangle input = translated;
	input.x -= input.height * 0.1;
	input.width += input.height * 0.1;
	if (input.inside(x, y)) in = 0;
	else
	{
		for (int i = 0; i < _outPositions.size(); i++)
		{
			if (_outPositions[i].inside(x, y))
			{
				if (select) _selectedOut = i;
				in = 1;
				break;
			}
		}
	}
	return in;
}

void MNTNode::setAsInput(bool isInput)
{
	_isInput = isInput;
	_inputs = 0;
	_outputs = 1;
}

void MNTNode::setAsOutput(bool isOutput)
{
	_isOutput = isOutput;
	_inputs = 1;
	_outputs = 0;
}

bool MNTNode::isInput()
{
	return _isInput;
}

bool MNTNode::isOutput()
{
	return _isOutput;
}

ofRectangle MNTNode::getBox()
{
	return _rect;
}
