import argparse
import sys

import analysis_cbcs
import analysis_nni
import pca_nni

def main():
	args = process_arguments(sys.argv)
	#input/output
	script = int(args['script'])
	file = args['file']
	#audio
	sample_rate = int(args['sample_rate'])
	window_size = int(args['window_size'])
	hop_length = int(args['hop_length'])
	#t-sne
	dimentions = int(args['perplexity'])
	perplexity = int(args['perplexity'])
	learning_rate = int(args['learning_rate'])
	iterations = int(args['iterations'])
	#cbcs
	cbcs_mode = int(args['cbcs_mode'])
	technique = int(args['technique'])

	if script == 0:
		analysis_cbcs.analyze(args)
	elif script == 1:
		analysis_nni.analyze(args)
	elif script == 2:
		pca_nni.analyze(args)
		
def process_arguments(args):
	parser = argparse.ArgumentParser(description='MNT Analysis')
	
	#seleccion de script-------------------------------------
	parser.add_argument('-s', '--script',
						default = -1,
						action='store',
						help='analysis script, 0:cbcs, 1:nni_audio, 2:nni_data')
	#input/output--------------------------------------------
	parser.add_argument('-f', '--file',
						action='store',
						help='path to the input/output file or directory')

	#dimensionality reduction technique----------------------
	parser.add_argument('-t', '--technique',
					 action='store',
					 default=1,
					 help='Dimensionality reduction technique (0: tsne, 1: pca)')
	#tsne----------------------------------------------------
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
	#audio----------------------------------------------------
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
	#cbcs----------------------------------------------------
	parser.add_argument('-cm', '--cbcs_mode',
					 action='store',
					 default=0,
					 help= 'mode (0:sample, 1:granular)')
	
	return vars(parser.parse_args())
	
	
main()

