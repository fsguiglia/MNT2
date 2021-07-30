#include "dimensionalityReduction.h"

DimensionalityReduction::DimensionalityReduction()
{
	_running = false;
	_completed = false;
}

void DimensionalityReduction::setup(string scriptPath, string name)
{
	_scriptPath = ofFilePath::getAbsolutePath(scriptPath);
	string folder = _scriptPath.substr(0, _scriptPath.find_last_of("\\/")) + "/tmp/";
	_inputFilePath = folder + name + ".tmp";
	_outputFilePath = folder + name + "_o.tmp";

}

void DimensionalityReduction::setParameters(map<string, float> parameters)
{
	_parameters = parameters;
}

map<string, float> DimensionalityReduction::getParameters()
{
	return _parameters;
}

void DimensionalityReduction::setParameter(string parameter, float value)
{
	_parameters[parameter] = value;
}

float DimensionalityReduction::getParameter(string parameter)
{
	return _parameters[parameter];
}

void DimensionalityReduction::start(ofJson data)
{
	ofSavePrettyJson(_inputFilePath, data);
	string command = "python " + _scriptPath;
	command += " -f " + _inputFilePath;
	for (auto parameter : _parameters) command = command + " " + parameter.first + " " + ofToString(parameter.second);
	
	system(command.c_str());
	_running = true;
}

void DimensionalityReduction::check()
{
	if (ofFile::doesFileExist(_outputFilePath)) _completed = true;
}

bool DimensionalityReduction::getRunning()
{
	return _running;
}

bool DimensionalityReduction::getCompleted()
{
	return _completed;
}

ofJson DimensionalityReduction::getData()
{
	ofJson data = ofLoadJson(_outputFilePath);
	end();
	return data;
}

void DimensionalityReduction::end()
{
	ofFile::removeFile(_inputFilePath);
	ofFile::removeFile(_outputFilePath);
	_running = false;
	_completed = false;
}
