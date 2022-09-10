import argparse
import sys
import json
import numpy as np
import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'  # or any {'0', '1', '2'}
#import tensorflow as tf
from tensorflow import keras
from tensorflow.keras import layers
import mdn
import easygui

def train(args):
	print('-----------------------------------------------------')
	print('-----------------------------------------------------')
	print('----------------go do something else-----------------')
	print('----------this is going to take a LONG time----------')
	print('-----------------------------------------------------')
	print('-----------------------------------------------------')
		
	path = args['file']
	length = int(args['sequence_length'])
	h1_n = int(args['hidden_units'])
	h2_n = int(args['hidden_units'])
	epochs = int(args['epochs'])
	batch_size = int(args['batch_size'])
	mode = int(args['mode'])
	mdn_components = int(args['mdn_components'])
	learning_rate = float(args['learning_rate'])

	with open('../ML/lstm/hyperparameters.ini') as f:
		hp = json.load(f)
		if length == -1: length = hp['sequence_length']
		if h1_n == -1: h1_n = hp['h1']
		if h2_n == -1: h2_n = hp['h2']
		if epochs == -1: epochs = hp['epochs']
		if batch_size == -1: batch_size = hp['batch_size']
		if mdn_components == -1: mdn_components = hp['mdn_components']
		if mode == -1: mode = int(hp['mode'])
		if learning_rate == -1: learning_rate = hp['learning_rate']
	
	data = dict()
	
	with open(path) as f:	
		data = json.load(f)
	
	gesture_list,t = get_gestures(data)
	#mean, std = get_mean_std(gesture_list)
	
	#define model
	if mode == 0:
		model = simple_lstm(h1_n, h2_n, batch_size, length, learning_rate)
	else:
		model = lstm_mdn(h1_n, h2_n, batch_size, length, learning_rate, mdn_components)
	model.summary()
	
	#fit model
	for i in range(epochs):
		cur_epoch = "epoch " + str(i + 1) + "/" + str(epochs)
		for j, gesture in enumerate(gesture_list):
			print(cur_epoch + ', gesture ' + str(j+1) + "/" + str(len(gesture_list)) + ": ", end = " ")
			X,y = get_examples(gesture, length)
			#X -= mean
			#X /= std
			#y -= mean
			#y /= std
			history = model.fit(X, y, epochs=1, batch_size=1, verbose=2, shuffle=False)
			model.reset_states()

	new_path = path[:path.rfind('.')] + '_o.tmp'
	with open(new_path, 'w+') as f:
		json.dump(data, f, indent = 4)
	
	default_path = os.environ['USERPROFILE'] + '//Desktop//' + '/model.h5'
	model_file = easygui.filesavebox(msg=None, title="Save model", default=default_path, filetypes=None)
	if model_file != None:
		model.save(model_file)
		print('saved as: ' + model_file)

def get_gestures(data):
	time_sum = 0
	num_points = 0
	gesture_list = list()

	for gesture in data["gestures"]:
		cur_gesture = list()
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
	return gesture_list, time

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

def rotate(sequence, radians):
	

def get_examples(sequence, length):
	X = list()
	y = list()
	
	for i in range(0, len(sequence)-length):
		X.append(sequence[i: i + length])
		y.append(sequence[i + length])
		
	X = np.array(X)
	y = np.array(y)
	return X,y
	
def simple_lstm(h1n, h2n, batch_size, length, lr):
	model = keras.Sequential()
	model.add(layers.LSTM(h1n, batch_input_shape=(batch_size, length, 2), return_sequences=True, stateful=True))
	if h2n > 0: model.add(layers.LSTM(h2n, stateful=True))
	model.add(layers.Dense(2, activation='linear'))
	optimizer = keras.optimizers.Adam(learning_rate=lr)
	model.compile(loss='mse', optimizer=optimizer)
	return model

def lstm_mdn(h1n, h2n, batch_size, length, lr, mdn_components):
	model = keras.Sequential()
	model.add(layers.LSTM(h1n, batch_input_shape=(batch_size, length, 2), return_sequences=True, stateful=True))
	if h2n > 0: model.add(layers.LSTM(h2n, stateful=True))
	model.add(mdn.MDN(2, mdn_components))
	optimizer = keras.optimizers.Adam(learning_rate = lr)
	model.compile(loss=mdn.get_mixture_loss_func(2,mdn_components), optimizer=optimizer)
	return model