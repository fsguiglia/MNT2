import argparse
import sys
import json
import os
import numpy as np
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'  # or any {'0', '1', '2'}
#import tensorflow as tf
from tensorflow import keras
from tensorflow.keras import layers
import mdn
import random
import easygui
import progressbar
'''
def main():
	#args
	args = process_arguments(sys.argv)
	#exit if no file is provided	
	if args['input_file'] is None: 
		print('no file, exiting')
		exit()
		
	json_path = args['input_file']
	temperature = float(args['temperature'])  
	mode = -1
	mdn_components = -1
	generate(input_file, temperature, mode, mdn_components)
'''
def generate(json_path, temperature, mode, mdn_components):	
	data = dict()
	new_path = json_path[:json_path.rfind('.')] + '_o.tmp'
	print('select model file')
	#get model path
	default_path = os.environ['USERPROFILE'] + '//Desktop//' + '*.h5'
	model_path = easygui.fileopenbox(msg='Load model', title='MNT2', default=default_path, filetypes=None, multiple=False)
	if model_path == None:
		print('no model loaded, exiting')
		with open(new_path, 'w+') as f:
			json.dump(data, f, indent = 4)
		exit()
	print('loading model...')
	
	#load model
	try:
		model = keras.models.load_model(model_path)
	except:
		model = keras.models.load_model(model_path, custom_objects={'MDN': mdn.MDN, 'mdn_loss_func': mdn.get_mixture_loss_func(2, mdn_components)})
	
	with open(json_path) as f:	
		data = json.load(f)
	
	#get gesture data
	try:
		gesture_list,t, max_name = get_gestures(data)
	except:
		print('no example gestures, exiting')
		with open(new_path, 'w+') as f:
			json.dump(data, f, indent = 4)
		exit()
	print('done!')
	
	#set number of gestures
	n_gestures = input('number of gestures to generate: ')
	try:
		n_gestures = int(n_gestures)
	except:
		print("that's not an number, generating 10 gestures")
		n_gestures = 10
	
	#generate
	print('generating gestures: ')
	new_sequences = generate_sequences(model, n_gestures, gesture_list, temperature)
	#prepare save file
	data['gestures'] = list()
	for i,sequence in enumerate(new_sequences):
		cur_gesture = dict()
		cur_gesture_data = list()
		for j, point in enumerate(sequence):
			cur_point = dict()
			cur_point['x'] = float(point[0])
			cur_point['y'] = float(point[1])
			cur_point['dtime'] = float(t)
			cur_point['time'] = float(t * j)
			cur_gesture_data.append(cur_point)
		cur_gesture['name'] = str(max_name + 1 + i)
		if len(cur_gesture['name']) == 1:
			cur_gesture['name'] = str(0) + cur_gesture['name']
		cur_gesture['data'] = cur_gesture_data
		data['gestures'].append(cur_gesture)
	
	with open(new_path, 'w+') as f:
		json.dump(data, f, indent = 4)
'''
def process_arguments(args):
	parser = argparse.ArgumentParser(description='gesture LSTM')
	
	parser.add_argument('-f', '--input_file',
						action='store',
						help='path to the input file')

	parser.add_argument('-t', '--temperature',
						action='store',
						default=-1,
						help='temperature')

	return vars(parser.parse_args())
'''

def get_gestures(data):
	time_sum = 0
	num_points = 0
	gesture_list = list()
	max_name = 0
	
	for gesture in data['gestures']:
		cur_gesture = list()
		if int(gesture['name']) > max_name:
			max_name = int(gesture['name'])
		for point in gesture['data']:
			cur_point = [point['x'], point['y']]
			time_sum+= point['dtime']
			num_points+= 1
			cur_gesture.append(cur_point)
		gesture_list.append(cur_gesture)
	
	#distancia temporal promedio entre puntos
	time = 0
	if(num_points > 0):
		time = time_sum / num_points
	
	return gesture_list, time, max_name

def get_mean_std(sequences):
	x = list()
	y = list()
	for sequence in sequences:
		for point in sequence:
			x.append(point[0])
			y.append(point[1])
	x = np.array(x)
	y = np.array(y)
	mean = (x.mean(), y.mean())
	std = (x.std(), y.std())
	return mean, std

def get_examples(sequence, length):
	X = list()
	y = list()
	
	for i in range(0, len(sequence)-length):
		X.append(sequence[i: i + length])
		y.append(sequence[i + length])
		
	X = np.array(X)
	y = np.array(y)
	return X,y

def generate_sequences(model, n, gesture_list, temperature):
	#get data from model
	batch_size = model.get_config()['layers'][1]['config']['batch_input_shape'][0]
	length = model.get_config()['layers'][1]['config']['batch_input_shape'][1]
	mode = 0
	mdn_components = -1
	if model.get_config()['layers'][-1]['class_name'] == 'MDN':
		mode = 1
		mdn_components = model.get_config()['layers'][-1]['config']['num_mixtures']
	
	#prepare first sequences
	#mean, std = get_mean_std(gesture_list)
	indexes = list()
	indexes_length = int(n / len(gesture_list))
	if indexes_length < 1 : indexes_length = 1
	for i in range(indexes_length):
		for j in range(len(gesture_list)):
			indexes.append(j)
	random.shuffle(indexes)
	indexes = indexes[:n]
	l = 0
	for i in range(n):
		l += len(gesture_list[indexes[i]])
	l -= length * n
	#progress bar
	bar_pos = 0
	bar = progressbar.ProgressBar(maxval=l, \
    widgets=[progressbar.Bar('=', '[', ']'), ' ', progressbar.Percentage()])
	bar.start()
	gen_sequences = list()
	
	for i in range(n):
		#get first sequence
		index = indexes[i]
		sequences, y = get_examples(gesture_list[index], length)
		cur_length = len(gesture_list[index])
		sequence = sequences[0]
		#sequence -= mean
		#sequence /= std
		predicted = np.zeros((cur_length, 2))
		predicted[:length] = sequence

		#generate
		for j in range(length, cur_length):
			y_hat = model.predict(sequence.reshape(batch_size,length,2), batch_size=batch_size)
			if mode == 1: y_hat = mdn.sample_from_output(y_hat[0], 2, mdn_components, temp=temperature)
			sequence[:-1] = sequence[1:]
			sequence[-1] = y_hat
			predicted[j] = y_hat
			model.reset_states()
			bar_pos += 1
			bar.update(bar_pos)
		
		#predicted *= std
		#predicted += mean
		gen_sequences.append(predicted.tolist())
		
	bar.finish()
	return gen_sequences

#main()