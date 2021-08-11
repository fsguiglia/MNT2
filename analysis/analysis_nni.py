#!/usr/bin/env python

#!/usr/bin/env python

from sklearn.manifold import TSNE
from sklearn.decomposition import PCA
import numpy as np
import librosa

from time import sleep
import progressbar
import os
import easygui
import argparse
import sys
import json

def main():
	args = process_arguments(sys.argv)
	json_path =	args['input_file']
	audio_path = args['audio_files']
	sample_rate = int(args['sample_rate'])
	window_size = int(args['window_size'])
	hop_length = int(args['hop_length'])
	perplexity = int(args['perplexity'])
	learning_rate = int(args['learning_rate'])
	iterations = int(args['iterations'])
	mode = int(args['mode'])
	technique = int(args['technique'])
	
	#exit if no file is provided
	if args['input_file'] is None: 
		print("no input file, exiting")
		sleep(1)
		exit()
		
	new_path = json_path[:json_path.rfind('.')] + '_o.tmp'
	data = dict()
	parameters = []
	
	with open(json_path) as f:
		data = json.load(f)
	for point in data['points']:
		cur_parameters = []
		if "parameters" not in point:
			print("zones need to have parameters before analysisi, exiting")
			save(data, new_path)
			sleep(1)
			exit()
		for parameter in point['parameters'].values():
			cur_parameters.append(parameter)
		parameters.append(cur_parameters)
		
	#save json and exit if no audio files are provided
	if args['audio_files'] is None:
		init_path = os.environ['USERPROFILE'] + '//Desktop//'
		args['audio_files'] = (easygui.diropenbox(default = init_path))
		if args['audio_files'] is None:
			save(data, new_path)
			sleep(1)
			exit()

	files = getListOfFiles(args['audio_files'], ['.wav'])
	validFiles = list()
	#save json and exit if files dont match zones
	for point in data["points"]:
		zone_id = point["id"]
		id_audio_file_exists = False
		for file in files:
			fileName = os.path.basename(file)
			if fileName[:-4] == str(zone_id):
				id_audio_file_exists = True
				validFiles.append(file)
				break
		if id_audio_file_exists == False:
			print("file missing: " + str(zone_id) + ".wav, exiting") 
			save(data, new_path)
			sleep(1)
			exit()
	
	X = np.array([]).reshape(0, int(sample_rate * 0.5)) #analize half a second, this needs to be tested
	
	#load audio files and get features
	print('loading ' + str(len(files)) + ' files')
	bar = progressbar.ProgressBar(maxval=len(files), \
    widgets=[progressbar.Bar('=', '[', ']'), ' ', progressbar.Percentage()])
	bar.start()
	for index, file in enumerate(validFiles):
		cur_X = getFilePosition(file, sample_rate)
		X = np.concatenate((X, cur_X))
		bar.update(index + 1)
	bar.finish()
	D = getFeatures(X, window_size, hop_length)
	y = np.zeros((len(files),2))
	
	#pca or pca+tsne (0.8 variance needs to be tested)
	if technique == 1:
		y = getPCA(D, 2)
	elif technique == 0:
		pca = getPCA(D, 0.8)
		y = getTSNE(pca, 2, perplexity, learning_rate, iterations) 
	
	y = min_max_normalize(y)
	for point, pos in zip(data['points'],y):
		point['pos']['x'] = float(pos[0])
		point['pos']['y'] = float(pos[1])
	
	save(data, new_path)
	exit()
		
def process_arguments(args):
	parser = argparse.ArgumentParser(description='CBCS t-SNE')

	parser.add_argument('-f', '--input_file',
						action='store',
						help='path to the output directory')
	
	parser.add_argument('-a', '--audio_files',
						action='store',
						help='path to the input directory')
	
	parser.add_argument('-d', '--dimentions',
						action='store',
						default=2,
						help='Dimension of the embedded space (default: 2)')

	parser.add_argument('-p', '--perplexity',
						action='store',
						default=30,
						help='Perplexity (default: 30)')
						
	parser.add_argument('-l', '--learning_rate',
						action='store',
						default = 200,
						help = 'Learning rate (default: 200)')
						
	parser.add_argument('-it', '--iterations',
						action='store',
						default=1000,
						help='Iterations (default: 1000)')
	
	parser.add_argument('-sr', '--sample_rate',
						action='store',
						default=22500,
						help='Sample rate (default: 22500)')
						
	parser.add_argument('-ws', '--window_size',
						action='store',
						default=2048,
						help='Window size (default: 2048)')
	
	parser.add_argument('-hl', '--hop_length',
						action='store',
						default=512,
						help='Hop length (default: 512)')
	
	parser.add_argument('-m', '--mode',
					 action='store',
					 default=0,
					 help= 'mode (0:sample, 1:granular)')
	
	parser.add_argument('-t', '--technique',
					 action='store',
					 default=1,
					 help='Dimensionality reduction technique (0: tsne, 1: pca)')

	return vars(parser.parse_args())

def getListOfFiles(dirName, extensions):
	listOfFile = os.listdir(dirName)
	allFiles = list()
	for entry in listOfFile:
		fullPath = os.path.join(dirName, entry)
		if os.path.isdir(fullPath):
			allFiles = allFiles + getListOfFiles(fullPath, extensions)
		else:
			valid_extension = False
			for extension in extensions:
				if fullPath[-4:] == extension:
					valid_extension = True
					break
			if(valid_extension):
				allFiles.append(fullPath)
	return allFiles

def getFilePosition(file, sample_rate):
	y, sr = librosa.load(file, sample_rate)
	y = librosa.to_mono(y)
	shape = int(sample_rate * 0.5)
	#pad with zeros if file is shorter than 0.5 seconds, this needs to be tested
	padded = np.zeros(shape)
	y_lenght = y.shape[0]
	if y_lenght > shape:
		y_lenght = shape
	padded[:y_lenght] = y[:y_lenght]
	padded = padded.reshape(1, shape)
	return padded
	
def getFeatures(samples, window_size, hop_length):
	#size of stft result is  (window size / 2 + 1) * (samples / hop length + 1)
	shape = int((1 + window_size * 0.5))
	shape *= int(samples[1].shape[0] / hop_length) + 1
	D = np.array([]).reshape(0, shape)
	for sample in samples:
		sample = sample.T
		S = librosa.stft(sample, n_fft = window_size, hop_length = hop_length)
		S = np.abs(S)
		S = S.reshape(S.shape[0] * S.shape[1])
		S = S.reshape(1, shape)
		D = np.concatenate((D, S))
	return D

def getPCA(data, components):
	print('Principal component analysis')
	pca = PCA(n_components=components)
	pca.fit(data)
	Y = pca.transform(data)
	print('done, ' + str(Y.shape[1]) + ' components')
	return Y

def getTSNE(data, components, perplexity, learning_rate, iterations):
	print('t-distributed stochastic neighbor embedding')
	tsne = TSNE(n_components = components,
			 perplexity = perplexity,
			 learning_rate = learning_rate,
			 n_iter = iterations,
			 verbose = 1,
			 )
	Y = tsne.fit_transform(data)
	print('done!')
	return Y

def min_max_normalize(a):
	a = np.transpose(a)
	max_values = np.amax(a,1)
	min_values = np.amin(a,1)
	a = np.transpose(a)
	norm = (a - min_values) / (max_values - min_values)
	return norm

def save(data, path):
	with open(path, 'w+') as f:
		json.dump(data, f, indent = 4)
		
		
	

main()	