import argparse
import sys

import train
import generate

def main():
	args = process_arguments(sys.argv)
	#io
	script = int(args['script'])
	file = args['file']
	
	#lstm
	length = int(args['sequence_length'])
	h1_n = int(args['hidden_units'])
	h2_n = int(args['hidden_units'])
	epochs = int(args['epochs'])
	batch_size = int(args['batch_size'])
	mode = int(args['mode'])
	mdn_components = int(args['mdn_components'])
	lr = float(args['learning_rate'])
	temperature = float(args['temperature'])
	
	if script == 0:
		train.train(file, length, h1_n, h2_n, epochs, batch_size, mode, mdn_components, lr)
	if script == 1:
		generate.generate(file, temperature, mode, mdn_components)
		
def process_arguments(args):
	parser = argparse.ArgumentParser(description='gesture LSTM')
	#input/output
	parser.add_argument('-s', '--script',
						action='store',
						default= -1,
						help='script, 0: train, 1: generate')

	parser.add_argument('-f', '--file',
						action='store',
						help='path to the input/output file or directory')
	#train
	parser.add_argument('-sl', '--sequence_length',
						action='store',
						default=-1,
						help='sequence length')
						
	parser.add_argument('-e', '--epochs',
						action='store',
						default=-1,
						help='epochs')
	
	parser.add_argument('-b', '--batch_size',
						action='store',
						default=-1,
						help='batch size')
	
	parser.add_argument('-hu', '--hidden_units',
						action='store',
						default=-1,
						help='hidden units')
	
	parser.add_argument('-m', '--mode',
						action='store',
						default=-1,
						help='mode: 0 for lstm, 1 for lstm + mdn')
	
	parser.add_argument('-lr', '--learning_rate',
						action='store',
						default=-1,
						help='learning rate')
	
	parser.add_argument('-mc', '--mdn_components',
						action='store',
						default=-1,
						help='mdn components')

	#generate
	parser.add_argument('-t', '--temperature',
						action='store',
						default=-1,
						help='temperature')

	return vars(parser.parse_args())

main()