#pragma once
#include "ofMain.h"
#include "../gui/scrollGui.h"
#include "../utils/mntUtils.h"
#include "ofxDatGui.h"

class Page {
public:
	Page();

	virtual void setup(string name, int w, int h, int guiWidth, int maxMessages = 20) = 0;
	virtual void update() = 0;
	virtual void draw(ofTrueTypeFont font) = 0;
	void setHeader(string label);

	//gui
	ofVec2f getPosition();
	int getHeight();
	int getWidth();
	void resize(int w, int h);
	void setVisible(bool visible);
	virtual void setColorPallete(vector<ofColor> colorPallete) = 0;

	//gui events
	//virtual void buttonEvent(ofxDatGuiButtonEvent e) = 0;
	//virtual void toggleEvent(ofxDatGuiToggleEvent e) = 0;
	//virtual void sliderEvent(ofxDatGuiSliderEvent e) = 0;

	//mouse input
	virtual void mouseMoved(int x, int y) = 0;
	virtual void mouseDragged(int x, int y, int button) = 0;
	virtual void mousePressed(int x, int y, int button, bool doubleClick = false) = 0;
	virtual void mouseReleased(int x, int y, int button) = 0;
	virtual void mouseScrolled(int scroll) = 0;

	//io settings
	void setMidiOutput(bool midiOutput);
	void setOscOutput(bool oscOutput);
	void setStringOutput(bool stringOutput);
	bool getMidiOutput();
	bool getOscOutput();
	bool getStringOutput();

	//mapping
	void clearMappings();

	//io
	void MIDIIn(string port, int control, int channel, float value);
	map<string, float> getMidiOut(bool clear = false);
	map<string, float> getMidiDump(bool clear = false);

	void OSCIn(string address, float value);
	map<string, float> getOscOut(bool clear = false);
	void setAddress(string address);
	string getAddress();
	vector<string> getStringOut(bool clear = false);
	void clearMessages();
	void clearMidiMap();

	//load/save
	virtual void load(ofJson& json) = 0;
	virtual ofJson save() = 0;

protected:
	virtual void moduleMIDIIn(string port, int control, int channel, float value) = 0;
	virtual void moduleMIDIMap(string port, int control, int channel, float value) = 0;
	virtual void moduleOSCIn(string address, float value) = 0;

	void saveMidiMap(ofJson& json);
	void loadMidiMap(ofJson& json);
	
	void addMessages(map<string, float> messages, map<string, float>& queue);
	void setStringMessages(vector<string> messages);
	void clearMessages(map<string, float>& queue);

	ofRectangle centerSquarePosition(int w, int h);

	//gui
	ScrollGui* _gui;
	ofRectangle _position;
	int _guiWidth;
	bool _visible, _inside;
	bool _controlLearn, _parameterLearn; //parameterlearn no tiene sentido para los generadores

	//mapping
	string _lastControl;
	map<string, string> _midiMap;

	//io
	int _maxMessages;
	bool _midiOutput, _oscOutput, _stringOutput;
	string _address;
	map<string, float> _previousOutput, _OSCOutMessages, _MIDIOutMessages, _MIDIDumpMessages;
	vector<string> _stringMessages;
};