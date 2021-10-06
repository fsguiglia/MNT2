#pragma once
#include "ofMain.h"
#include "page.h"
#include "../gestures/gesture.h"
#include "../ML/pythonML.h"

class GesturePage : public Page {
public:
	GesturePage();
	void setup(string name, int w, int h, int guiWidth, ofTrueTypeFont font, int maxMessages = 20);
	void setupGui(string name);
	void setupLSTM();

	void update();
	void updateGui();
	void draw();
	void drawTile(int x, int y, int w, int h, int margin);

	void setColorPallete(vector<ofColor> colorPalette);
	void resize(int w, int h);
	
	void record();
	void startRecording();
	void endRecording();
	
	void play();
	void next();
	void previous();
	void random();
	void startPlaying();

	void getCursorAtPercent(float position);

	void scrollViewEvent(ofxDatGuiScrollViewEvent e);
	void buttonEvent(ofxDatGuiButtonEvent e);
	void toggleEvent(ofxDatGuiToggleEvent e);
	void sliderEvent(ofxDatGuiSliderEvent e);

	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button, bool doubleClick = false);
	void mouseReleased(int x, int y, int button);
	void mouseScrolled(int scroll);

	void moduleMIDIIn(string port, int control, int channel, float value);
	void moduleMIDIMap(string port, int control, int channel, float value);
	void moduleOSCIn(string address, float value);

	void load(ofJson& json);
	ofJson save();

private:
	void drawGestures(int x, int y, int w, int h);
	void selectGesture(int index);
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
	bool _mouseControl;
	
	Gesture _playGesture;
	ofPolyline _playPoly, _scrubPoly;
	float _scrubPolySpacing;
	int _playGestureIndex;
	int _lastPointTime;
	bool _playing;

	int _guiHeight;
	ofxDatGuiFolder* _transportFolder;
	ofxDatGuiFolder* _controlFolder;
	ofxDatGuiFolder* _generateFolder;
	ofxDatGuiScrollView* _scrollView;
	vector<ofColor> _colorPallete;

	ofVec2f _cursor, _prevCursor;

	PythonML _lstmTrain, _lstmGen;
};
