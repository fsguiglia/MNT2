#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "ofxXmlSettings.h"
#include "ofxNetwork.h"
#include "ofxOsc.h"
#include "ofxMidi.h"
#include "page.h"
#include "NNIPage.h"
#include "triggerPage.h"
#include "rgbPage.h"
#include "mntUtils.h"

class ofApp : public ofBaseApp, public ofxMidiListener {

	public:
		void setup();
		void update();
		void draw();
		void exit();
		//--------------------------------------------------------------
		void setupMIDI();
		void setupMIDIGui();
		void updateMIDIGui(bool visible);
		void MIDIInToggle(ofxDatGuiToggleEvent e);
		void MIDIOutToggle(ofxDatGuiToggleEvent e);
		void newMidiMessage(ofxMidiMessage& msg);
		map<string, float> removePortFromMessages(map<string, float> messages);
		void sendMIDICC(map<string, float> parameters, map<string, ofxMidiOut> ports);
		void drawMIDI();
		//--------------------------------------------------------------
		void load();
		void save();
		void setWindowTitle(string title);
		//--------------------------------------------------------------
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseScrolled(ofMouseEventArgs& mouse);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		//NNI
		NNIPage _nni;
		
		//Trigger
		TriggerPage _trigger;

		//RGB
		RGBPage _rgb;

		//GUI
		const size_t _maxPages = 3;
		const size_t _guiWidth = 300;
		int _page;
		ofxDatGui* _gMIDIIn;
		ofxDatGui* _gMIDIOut;
		vector<ofColor> _colorPallete;
		ofTrueTypeFont verdana;

		//IO
		ofJson _settings;
		string _folder, _file;

		//MIDI
		map<string, ofxMidiIn> _MIDIInputs;
		vector<string> _MIDIInPorts, _MIDIOutPorts;
		map<string, ofxMidiOut> _MIDIOutputs;
		vector<ofxMidiMessage> _MIDIMessages;
		size_t _maxMidiMessages;
};
