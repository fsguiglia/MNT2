#pragma once
#include "ofMain.h"
#include "ofxDatGui.h"

class NoiseGenerator {
public:
	NoiseGenerator();
	void setup(int w, int h, int guiWidth);
	void setupGui();
	void update();
	void draw(ofTrueTypeFont font);

	void resize(int w, int h);
	void setColorPallete(vector<ofColor> colorPalette);
	void setVisible(bool visible);

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
	void OSCIn(string address, float value);
	map<string, float> getOscOut();
	void clearMessages();
	void clearMappings();

	void load(ofJson& json);
	ofJson save();

private:
	ofRectangle centerSquarePosition(int w, int h);
	void generate();

	ofVec2f _cursor, _prevCursor;
	float _seed;
	float _xFrame;
	float _yFrame;

	vector<ofColor> _colorPallete;
	ofxDatGui* _gui;
	ofRectangle _position;
	float _xSpeed, _ySpeed;
	float _minX, _maxX, _minY, _maxY;
	int _guiWidth;
	bool _visible, _active;

	map<string, string> _midiMap;
	string _lastControl;
	bool _learn;

	map<string, float> _oscOutput;
};
