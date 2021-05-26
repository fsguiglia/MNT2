#include "Point.h"

Point::Point()
{
	setPosition(-1, -1);
}

Point::Point(ofVec2f position)
{
	setPosition(position);
}

void Point::setPosition(ofVec2f position)
{
	_position = position;
}

void Point::setPosition(float x, float y)
{
	setPosition(ofVec2f(x, y));
}

void Point::setValue(string key, float value)
{
	_parameters[key] = value;
}

void Point::deleteValue(string key)
{
	if(_parameters.find(key) != _parameters.end()) _parameters.erase(key);
}

void Point::setValues(map<string, float> parameters)
{
	_parameters = parameters;
}

void Point::setClass(int class_)
{
	_class = class_;
}

ofVec2f Point::getPosition()
{
	return _position;
}

bool Point::hasValue(string key)
{
	bool value = false;
	if(_parameters.find(key) != _parameters.end()) value = true;
	return value;
}

float Point::getValue(string key)
{
	float value = -1.0;
	if (_parameters.find(key) != _parameters.end()) value = _parameters[key];
	return value;
}

map<string, float> Point::getValues()
{
	return _parameters;
}

int Point::getClass()
{
	return _class;
}

vector<pair<float, int>> Point::getClosest(vector<ofVec2f>& positions, int n)
{
	vector<pair<float, int>> distances;

	for (int i = 0; i < positions.size(); i++)
	{
		float curDistance = _position.distance(positions[i]);
		if (distances.size() < n)
		{
			distances.push_back({ curDistance, i });
			sort(distances.rbegin(), distances.rend());
		}
		else
		{
			for (auto& distance : distances)
			{
				if (curDistance < distance.first)
				{
					distance = { curDistance, i };
					sort(distances.rbegin(), distances.rend());
					break;
				}
			}
		}
	}
	return distances;
}

pair<float, int> Point::getClosest(vector<ofVec2f>& positions)
{
	pair<float,int> closest = getClosest(positions, 1)[0];
	return closest;
}
