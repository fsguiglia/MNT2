#!/usr/bin/env python

from sklearn.manifold import TSNE
from sklearn.decomposition import PCA

import numpy as np
import librosa

import progressbar

import os
import easygui
import argparse
import sys
import json

def main():
	args = process_arguments(sys.argv)
	output_folder = args['output_folder']
	sample_rate = int(args['sample_rate'])
	window_size = int(args['window_size'])
	hop_length = int(args['hop_length'])
	perplexity = int(args['perplexity'])
	learning_rate = int(args['learning_rate'])
	iterations = int(args['iterations'])
	mode = int(args['mode'])
	technique = int(args['technique'])
	new_path = output_folder[:output_folder.rfind('.')] + '_o.tmp'
	file_position = list()
	Y = np.array([])
	
	#save empty json and exit if no file is provided
	if args['input'] is None:
		init_path = os.environ['USERPROFILE'] + '//Desktop//'
		args['input'] = (easygui.diropenbox(default = init_path))
		if args['input'] is None:
			save(Y, file_position, new_path)
			exit()

	files = getListOfFiles(args['input'], ['.wav'])
	
	#save empty json and exit if less than 5 files are provided
	if len(files) < 5:
		save(y, file_position, new_path)
		exit()
	
	X = np.array([]).reshape(0, int(sample_rate * 0.5)) #analize half a second, this needs to be tested
	
	#load audio files and get features
	print('loading ' + str(len(files)) + ' files')
	bar = progressbar.ProgressBar(maxval=len(files), \
    widgets=[progressbar.Bar('=', '[', ']'), ' ', progressbar.Percentage()])
	bar.start()
	for index, file in enumerate(files):
		cur_file_position, cur_X = getFilePosition(file, sample_rate, mode)
		file_position.append(cur_file_position)
		X = np.concatenate((X, cur_X))
		bar.update(index + 1)
	bar.finish()
	D = getFeatures(X, window_size, hop_length)
	Y = np.zeros((len(file_position),2))
	
	#pca or pca+tsne (0.8 variance needs to be tested)
	if technique == 1:
		Y = getPCA(D, 2)
	elif technique == 0:
		pca = getPCA(D, 0.8)
		Y = getTSNE(pca, 2, perplexity, learning_rate, iterations) 
	
	Y = min_max_normalize(Y)
	
	
	save(Y, file_position, new_path)
	exit()
		
def process_arguments(args):
	parser = argparse.ArgumentParser(description='CBCS t-SNE')
	
	parser.add_argument('-i', '--input',
						action='store',
						help='path to the input directory')
	
	parser.add_argument('-f', '--output_folder',
						action='store',
						help='path to the output directory')

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

def getFilePosition(file, sample_rate, mode=0):
	y, sr = librosa.load(file, sample_rate)
	y = librosa.to_mono(y)
	shape = int(sample_rate * 0.5)

	windows = int(y.shape[0] / shape)
	if mode == 0: 
		windows = 0
	X = list()
	D = np.array([]).reshape(0, shape)
	
	for i in range(windows + 1):
		start = int(shape * i)
		end = int(shape * (i + 1))
		if end > y.shape[0]: 
			end = y.shape[0]
		X.append((file, i * 500))
		#pad with zeros if file is shorter than 0.5 seconds, this needs to be tested
		padded = np.zeros(shape)
		padded[:end] = y[start:end]
		padded = padded.reshape(1, shape)
		D = np.concatenate((D,padded))
	
	return X, D
	
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

def save(data, files, output_file):
	out = dict()
	points = dict()
	for i in range(len(files)):
		curOut = dict()
		curOut['name'] = files[i][0][0]
		curOut['pos'] = files[i][0][1]
		curOut['x'] = float(data[i][0])
		curOut['y'] = float(data[i][1])
		points[i] = curOut
	
	out['points'] = points
	with open(output_file, 'w+') as f:
		json.dump(out, f, indent = 4)
		
		
	

main()