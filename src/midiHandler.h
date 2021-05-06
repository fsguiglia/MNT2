#pragma once
#ifndef _MIDIHANDLER
#define _MIDIHANDLER
#include "ofMain.h"
#include "ofxMidi.h"

class MIDIHandler : public ofxMidiListener
{
public:
	MIDIHandler();
	void setMaxMessages(int max);
	void openInput(int index, int mode);
	void closeInput(int index);
	void openOutput(int index, int mode);
	void closeOutput(int index);
	void close();
	
	vector<string> getInputs();
	vector<string> getOutputs();

private:
	void newMidiMessage(ofxMidiMessage& msg);

	int _maxMessages;
	vector<ofxMidiMessage> _messages;

	vector<ofxMidiIn> _inputs;
	vector<int> _inputMode;
	vector<string> _inPorts;

	vector<ofxMidiOut> _outputs;
	vector<int> _outputMode;
	vector<string> _outPorts;
};

#endif