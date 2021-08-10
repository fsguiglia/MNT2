#pragma once
#include "ofMain.h"
#include "gesture.h"
#include "ofxDatGui.h"
#include "../utils/mntUtils.h"

class GesturePage {
public:
	GesturePage();
	void setup(int w, int h, int guiWidth);
	void setupGui();
	void update();
	void updateGui();
	void draw(ofTrueTypeFont font);
	
	void resize(int w, int h);
	void setColorPallete(vector<ofColor> colorPalette);
	void setVisible(bool visible);
	
	void record();
	void startRecording();
	void endRecording();
	
	void play();
	void playNext();
	void playPrev();
	void playRandom();
	void startPlaying();

	void scrollViewEvent(ofxDatGuiScrollViewEvent e);
	void buttonEvent(ofxDatGuiButtonEvent e);
	void toggleEvent(ofxDatGuiToggleEvent e);
	void sliderEvent(ofxDatGuiSliderEvent e);

	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button, bool doubleClick = false);
	void mouseReleased(int x, int y, int button);
	void mouseScrolled(int scroll);

	void setMidiOutput(bool midiOutput);
	void setOscOutput(bool oscOutput);
	void setStringOutput(bool stringOutput);
	bool getMidiOutput();
	bool getOscOutput();
	bool getStringOutput();

	void MIDIIn(string port, int control, int channel, float value);
	map<string, float> getMidiOut();
	map<string, float> getMidiDump();
	void OSCIn(string address, float value);
	map<string, float> getOscOut();
	string getAddress();
	vector<string> getStringOut();
	void clearMessages();
	void clearMappings();

	void load(ofJson& json);
	ofJson save();

private:
	void addGesture(Gesture gesture, string name);
	void removeGesture(string name);
	ofRectangle centerSquarePosition(int w, int h);

	map<string, Gesture> _gestures;
	vector<string> _gestureNames;
	int _index;

	Gesture _curGesture;
	string _curGestureName;
	int _curGestureIndex;
	bool _recording;
	
	Gesture _playGesture;
	ofPolyline _playPoly;
	int _playGestureIndex;
	int _lastPointTime;
	bool _playing;
	
	
	ofxDatGui* _gui;
	ofxDatGuiFolder* _transportFolder;
	ofxDatGuiFolder* _controlFolder;
	ofxDatGuiFolder* _generateFolder;
	ofxDatGuiScrollView* _scrollView;
	vector<ofColor> _colorPallete;
	ofRectangle _position;
	int _guiWidth, _guiHeight;
	bool _inside, _visible, _mouseControl, _learn;

	map<string, float> _output;
	map<string, string> _midiMap;
	string _lastControl;
	bool _midiOutput, _oscOutput, _stringOutput;

	ofVec2f _cursor, _prevCursor;
};
