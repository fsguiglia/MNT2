#!/usr/bin/env python


import argparse
import sys
import json
import numpy as np
from sklearn.manifold import TSNE

def main():
	args = process_arguments(sys.argv)
	#exit if no file is provided
	if args['input_file'] is None: 
		exit()
	
	data = dict()
	parameters = []
	path = args['input_file']
	dimentions = int(args['dimentions'])
	perplexity = int(args['perplexity'])
	learning_rate = int(args['learning_rate'])
	iterations = int(args['iterations'])
	
	with open(path) as f:
		data = json.load(f)
	#extract parameters from json
	for point in data['points']:
		cur_parameters = []
		for parameter in point['parameters'].values():
			cur_parameters.append(parameter)
		parameters.append(cur_parameters)
	
	#t-sne
	augmentation = 2
	y = tsne(parameters, dimentions, perplexity, learning_rate, iterations, augmentation)
	y = min_max_normalize(y)
	y = y.tolist()
	
	#insert new positions back to original data
	for point, pos in zip(data['points'],y):
		point['pos']['x'] = pos[0]
		point['pos']['y'] = pos[1]
	
	#save data
	new_path = path[:path.rfind('.')] + '_o.tmp'
	
	with open(new_path, 'w+') as f:
		json.dump(data, f, indent = 4)
	
def process_arguments(args):
	parser = argparse.ArgumentParser(description='CBCS t-SNE')
	
	parser.add_argument('-f', '--input_file',
						action='store',
						help='path to the input file')

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
						default = 1,
						help = 'Learning rate (default: 1)')
						
	parser.add_argument('-i', '--iterations',
						action='store',
						default=250,
						help='Iterations (default: 10)')
						
	
	parser.add_argument('-n', '--normalize',
						action = 'store_true',
						help='Normalize positions')
	

	return vars(parser.parse_args())


def tsne(data, d, p, l, i, a):
	print('--------processing--------')
	X = np.array(data)
	X = augment(X,a)
	
	tsne_2d = TSNE(
		n_components = d,
		perplexity = p,
		learning_rate= l,
		n_iter = i,
		early_exaggeration = 6).fit_transform(X)
	
	tsne_2d = tsne_2d[:int(X.shape[0] / (a + 1))]
	
	return tsne_2d

def augment(a, n):
	random = a
	for i in range(n):
		cur_random = np.random.rand(a.shape[0], a.shape[1])
		cur_random *= 0.2
		cur_random -= 0.1
		random = np.concatenate((random, cur_random))
	return random
	

def min_max_normalize(a):
	a = np.transpose(a)
	max_values = np.amax(a,1)
	min_values = np.amin(a,1)
	a = np.transpose(a)
	norm = (a - min_values) / (max_values - min_values)
	return norm

	
		 
main()