#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "ofxXmlSettings.h"
#include "ofxNetwork.h"
#include "ofxOsc.h"
#include "ofxMidi.h"
#include "NNIPage.h"
#include "triggerMap.h"
#include "mntUtils.h"
#include "page.h"

/*
conflictos con MIDI: cuando interpola mandar solo si cambiaron los valores,
quilombo cuando varios puertos le dan al mismo cc
*/

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
		/*
		TriggerMap _trigger;
		bool _nniMouseControl, _nniInside;
		string _triggerLastSelected;
		string _nniCCXY[2];
		ofRectangle _nniPosition;
		bool _nniControlLearn, _nniParameterLearn;
		ScrollGui* _gTrigger;
		*/

		//GUI
		const size_t _maxPages = 2;
		const size_t _guiWidth = 300;
		int _page;
		ofxDatGui* _gMIDIIn;
		ofxDatGui* _gMIDIOut;
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
