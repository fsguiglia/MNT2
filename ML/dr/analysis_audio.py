#!/usr/bin/env python

from sklearn.manifold import TSNE
from sklearn.decomposition import PCA

import numpy as np
import librosa

import enlighten

import os
import easygui
import json
import time

def analyze(args):
	output_folder = args['file']
	sample_rate = int(args['sample_rate'])
	window_size = int(args['window_size'])
	hop_length = int(args['hop_length'])
	perplexity = int(args['perplexity'])
	learning_rate = int(args['learning_rate'])
	iterations = int(args['iterations'])
	mode = int(args['cbcs_mode'])
	unit_length = float(args['unit_length']) / 1000 #seconds
	
	error = 'something went wrong, please check path and folder contents'
	new_path = output_folder[:output_folder.rfind('.')] + '_o.tmp'
	
	try:
		file_position = list()
		Y = np.array([])
		
		#save empty json and exit if no file is provided
		init_path = os.environ['USERPROFILE'] + '//Desktop//'
		audio_files = (easygui.diropenbox(msg='Audio file folder', title='MNT2', default = init_path))
		if audio_files is None:
			error = 'no files found'
			save_empty_file(error, new_path)

		files = getListOfFiles(audio_files, ['.wav', '.WAV'])
		
		X = np.array([]).reshape(0, int(sample_rate * unit_length)) #samples
		
		#load audio files and get features
		manager = enlighten.get_manager()
		file_progress = manager.counter(total=len(files), desc='Files', unit='file', leave=True)
		print('loading ' + str(len(files)) + ' file(s)...')
		for index, file in enumerate(files):
			try:
				cur_file_position, cur_X = getFilePosition(manager, file, sample_rate, unit_length, mode)
				for file in cur_file_position:	
					file_position.append(file)
				X = np.concatenate((X, cur_X))
			except:
				border_msg(file + ' is not a valid file')
			file_progress.update()
		
		print('done')
		#save empty json and exit if less than 5 units are found
		if len(file_position) < 5:
			error = 'not enough data to create map'
			save_empty_file(error, new_path)

		print('getting features...')
		F, MFCC = getFeatures(manager, X, window_size, hop_length)
		TSNE = np.zeros((len(file_position),2))
		
		#0.8 variance needs to be tested
		PCA = getPCA(MFCC, 0.8)
		if PCA.shape[1] < 2: 
			PCA = getPCA(MFCC, 2)
		TSNE = getTSNE(PCA, 2, perplexity, learning_rate, iterations)
		TSNE = min_max_normalize(TSNE)
		#keep first two components of PCA and normalize
		PCA = PCA.T
		PCA = PCA[:2]
		PCA = PCA.T
		PCA = min_max_normalize(PCA)
		#normalize features
		F = min_max_normalize(F)
		#save
		save(F, PCA, TSNE, files, file_position, new_path)
		manager.stop()
	except Exception as ex:
		#raise
		save_empty_file(error, new_path)
		
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

def getFilePosition(manager, file, sample_rate, unit_length, mode=0):
	y, sr = librosa.load(file, sample_rate)
	y = librosa.to_mono(y)
	shape = int(sample_rate * unit_length) #unit length in samples
	units = int(y.shape[0] / shape)
	if mode == 0: 
		units = 0
	X = list() #list of file names and unit position in ms
	D = np.array([]).reshape(0, shape) #unit samples
	
	
	bar = manager.counter(total=units+1, leave=False)

	for i in range(units + 1):
		start = int(shape * i)
		end = int(shape * (i + 1))
		if end > y.shape[0]: 
			end = y.shape[0]
		X.append((file, i * unit_length * 1000))
		#pad with zeros if file is shorter than unit_length
		padded = np.zeros(shape)
		padded[:end-start] = y[start:end]
		padded = padded.reshape(1, shape)
		D = np.concatenate((D,padded))
		bar.update()
	bar.close()
	return X, D

