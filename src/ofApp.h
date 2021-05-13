#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "ofxXmlSettings.h"
#include "ofxNetwork.h"
#include "ofxOsc.h"
#include "ofxMidi.h"
#include "NNI.h"
#include "mntUtils.h"
#include "scrollGui.h"

/*
conflictos con MIDI: cuando interpola mandar solo si cambiaron los valores,
quilombo cuando varios puertos le dan al mismo cc

deberia hacer una subclase que se llame page? -> puedo primero escribir trigger y ahi veo

en nni los parametros se podrian mapear todos a partir de un diccionario, no los agrego con learn
sino con un + y después los mapeo
*/

class ofApp : public ofBaseApp, public ofxMidiListener {

	public:
		void setup();
		void update();
		void updateGuis();
		void draw();
		void exit();
		//--------------------------------------------------------------
		void setupNNI();
		void setupNNIGui(map<string, float> parameters, bool toggleState);
		void NNIToggle(ofxDatGuiToggleEvent e);
		void NNISlider(ofxDatGuiSliderEvent e);
		void updateNNISite(int selected, map<string, float> parameters);
		void drawNNI();
		void NNIMIDIIn(ofxMidiMessage& msg);
		void NNIMIDIOut(map<string, float> weights);
		//--------------------------------------------------------------
		void setupMIDI();
		void setupMIDIGui();
		void MIDIInToggle(ofxDatGuiToggleEvent e);
		void MIDIOutToggle(ofxDatGuiToggleEvent e);
		void newMidiMessage(ofxMidiMessage& msg);
		void sendMIDICC(map<string, float> parameters, ofxMidiOut port);
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
		NNI _nni;
		bool _nniMouseControl, _nniInside;
		string _nniCCXY[2];
		string _nniLastSelected;
		ofRectangle _nniPosition;
		bool _nniControlLearn, _nniParameterLearn;

		//GUI
		const size_t _maxPages = 1;
		const size_t _guiWidth = 300;
		int _page;
		ScrollGui* _gNNI;
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
