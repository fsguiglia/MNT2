#ifndef _MAP
#define _MAP
#include "ofMain.h"
#include "point.h"

/*
acá la lista de puntos,
la lista de parametros, las funciones para devolver parametros y una funcion abstracta update?
*/

class Map {
public:
	Map();

	template<typename T>
	void addElement(vector<T>& points, set<string> parameters, map<string, float> values, ofVec2f position)
	{
		T point;
		map<string, float> curParameters;
		for (auto value : values)
		{
			if (parameters.find(value.first) != parameters.end()) curParameters.insert(value);
		}
		point.setValues(curParameters);
		point.setPosition(position);
		points.push_back(point);
	}

	template<typename T>
	void remove(vector<T>& points, int index)
	{
		if(index < points.size()) points.erase(points.begin() + index);
	}

	template<typename T>
	void addParameter(vector<T>& points, set<string>& parameters, string parameter, float value)
	{
		if (parameters.find(parameter) == parameters.end())
		{
			parameters.insert(parameter);
			for (Point& point : points)
			{
				if (!point.hasValue(parameter)) point.setValue(parameter, value);
			}
		}
	}

	template<typename T>
	void removeParameter(vector<T>& points, set<string>& parameters, string parameter)
	{
		if (parameters.find(parameter) != parameters.end()) parameters.erase(parameter);
		for (Point& point : points) point.deleteValue(parameter);

	}

	template<typename T>
	void updateParameters(T& point, map<string, float> values)
	{
		for (auto& value : values)
		{
			if (point.hasValue(value.first)) point.setValue(value.first, value.second);
		}
	}
};
#endif
