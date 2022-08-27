#include "pythonML.h"

PythonML::PythonML()
{
	_running = false;
	_completed = false;
	_prefix = "";
}

void PythonML::setup(string scriptPath, string name, string prefix)
{
	_scriptPath = ofFilePath::getAbsolutePath(scriptPath);
	string folder = _scriptPath.substr(0, _scriptPath.find_last_of("\\/")) + "/tmp/";
	_inputFilePath = folder + name + ".tmp";
	_outputFilePath = folder + name + "_o.tmp";
	_prefix = prefix;

}

void PythonML::setPrefix(string prefix)
{
	_prefix = prefix;
}

void PythonML::setParameters(map<string, float> parameters)
{
	_parameters = parameters;
}

map<string, float> PythonML::getParameters()
{
	return _parameters;
}

void PythonML::setParameter(string parameter, float value)
{
	_parameters[parameter] = value;
}

float PythonML::getParameter(string parameter)
{
	return _parameters[parameter];
}

void PythonML::start(ofJson data)
{
	ofSavePrettyJson(_inputFilePath, data);
	start();
}

void PythonML::start()
{
	string command = _prefix + " " + _scriptPath;
	command += " -f " + _inputFilePath;
	for (auto parameter : _parameters) command = command + " " + parameter.first + " " + ofToString(parameter.second);
	system(command.c_str());
	_running = true;
}

void PythonML::check()
{
	if (ofFile::doesFileExist(_outputFilePath)) _completed = true;
}

bool PythonML::getRunning()
{
	return _running;
}

bool PythonML::getCompleted()
{
	return _completed;
}

ofJson PythonML::getData()
{
	ofJson data = ofLoadJson(_outputFilePath);
	end();
	return data;
}

void PythonML::end()
{
	//ofFile::removeFile(_inputFilePath);
	//ofFile::removeFile(_outputFilePath);
	_running = false;
	_completed = false;
}
