#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "ofxXmlSettings.h"
#include "ofxNetwork.h"
#include "ofxOsc.h"
#include "ofxMidi.h"
#include "NNI.h"

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
		void exit();

		ofRectangle centerMapPosition(int w, int h);
		ofVec2f normalizeMapCursor(int x, int y, ofRectangle mapPosition);

		void setupNNI();
		void setupNNIGui(map<string, float> parameters, bool toggleState);
		void addNNISite(float x, float y, int id);
		void selectNNISite(float x, float y);
		void NNIToggle(ofxDatGuiToggleEvent e);
		void NNISlider(ofxDatGuiSliderEvent e);
		void NNIMIDI(ofxMidiMessage& msg);
		void drawNNI();

		void setupMIDI();
		void drawMIDI();
		void MIDIInToggle(ofxDatGuiToggleEvent e);
		void MIDIOutToggle(ofxDatGuiToggleEvent e);
		void newMidiMessage(ofxMidiMessage& msg);
		void sendMIDICC(map<string, float> parameters, ofxMidiOut port);

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		

		//NNI
		NNI _nni;
		int _selNNISite;
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
		ofxDatGui* _gNNI;
		ofxDatGuiScrollView* _gNNIParameters;
		ofxDatGui* _gMIDIIn;
		ofxDatGui* _gMIDIOut;

		//MIDI
		map<string, ofxMidiIn> _MIDIInputs;
		map<string, ofxMidiOut> _MIDIOutputs;
		vector<ofxMidiMessage> _MIDIMessages;
		size_t _maxMidiMessages;
};
