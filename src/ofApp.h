#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "ofxOsc.h"
#include "ofxMidi.h"
#include "nodes/mntNode.h"
#include "nodes/moduleInterface.h"
#include "nodes/moduleNode.h"
#include "nodes/connection.h"
#include "pages/NNIpage.h"
#include "pages/CBCSPage.h"
#include "pages/triggerPage.h"
#include "pages/rgbPage.h"
#include "pages/gesturePage.h"
#include "pages/noiseGenerator.h"
#include "utils/mntUtils.h"

class ofApp : public ofBaseApp, public ofxMidiListener {
	public:
		void setup();
		void update();
		void draw();
		void drawIntro();
		void drawConnectionMode();
		void drawEditMode();
		void drawTileMode();
		void drawCurConnection();
		void drawConnection(Connection& connection);
		void exit();
		//--------------------------------------------------------------
		void setupColor();
		void setupGui();
		void moduleButtonEvent(ofxDatGuiButtonEvent e);
		void buttonEvent(ofxDatGuiButtonEvent e);
		//--------------------------------------------------------------
		void changePage(int page);
		//--------------------------------------------------------------
		int createModule(string type, float x, float y);
		int createModule(string type, float x, float y, int id);
		int setupModule(string type, float x, float y);
		void setupModulePage(int index);
		//--------------------------------------------------------------
		void setupMIDI();
		string removePortNumber(string name);
		void MIDIInToggle(ofxDatGuiToggleEvent e);
		void MIDIOutToggle(ofxDatGuiToggleEvent e);
		string createMIDIInput(string port, float x, float y);
		void deleteMIDIInput(string port);
		string createMIDIOutput(string port, float x, float y);
		void deleteMIDIOutput(string port);
		void newMidiMessage(ofxMidiMessage& msg);
		//--------------------------------------------------------------
		void setupOSC();
		void OSCTextInput(ofxDatGuiTextInputEvent e);
		void createOscInput(string port, float x, float y);
		void deleteOscInput(string port);
		void createOscOutput(string ip, string port, float x, float y);
		void deleteOscOutput(string ip, string port);
		//--------------------------------------------------------------
		tuple<string, int, int, ofVec2f, int> selectNode(int x, int y);
		void createDeleteConnection(tuple<string, int, int, ofVec2f, int> out, tuple<string, int, int, ofVec2f, int> in, bool dump);
		void updateConnections();
		//--------------------------------------------------------------
		void clear();
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
		vector<unique_ptr<ModuleInterface>> _moduleNodes;
		vector<MNTNode> _inputNodes;
		vector<MNTNode> _outputNodes;
		vector<Connection> _connections;
		string _selected = ""; 
		ofVec2f _selectionOffset;
		int _lastClick = 0;
		tuple<string, int, int, ofVec2f, int> _shiftSelected;
		bool _control, _shift;
		bool _connectorSelected;
		
		//GUI
		const size_t _maxPages = 1;
		const int _moduleSize = 1024;
		int _moduleNodeHeight = 30;
		int _guiWidth = 300;
		int _lastWidth, _lastHeight;
		int _pageMarginLeft, _pageMarginRight;
		int _page;
		vector<ofColor> _colorPallete;
		ofTrueTypeFont _verdana;
		ofxDatGui* _gui;
		ofxDatGuiFolder* _moduleFolder;
		ofxDatGuiFolder* _midiInFolder;
		ofxDatGuiFolder* _midiOutFolder;
		ofxDatGuiFolder* _oscFolder;
		ofColor _moduleColor, _generatorColor, _ioColor;
		float _scale;
		bool _prevNextButton;
		vector<ofImage> _introSequence;
		bool _intro;

		//tile mode
		ofImage _tileIcon;
		const int _tileIconMargin = 20;
		bool _tileIconHovered = false;
		const int COLUMNS = 3;

		//Modes
		int _mode = 0;
		static constexpr int CONNECT_MODE = 0;
		static constexpr int EDIT_MODE = 1;
		static constexpr int TILE_MODE = 2;

		//IO
		string _folder, _file;

		//MIDI
		map<string, ofxMidiIn> _MIDIInputs;
		map<string, string> _MIDIInPorts, _MIDIOutPorts;
		map<string, ofxMidiOut> _MIDIOutputs;
		vector<ofxMidiMessage> _MIDIMessages;
		size_t _maxMidiMessages;
		ofMutex midiMutex;

		//OSC
		map<string, ofxOscReceiver> _oscReceivers;
		map<string, ofxOscSender> _oscSenders;
		vector<ofxOscMessage> _oscMessages;
		size_t _maxOscMessages;
};