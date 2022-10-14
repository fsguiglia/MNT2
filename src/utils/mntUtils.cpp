#include "mntUtils.h"

float normalize(float value, float min, float max)
{
	if(max != min) value = (value - min) / (max - min);
	return value;
}

ofVec2f normalize(ofVec2f pos, ofRectangle rect)
{
	pos.x = normalize(pos.x, rect.x, rect.x + rect.getWidth());
	pos.y = normalize(pos.y, rect.y, rect.y + rect.getHeight());
	return pos;
}

vector<float> normalize(vector<float> values)
{
	if (values.size() > 0)
	{
		float min = values[0];
		float max = values[0];

		for (float value : values)
		{
			if (value < min) min = value;
			if (value > max) max = value;
		}
		for (float& value : values) value = normalize(value, min, max);
	}
	return values;
}

vector<ofVec2f> normalize(vector<ofVec2f> values)
{
	if (values.size() > 0)
	{
		float minX = values[0].x;
		float maxX = values[0].x;
		float minY = values[0].y;
		float maxY = values[0].y;
		for (auto& value : values)
		{
			if (value.x < minX) minX = value.x;
			if (value.x > maxX) maxX = value.x;
			if (value.y < minY) minY = value.y;
			if (value.y > maxY) maxY = value.y;
		}
		for (auto& value : values)
		{
			if (value.x < minX) minX = value.x;
			if (value.x > maxX) maxX = value.x;
			if (value.y < minY) minY = value.y;
			if (value.y > maxY) maxY = value.y;
		}
		for (auto& value : values)
		{
			value.x = normalize(value.x, minX, maxX);
			value.y = normalize(value.y, minY, maxY);
		}
	}
	return values;
}

float standardize(float value, float mean, float sd)
{
	if(sd != 0) value = (value - mean) / sd;
	return value;
}

vector<float> standardize(vector<float> values, float mean, float sd)
{
	for (float& value : values) value = standardize(value, mean, sd);
	return values;
}

vector<float> standardize(vector<float> values)
{
	float sum = 0.0, mean = 0.0, sd = 0.0;
	for (float value : values) sum += value;
	mean = sum / values.size();
	for (float value : values) sd += pow(value - mean, 2);
	sd = sqrt(sd / values.size());
	for (float& value : values) value = standardize(value, mean, sd);
	return values;
}


