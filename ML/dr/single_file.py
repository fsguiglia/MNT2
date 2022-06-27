import enlighten
import librosa
import soundfile as sf
import numpy as np
import json
import easygui
import os
import time

def export(args):
    error = 'something went wrong'
    debug = False

    try:
        input_file = args['file']
        samplerate = int(args['output_sample_rate'])
        unit_length = int(args['unit_length'])
        default_path = os.environ['USERPROFILE'] + '//Desktop//concatenate.wav'
        new_path = input_file[:input_file.rfind('.')] + '_o.tmp'
        
        files = load_json(input_file)
        X, D = concatenate(files, unit_length, samplerate)
        saveData(D, new_path)
        saveFile(X, default_path, samplerate)
    
    except Exception as ex:
        if(debug):
            print(ex)
            input("Press key to exit.")
        
        save_empty_file(error, new_path)

def load_json(file):
    f = open(file)
    data = json.load(f)
    X = list()
    for file in data['files']:
        X.append(file)
    return X

def concatenate(files, unit_length, samplerate):
    X = np.array([])
    s = np.zeros(int(unit_length / 1000 * samplerate))
    manager = enlighten.get_manager()
    file_progress = manager.counter(total=len(files), desc='Files', unit='file', leave=True)
    print("concatenating " + str(len(files)) + " files...")
    D = dict()

    for file in files:
        D[file] = int(1000 * X.shape[0] / samplerate)
        cur_X, sr = librosa.load(file, samplerate)
        cur_X = librosa.to_mono(cur_X)
        X = np.append(X, cur_X)
        X = np.append(X, s)
        file_progress.update()
    
    manager.stop()
    return X, D 

def saveFile(X, path, samplerate):
    output = easygui.filesavebox(msg="Save file", title="MNT2", default=path)
    sf.write(output, X, samplerate, format='wav')

def saveData(D, output_file):
    out = dict()
    out["files"] = D
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


