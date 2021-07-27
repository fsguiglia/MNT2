#!/usr/bin/env python

from sklearn.manifold import TSNE
from sklearn.decomposition import PCA
from scipy import stats

import os
import easygui
import argparse
import sys
import json
import numpy as np
import matplotlib.pyplot as plt

def main():
	args = process_arguments(sys.argv)
	if args["input_file"] is None: 
		exit()
	
	data = dict()
	parameters = list()
	path = args["input_file"]
	
	with open(path) as f:
		data = json.load(f)
	
	#aca el analisis, va un random de placeholder
	for point in data["points"]:
		point["pos"]["x"] = np.random.random()
		point["pos"]["y"] = np.random.random()
	
	new_path = path[:path.rfind('.')] + "_tsne.tmp"
	
	with open(new_path, "w+") as f:
		json.dump(data, f, indent = 4)
	
def process_arguments(args):
	parser = argparse.ArgumentParser(description='CBCS t-SNE')
	
	parser.add_argument("-f", "--input_file",
						action='store',
						help='path to the input file')

	parser.add_argument("-d", "--dimentions",
						action='store',
						default=2,
						help='Dimension of the embedded space (default: 2)')

	parser.add_argument("-p", "--perplexity",
						action='store',
						default=30,
						help='Perplexity (default: 30)')
						
	parser.add_argument("-l", "--learning_rate",
						action='store',
						default = 1,
						help = 'Learning rate (default: 1)')
						
	parser.add_argument("-i", "--iterations",
						action='store',
						default=3,
						help='iterations (default: 10)')
						
	parser.add_argument("-pl", "--plot",
						action = "store_true",
						help='plot')
	
	parser.add_argument("-n", "--normalize",
						action = "store_true",
						help='normalize features')
	
	parser.add_argument("-z", "--z_score",
						action = "store",
						default = -1,
						help='remove outliers with z score > n')

	return vars(parser.parse_args())

def min_max_normalize(a):
	if(np.ptp(a) != 0):	
		a = (a - np.min(a)) / np.ptp(a)
	return(a)

def normalize(a):
	if(np.std(a) != 0):
		a-=np.mean(a)
		a/=np.std(a)
	return(a)

def remove_outliers(a, threshold):
	pass
	#remove_outliers (lo quiero hacer?)
	
def normalize_(a):
	pass
	#normalize

def process_json(d, args):
	feature_list = []
	
	for sample in d["samples"]:
		cur_sample = sample["fft"]
		feature_list.append(cur_sample)

	n = args["dimentions"]
	p = int(args["perplexity"])
	
	print("---------PCA," + str(n) + " dimentions--------")
	print("processing...")
	pca = PCA(n_components=2)
	principal_components = pca.fit_transform(feature_list)
	if args["plot"]:
		plt.title("PCA - retained variance: " + str(np.sum(pca.explained_variance_ratio_)))
		plt.plot(np.transpose(principal_components)[0], np.transpose(principal_components)[1], 'bo')
		plt.show()
	
	print("--------t-SNE, 1 dimention--------")
	tsne_1D = TSNE(n_components=1, perplexity=p, verbose=2).fit_transform(feature_list)
	
	print("--------t-SNE," + str(n) + " dimentions--------")
	tsne_nD = TSNE(n_components=n, perplexity=p, verbose=2).fit_transform(feature_list)
	if args["plot"]:
		plt.title("t-SNE output, perplexity: " + str(args["perplexity"]))
		plt.plot(np.transpose(tsne_nD)[0], np.transpose(tsne_nD)[1], 'bo')
		plt.show()
	
	for sample, xy in zip(d["samples"],tsne_nD):
		sample["tsne_x"] = xy[0].item()
		sample["tsne_y"] = xy[1].item()
			
	for sample, xy in zip(d["samples"],principal_components):
		sample["pca_x"] = xy[0].item()
		sample["pca_y"] = xy[1].item()
			
	for sample, value in zip(d["samples"],tsne_1D):
		sample["tsne_1D"] = float(value)
	
main()