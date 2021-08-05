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
	void draw(ofTrueTypeFont font);
	
	void resize(int w, int h);
	void setColorPallete(vector<ofColor> colorPalette);
	void setVisible(bool visible);
	
	void record();
	void startRecording();
	void endRecording();
	
	void play();
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

	void MIDIIn(string port, int control, int channel, float value);
	map<string, float> getMidiOut();
	map<string, float> getMidiDump();
	void clearMIDIMessages();
	void OSCIn(string address, float value);

	void load(ofJson& json);
	ofJson save();

private:
	ofRectangle centerSquarePosition(int w, int h);

	map<string, Gesture> _gestures;
	int _index;

	Gesture _curGesture;
	string _curGestureName;
	bool _recording;
	
	Gesture _playGesture;
	ofPolyline _playPoly;
	int _playGestureIndex;
	int _lastPointTime;
	bool _playing;
	
	
	ofxDatGui* _gui;
	ofxDatGuiScrollView* _scrollView;
	vector<ofColor> _colorPallete;
	ofRectangle _position;
	int _guiWidth;
	bool _inside, _visible, _mouseControl, _learn;

	map<string, string> _midiMap;
	string _lastControl;

	ofVec2f _cursor, _prevCursor;
};
