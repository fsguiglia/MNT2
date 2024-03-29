import argparse
import sys

import analysis_audio
import analysis_midi
import pca_nni
import single_file

def main():
	args = process_arguments(sys.argv)
	script = int(args['script'])
	
	if script == 0:
		analysis_audio.analyze(args)
	elif script == 1:
		analysis_midi.analyze(args)
	elif script == 2:
		single_file.export(args)
		
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

	parser.add_argument('-osr', '--output_sample_rate',
						action='store',
						default=44100,
						help='Output sample rate (default: 44100)')
						
	parser.add_argument('-ws', '--window_size',
						action='store',
						default=2048,
						help='Window size (default: 2048)')
	
	parser.add_argument('-hl', '--hop_length',
						action='store',
						default=512,
						help='Hop length (default: 512)')
						
	parser.add_argument('-ul', '--unit_length',
						action='store',
						default = 500,
						help='Unit length in ms (default: 500)')
	#cbcs----------------------------------------------------
	parser.add_argument('-cm', '--cbcs_mode',
					 action='store',
					 default=0,
					 help= 'mode (0:sample, 1:granular)')
	
	return vars(parser.parse_args())
	
	
main()

