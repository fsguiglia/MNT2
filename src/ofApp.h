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
en nni los parametros se podrian mapear todos a partir de un diccionario, no los agrego con learn
sino con un + y después los mapeo
*/

class ofApp : public ofBaseApp, public ofxMidiListener {

	public:
		void setup();
		void update();
		void updateGuis();
		void draw();
		void drawPosition(ofVec2f position, ofRectangle rect);
		void exit();
		//--------------------------------------------------------------
		void setupNNI();
		void setupNNIGui(map<string, float> parameters, bool toggleState);
		void NNIToggle(ofxDatGuiToggleEvent e);
		void NNISlider(ofxDatGuiSliderEvent e);
		void NNIMIDI(ofxMidiMessage& msg);
		void addNNISite(float x, float y, int id);
		void selectNNISite(float x, float y);
		void drawNNI();
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
		int _selNNISite, _nniDrag;
		bool _nniInterpolate, _nniMouseControl, _nniRandom, _nniInside;
		map<string, float> _nniWeights;
		string _nniCCXY[2];
		string _nniLastSelected;
		ofVec2f _nniCursor;
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
