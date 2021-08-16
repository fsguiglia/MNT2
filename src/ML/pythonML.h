#pragma once
#include "ofMain.h"


class PythonML : public ofThread
{
public:
	PythonML ();
	void setup(string scriptPath, string name);
	void setParameters(map<string, float> parameters);
	map<string, float> getParameters();
	void setParameter(string parameter, float value);
	float getParameter(string parameter);
	void start(ofJson data);
	void start();
	void check();
	bool getRunning();
	bool getCompleted();
	ofJson getData();
	void end();

private:
	map<string, float> _parameters;
	bool _running, _completed;
	string _name, _scriptPath, _inputFilePath, _outputFilePath;
};