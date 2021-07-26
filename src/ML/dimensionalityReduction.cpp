#include "dimensionalityReduction.h"

DimensionalityReduction::DimensionalityReduction()
{
	//set default values
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

void DimensionalityReduction::start(ofJson data)
{
	//crea archivo para analisis
	//llama al script de python
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
