#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "ofxXmlSettings.h"
#include "ofxNetwork.h"
#include "ofxOsc.h"
#include "ofxMidi.h"
#include "gui/node.h"
#include "gui/connection.h"
#include "pages/NNIpage.h"
#include "pages/triggerPage.h"
#include "pages/rgbPage.h"
#include "utils/mntUtils.h"
#include "test.h"

/*
No logro hacer un vector con los distintos tipos de nodo: vector de unique_ptr no funciona, 
of no tiene std::variant ni std::any
*/

class ofApp : public ofBaseApp, public ofxMidiListener {

	public:
		void setup();
		void update();
		void draw();
		void drawConnection(Connection& connection);
		void exit();
		//--------------------------------------------------------------
		void setupGui();
		void buttonEvent(ofxDatGuiButtonEvent e);
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

		//Nodes
		vector<unique_ptr<NodeInterface>> _nodes;
		vector<Connection> _connections;
		string _selected;
		int _lastClick = 0;
		string _shiftSelected;
		bool _shift;
		int _id;
		
		//GUI
		const size_t _maxPages = 1;
		const size_t _guiWidth = 300;
		int _page;
		bool _mode;
		vector<ofColor> _colorPallete;
		ofTrueTypeFont _verdana;
		ofxDatGui* _gui;

		//IO
		ofJson _settings;
		string _folder, _file;

		//MIDI
		map<string, ofxMidiIn> _MIDIInputs;
		vector<string> _MIDIInPorts, _MIDIOutPorts;
		map<string, ofxMidiOut> _MIDIOutputs;
		vector<ofxMidiMessage> _MIDIMessages;
		size_t _maxMidiMessages;
		ofxDatGui* _gMIDIIn;
		ofxDatGui* _gMIDIOut;

};