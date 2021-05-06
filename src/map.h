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
	void addElement(vector<T>& points, map<string, float> values, ofVec2f position)
	{
		T point;
		point.setValues(values);
		point.setPosition(position);
		points.push_back(point);
	}

	template<typename T>
	void remove(vector<T>& points, int index)
	{
		if(index < points.size()) points.erase(points.begin() + index);
	}

	template<typename T>
	void addParameter(vector<T>& points, map<string, float>& parameters, string parameter, float value)
	{
		if (parameters.find(parameter) == parameters.end())
		{
			parameters[parameter] = value;
			for (auto& point : points)
			{
				if (!point.hasValue(parameter)) point.setValue(parameter, value);
			}
		}
	}

	void setParameter(map<string, float>& parameters, string parameter, float value)
	{
		if (parameters.find(parameter) != parameters.end()) parameters[parameter] = value;
	}

	template<typename T>
	void setParameter(vector<T>& points, int index, string parameter, float value)
	{
		if (points[index].hasValue(parameter)) points[index].setValue(parameter, value);
	}

	template<typename T>
	void removeParameter(vector<T>& points, map<string, float>& parameters, string parameter)
	{
		if (parameters.find(parameter) != parameters.end()) parameters.erase(parameter);
		for (auto& point : points) point.deleteValue(parameter);

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
