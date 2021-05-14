#include "mntUtils.h"

float normalize(float value, float min, float max)
{
	value = (value - min) / (max - min);
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
	float min = NULL;
	float max = NULL;

	for (float value : values)
	{
		if (value < min || min == NULL) min = value;
		if (value > max || max == NULL) max = value;
	}
	for (float& value : values) value = normalize(value, min, max);

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
	float sum = 0.0, mean, sd = 0.0;
	for (float value : values) sum += value;
	for (float value : values) sd += pow(value - mean, 2);
	sd = sqrt(sd / values.size());
	for (float& value : values) value = standardize(value, mean, sd);
	return values;
}


