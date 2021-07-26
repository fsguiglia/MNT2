#pragma once
#include "ofMain.h"


class DimensionalityReduction : public ofThread
{
public:
	DimensionalityReduction();
	void setup(int perplexity, int learningRate, int iterations);
	void start(ofJson data);
	void check();
	bool getRunning();
	bool getCompleted();
	ofJson getData();

private:
	void end();
	int _perplexity, _learningRate, _iterations;
	bool _running, _completed;
	string _path, _tsnePath;
};