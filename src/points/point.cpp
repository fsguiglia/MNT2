#include "Point.h"

Point::Point()
{
	setPosition(-1, -1);
	_name = "";
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

void Point::setParameter(string key, float value)
{
	_parameters[key] = value;
}

void Point::setFeature(string key, float value)
{
	_features[key] = value;
}

void Point::deleteParameter(string key)
{
	if(_parameters.find(key) != _parameters.end()) _parameters.erase(key);
}

void Point::deleteFeature(string key)
{
	if (_features.find(key) != _features.end()) _features.erase(key);
}

void Point::setParameters(map<string, float> parameters)
{
	_parameters = parameters;
}

void Point::setFeatures(map<string, float> features)
{
	_features = features;
}

void Point::setName(string name)
{
	_name = name;
}

string Point::getName()
{
	return _name;
}

ofVec2f Point::getPosition()
{
	return _position;
}

bool Point::hasFeature(string key)
{
	bool value = false;
	if (_features.find(key) != _features.end()) value = true;
	return value;
}

bool Point::hasParameter(string key)
{
	bool value = false;
	if(_parameters.find(key) != _parameters.end()) value = true;
	return value;
}

float Point::getParameter(string key)
{
	float value = -1.0;
	if (_parameters.find(key) != _parameters.end()) value = _parameters[key];
	return value;
}

float Point::getFeature(string key)
{
	float value = -1.0;
	if (_features.find(key) != _features.end()) value = _features[key];
	return value;
}

map<string, float> Point::getParameters()
{
	return _parameters;
}

map<string, float> Point::getFeatures()
{
	return _features;
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
