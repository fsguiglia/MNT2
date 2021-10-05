#pragma once
#include "ofMain.h"
#include "../pages/page.h"

class NoiseGenerator : public Page {
public:
	NoiseGenerator();
	void setup(string name, int w, int h, int guiWidth, int maxMessages = 20);
	void setupGui(string name);
	void update();
	void draw(ofTrueTypeFont font);

	void setColorPallete(vector<ofColor> colorPalette);

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
	void generate();

	ofVec2f _cursor, _prevCursor;
	float _seed;
	float _xFrame;
	float _yFrame;

	vector<ofColor> _colorPallete;
	float _xSpeed, _ySpeed;
	ofVec2f _center, _radius;
	bool _active;
};
