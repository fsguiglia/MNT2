#pragma once
#include "ofMain.h"

float normalize(float value, float min, float max);

ofVec2f normalize(ofVec2f pos, ofRectangle rect);

vector<float> normalize(vector<float> values);

float standardize(float value, float mean, float sd);

vector<float> standardize(vector<float> values, float mean, float sd);

vector<float> standardize(vector<float> values);



