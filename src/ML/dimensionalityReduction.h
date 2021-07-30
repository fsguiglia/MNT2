#pragma once
#include "ofMain.h"


class DimensionalityReduction : public ofThread
{
public:
	DimensionalityReduction();
	void setup(int perplexity, int learningRate, int iterations);
	void setPerplexity(int perplexity);
	int getPerplexity();
	void setLearningRate(int learningRate);
	int getLearningRate();
	void setIterations(int iterations);
	int getIterations();
	void start(ofJson data, string name);
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