def getFeatures(manager, samples, window_size, hop_length):
	#size of mfcc result ONLY FOR DEFAULT PARAMETERS -> fix
	shape = 19 * int(1 + samples[0].shape[0] / 512)
	D = np.array([]).reshape(0, shape)
	#rms, centroid, bandwidth, flatness, rolloff
	F = np.array([]).reshape(0, 5)
	#stack fft results
	bar = manager.counter(total=samples.shape[0], desc="STFT");
	for index,sample in enumerate(samples):
		sample = sample.T
		S = librosa.stft(sample, n_fft = window_size, hop_length = hop_length)
		#featuress
		mp, phase = librosa.magphase(S)
		rms =librosa.feature.rms(S=mp)[0][0]
		sc = librosa.feature.spectral_centroid(S=mp)[0][0]
		bw = librosa.feature.spectral_bandwidth(S=mp)[0][0]
		sf = librosa.feature.spectral_flatness(S=mp)[0][0]
		sr = librosa.feature.spectral_rolloff(S=mp)[0][0]
		f =  np.array([rms, sc, bw, sf, sr]) 
		f = f.reshape(1,5)
		F = np.concatenate((F,f))

		mfcc = librosa.feature.mfcc(sample)[1:]
		mfcc = mfcc.reshape(mfcc.shape[0] * mfcc.shape[1])
		mfcc = mfcc.reshape(1, shape)
		D = np.concatenate((D,mfcc))
		bar.update()
	
	return F, D

	for index, sample in (enumerate(samples)):
		mfcc = librosa.feature.mfcc(sample)[1:]
		mfcc = mfcc.reshape(mfcc.shape[0] * mfcc.shape[1])
		mfcc = mfcc.reshape(1, shape)
		Y = np.concatenate((D,mfcc))
		
	return Y

def getPCA(data, components):
	print('Principal component analysis (this can take a while)...')
	pca = PCA(n_components=components)
	pca.fit(data)
	Y = pca.transform(data)
	print('done, ' + str(Y.shape[1]) + ' components')
	return Y

def getTSNE(data, components, perplexity, learning_rate, iterations):
	print('t-distributed stochastic neighbor embedding (this can take a while)...')
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
	min_max = np.amax(a.T,1) - np.amin(a.T,1)
	min_max[min_max == 0] = 1
	X = (a - np.amin(a.T,1)) / min_max
	return X

def save(F, PCA, TSNE, files, positions, output_file):
	out = dict()
	points = dict()
	for i in range(len(positions)):
		curOut = dict()
		curOut['name'] = positions[i][0]
		curOut['position'] = positions[i][1]
		curOut['tsne-x'] = float(TSNE[i][0])
		curOut['tsne-y'] = float(TSNE[i][1])
		curOut['pca-x'] = float(PCA[i][0])
		curOut['pca-y'] = float(PCA[i][1])
		curOut['rms'] = float(F[i][0])
		curOut['centroid'] = float(F[i][1])
		curOut['bandwidth'] = float(F[i][2])
		curOut['flatness'] = float(F[i][3])
		curOut['rolloff'] = float(F[i][4])

		if len(files) == 1:
			curOut['single-file-position'] = positions[i][1]
		else:
			curOut['single-file-position'] = -1
		points[i] = curOut
	
	out['features'] = ['pca-x', 'pca-y', 'tsne-x', 'tsne-y', 'rms', 'centroid', 'bandwidth', 'flatness', 'rolloff']
	out['selected'] = ['tsne-x', 'tsne-y']
	out['points'] = points

	with open(output_file, 'w+') as f:
		json.dump(out, f, indent = 4)

def save_empty_file(error, output_file):
	border_msg(error)
	time.sleep(3)
	out = dict()
	out["error"] = error
	with open(output_file, 'w+') as f:
		json.dump(out, f, indent = 4)
	exit()
	
def border_msg(msg):
	row = len(msg)
	h = ''.join(['-' *row])
	result= h + '\n' +msg+ '\n' + h
	print('\n')
	print(result)
	print('\n')