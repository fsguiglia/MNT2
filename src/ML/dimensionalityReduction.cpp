#include "dimensionalityReduction.h"

DimensionalityReduction::DimensionalityReduction()
{
	//check default values
	_perplexity = 0;
	_learningRate = 0;
	_iterations = 0;
	_running = false;
	_completed = false;
}

void DimensionalityReduction::setup(int perplexity, int learningRate, int iterations)
{
	_perplexity = perplexity;
	_learningRate = learningRate;
	_iterations = iterations;
}

void DimensionalityReduction::setPerplexity(int perplexity)
{
	_perplexity = perplexity;
}

int DimensionalityReduction::getPerplexity()
{
	return _perplexity;
}

void DimensionalityReduction::setLearningRate(int learningRate)
{
	_learningRate = learningRate;
}

int DimensionalityReduction::getLearningRate()
{
	return _learningRate;
}

void DimensionalityReduction::setIterations(int iterations)
{
	_iterations = iterations;
}

int DimensionalityReduction::getIterations()
{
	return _iterations;
}

void DimensionalityReduction::start(ofJson data, string name)
{
	_path = "../../analysis/tmp/" + name + "_analisis.tmp";
	_tsnePath = "../../analysis/tmp/" + name + "_analisis_tsne.tmp";
	_path = ofFilePath::getAbsolutePath(_path);
	_tsnePath = ofFilePath::getAbsolutePath(_tsnePath);

	ofSavePrettyJson(_path, data);

	
	string command = "python ../analysis/tsne.py";
	command += " -f " + _path;
	command += " -p " + ofToString(_perplexity);
	command += " -l " + ofToString(_learningRate);
	command += " -i " + ofToString(_iterations);
	system(command.c_str());
	_running = true;
}

void DimensionalityReduction::check()
{
	if (ofFile::doesFileExist(_tsnePath)) _completed = true;
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
	ofJson data = ofLoadJson(_tsnePath);
	end();
	return data;
}

void DimensionalityReduction::end()
{
	ofFile::removeFile(_path);
	ofFile::removeFile(_tsnePath);
	_path = "";
	_tsnePath = "";
	_running = false;
	_completed = false;
}
