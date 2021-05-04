#include "trigger.h"

Trigger::Trigger()
{
	setPosition(-1, -1);
	setRadius(0);
	setThreshold(0);
	setState(false);
}

Trigger::Trigger(ofVec2f position, float radius, float threshold)
{
	setPosition(position);
	setRadius(radius);
	setThreshold(threshold);
	setState(false);
}

void Trigger::setRadius(float radius)
{
	_radius = radius;
}

void Trigger::setThreshold(float threshold)
{
	_threshold = threshold;
}

void Trigger::setState(bool state)
{
	_state = state;
}

void Trigger::setColor(ofColor color)
{
	_color = color;
}

void Trigger::setOutValue(string key, float value)
{
	_outParameters[key] = value;
}

void Trigger::setOutValues(map<string, float> parameters)
{
	_outParameters = parameters;
}

void Trigger::update(ofVec2f position)
{
	vector<ofVec2f> positions = { position };
	update(positions);
}

void Trigger::update(vector<ofVec2f>& positions)
{
	if (_state)
	{
		bool inside = false;
		for (auto position : positions)
		{
			if (position.distance(getPosition()) < _radius)
			{
				inside = true;
				break;
			}
		}
		if (!inside) _state = false;
	}
	else
	{
		bool inside = false;
		for (auto position : positions)
		{
			if (position.distance(getPosition()) < _radius * _threshold)
			{
				inside = true;
				break;
			}
		}
		if (inside) _state = false;
	}
}

bool Trigger::getState()
{
	return _state;
}

void Trigger::draw()
{
	ofPushStyle();
	ofSetColor(_color);
	ofDrawCircle(getPosition(), _radius);
	ofPopStyle();
}
