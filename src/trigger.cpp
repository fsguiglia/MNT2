#include "trigger.h"

Trigger::Trigger()
{
	setPosition(ofVec2f(-1,-1));
	setRadius(0);
	setThreshold(0);
	setState(false);
	setSwitch(false);
}

Trigger::Trigger(ofVec2f position, float radius, float threshold, bool isSwitch)
{
	setPosition(position);
	setRadius(radius);
	setThreshold(threshold);
	setState(false);
	setSwitch(isSwitch);
}

void Trigger::setRadius(float radius)
{
	_radius = radius;
}

float Trigger::getRadius()
{
	return _radius;
}

void Trigger::setThreshold(float threshold)
{
	_threshold = threshold;
}

float Trigger::getThreshold()
{
	return _threshold;
}

void Trigger::setSwitch(bool isSwitch)
{
	_switch = isSwitch;
}

bool Trigger::getSwitch()
{
	return _switch;
}

void Trigger::setState(bool state)
{
	_state = state;
}

bool Trigger::setState(vector<ofVec2f> cursors)
{
	bool curState = false;
	float threshold = _radius;
	if (!_state) threshold = _radius * _threshold;
	for (auto cursor : cursors)
	{
		if (cursor.distance(_position) < threshold)
		{
			curState = true;
			break;
		}
	}
	_state = curState;
	return _state;
}

void Trigger::setColor(ofColor color)
{
	_color = color;
}

ofColor Trigger::getColor()
{
	return _color;
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
