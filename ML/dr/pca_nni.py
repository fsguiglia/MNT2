#!/usr/bin/env python

import argparse
import sys
import json
import numpy as np
from sklearn.decomposition import PCA

def analyze(args):
	path = args['file']
	dimentions = int(args['perplexity'])
	
	data = dict()
	parameters = []
	
	with open(path) as f:	
		data = json.load(f)
	#extract parameters from json
	for point in data['points']:
		cur_parameters = []
		for parameter in point['parameters'].values():
			cur_parameters.append(parameter)
		parameters.append(cur_parameters)
	
	#pca
	pca = PCA(n_components=2)
	y = pca.fit_transform(parameters)
	y = min_max_normalize(y)
	y = y.tolist()
	
	#insert new positions back to original data
	for point, pos in zip(data['points'],y):
		point['pos']['x'] = pos[0]
		point['pos']['y'] = pos[1]
	
	#save data
	new_path = path[:path.rfind('.')] + '_o.tmp'
	save(data, new_path)

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