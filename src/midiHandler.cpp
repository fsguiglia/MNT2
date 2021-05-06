#include "midiHandler.h"

MIDIHandler::MIDIHandler()
{
	_maxMessages = 1;
	
	ofxMidiIn midiIn;
	_inPorts = midiIn.getInPortList();
	_inputs.assign(_inPorts.size(), ofxMidiIn());
	_inputMode.assign(_inPorts.size(), -1);

	ofxMidiOut midiOut;
	_outPorts = midiOut.getOutPortList();
	_outputs.assign(_outPorts.size(), ofxMidiOut());
	_outputMode.assign(_inPorts.size(), -1);
}

void MIDIHandler::setMaxMessages(int max)
{
	_maxMessages = max;
}

void MIDIHandler::openInput(int index, int mode)
{
	_inputs[index].openPort(_inPorts[index]);
	_inputMode[index] = mode;
}

void MIDIHandler::closeInput(int index)
{
	_inputs[index].closePort();
	_inputs[index].removeListener(this);
	_inputMode[index] = -1;
}

void MIDIHandler::openOutput(int index, int mode)
{
	_outputs[index].openPort(_inPorts[index]);
	_outputMode[index] = mode;
}

void MIDIHandler::closeOutput(int index)
{
	_outputs[index].closePort();
}

void MIDIHandler::close()
{
	for (auto& input : _inputs) input.closePort();
	for (auto& output : _outputs) output.closePort();
}

vector<string> MIDIHandler::getInputs()
{
	return _inPorts;
}

vector<string> MIDIHandler::getOutputs()
{
	return _outPorts;
}

void MIDIHandler::newMidiMessage(ofxMidiMessage& msg)
{
	_messages.push_back(msg);
	while (_messages.size() > _maxMessages) {
		_messages.erase(_messages.begin());
	}

	int index = -1;
	for (int i = 0; i < _inPorts.size(); i++)
	{
		if (_inPorts[i] == msg.portName)
		{
			index = i;
			break;
		}
	}
	if (_inputMode[index] == 1)
	{
		for (int i = 0; i < _outPorts.size(); i++)
		{
			if (_outputMode[i] == 1) _outputs[i].sendMidiBytes(msg.bytes);
		}
	}
}
