#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "ofxXmlSettings.h"
#include "ofxNetwork.h"
#include "ofxOsc.h"
#include "ofxMidi.h"
#include "nodes/node.h"
#include "nodes/moduleInterface.h"
#include "nodes/moduleNode.h"
#include "nodes/connection.h"
#include "pages/NNIpage.h"
#include "pages/triggerPage.h"
#include "pages/rgbPage.h"
#include "gestures/gesturePage.h"
#include "generators/noiseGenerator.h"
#include "utils/mntUtils.h"

/*
en vez de map<string, float> podría hacer un struct controlchange, pero ojo que ofxmidiout ya tiene

osc va a tener dos problemas: por un lado el control esta todo pensado para midi (como se reflejan las 
direcciones en el control de parámetros en los mapas?) y por el otro, no preví una manera de reflejar
la ip de destino en los outputs

quiza osc tiene que ser un módulo en el que uno hace doble click y puede configurar las entradas y salidas?

hay un quilombo porque los mensajes midi llegan en otro thread. no puedo limpiar la lista una vez que los recibo.
pero si pongo un mutex, a veces dos mensajes llegan al mismo tiempo y no necesariamente se mantiene el orden de
llegada
*/

class ofApp : public ofBaseApp, public ofxMidiListener {

	public:
		void setup();
		void update();
		void draw();
		void drawConnection(Connection& connection);
		void exit();
		//--------------------------------------------------------------
		void setupColor();
		void setupGui();
		void buttonEvent(ofxDatGuiButtonEvent e);
		//--------------------------------------------------------------
		void setupMIDI();
		string removePortNumber(string name);
		void MIDIInToggle(ofxDatGuiToggleEvent e);
		void MIDIOutToggle(ofxDatGuiToggleEvent e);
		string createMIDIInput(string port, int x, int y);
		void deleteMIDIInput(string port);
		string createMIDIOutput(string port, int x, int y);
		void deleteMIDIOutput(string port);
		void newMidiMessage(ofxMidiMessage& msg);
		//--------------------------------------------------------------
		void setupOSC();
		void OSCTextInput(ofxDatGuiTextInputEvent e);
		void createOscInput(string port, int x, int y);
		void deleteOscInput(string port);
		void createOscOutput(string ip, string port, int x, int y);
		void deleteOscOutput(string ip, string port);
		//--------------------------------------------------------------
		tuple<string, int, int> selectNode(int x, int y);
		void createDeleteConnection(tuple<string, int, int> out, tuple<string, int, int> in, bool dump);
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
		vector<Node> _inputNodes;
		vector<Node> _outputNodes;
		vector<Connection> _connections;
		string _selected;
		int _lastClick = 0;
		tuple<string, int, int> _shiftSelected;
		bool _shift, _control;
		
		//GUI
		const size_t _maxPages = 1;
		const size_t _guiWidth = 300;
		int _lastWidth, _lastHeight;
		int _page;
		bool _mode;
		vector<ofColor> _colorPallete;
		ofTrueTypeFont _verdana;
		ofxDatGui* _gui;
		ofxDatGuiFolder* _midiInFolder;
		ofxDatGuiFolder* _midiOutFolder;
		ofxDatGuiFolder* _oscFolder;

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