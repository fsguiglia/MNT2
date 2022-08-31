#!/usr/bin/env python

from sklearn.manifold import TSNE
from sklearn.decomposition import PCA
import numpy as np
#import librosa

from time import sleep
import progressbar
import os
import easygui
import sys
import time
import json

from analysis_audio import save_empty_file

def analyze(args):
	json_path = args['file']
	sample_rate = int(args['sample_rate'])
	window_size = int(args['window_size'])
	hop_length = int(args['hop_length'])
	perplexity = int(args['perplexity'])
	learning_rate = int(args['learning_rate'])
	iterations = int(args['iterations'])

	error = 'something went wrong, please add more zones or paramaters'
	new_path = json_path[:json_path.rfind('.')] + '_o.tmp'

	try:
		with open(json_path) as f:
			D = json.load(f)
		X, cc, pos = get_data(D)
		PCA = getPCA(X, 0.8)
		if PCA.shape[1] < 2: 
			PCA = getPCA(X, 2)
		TSNE = getTSNE(PCA, 2, perplexity, learning_rate, iterations)
		TSNE = min_max_normalize(TSNE)
		PCA = PCA.T
		PCA = PCA[:2]
		PCA = PCA.T
		PCA = min_max_normalize(PCA)
		X = min_max_normalize(X)
		save(X, cc, pos, PCA, TSNE, new_path)
	except:
		raise
		#save_empty_file(error, new_path)

		
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

def get_data(D):
	cc = D['parameters']
	X = np.array([]).reshape(0, len(cc))
	pos = np.array([]).reshape(0, 2)

	for point in D['points']:
			curPos = np.array([float(point['pos']['x']), float(point['pos']['x'])])
			curPos = curPos.reshape(1, 2)
			pos = np.concatenate((pos, curPos))
			F = []
			for parameter in cc:
				if parameter in point['parameters']:
					F.append(float(point['parameters'][parameter]))
				else:
					F.append(0)
			F = np.array(F)
			F = F.reshape(1, len(cc))
			X = np.concatenate((X, F))
	return X, cc, pos

def min_max_normalize(a):
	a = np.transpose(a)
	max_values = np.amax(a,1)
	min_values = np.amin(a,1)
	a = np.transpose(a)
	norm = (a - min_values) / (max_values - min_values)
	return norm

def save(X, cc, pos, PCA, TSNE, output_file):
	out = dict()
	out['parameters'] = cc
	points = []
	for i in range(X.shape[0]):
		curOut = dict()
		parameters = dict()
		for j, parameter in enumerate(cc):
			parameters[parameter] = float(X[i][j])
		dr = dict()
		dr['pos_x'] = float(pos[i][0])
		dr['pos_y'] = float(pos[i][1])
		dr['pca_x'] = float(PCA[i][0])
		dr['pca_y'] = float(PCA[i][1])
		dr['tnse_x'] = float(TSNE[i][0])
		dr['tnse_y'] = float(TSNE[i][1])
		
		curOut['id'] = i
		curOut['parameters'] = parameters
		curOut['dr'] = dr
		points.append(curOut)
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