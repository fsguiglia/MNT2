#include "dimensionalityReduction.h"

DimensionalityReduction::DimensionalityReduction()
{
	//set default values
	_perplexity = 0;
	_learningRate = 0;
	_iterations = 0;
	_running = false;
	_analizing = false;
	_completed = false;
}

void DimensionalityReduction::setup(int perplexity, int learningRate, int iterations)
{
	_perplexity = perplexity;
	_learningRate = learningRate;
	_iterations = iterations;
}

void DimensionalityReduction::start(ofJson data, string name)
{
	_path = "../../analysis/tmp/" + name + "_analisis";
	string curPath = _path + ".json";
	ofSavePrettyJson(curPath, data);

	string absolutePath = ofFilePath::getAbsolutePath(curPath);
	string command = "python ../analysis/tsne.py";
	command += " -f " + absolutePath;
	command += " -p " + ofToString(_perplexity);
	command += " -l " + ofToString(_learningRate);
	command += " -i " + ofToString(_iterations);
	system(command.c_str());

	_running = true;
}

void DimensionalityReduction::check()
{


		
	//chequea si python creo el archivo de analisis
	//si ya estamos llama a end
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
	ofJson data;
	_completed = false;
	return data;
}

void DimensionalityReduction::end()
{
	//carga los puntos y borra los archivos de analisis
	_completed = true;
}
