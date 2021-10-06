#pragma once
#include "ofMain.h"
#include "node.h"

class ModuleInterface : public Node {
public:
	virtual void setupPage(string name, int w, int h, int guiWidth, ofTrueTypeFont font, vector<ofColor> colorPalette) = 0;
	virtual void setPageHeader(string label) = 0;
	virtual void update() = 0;
	virtual void drawPage() = 0;
	virtual void drawTile(int x, int y, int w, int h, int margin) = 0;
	virtual void resizePage(int w, int h) = 0;

	virtual ofVec2f getPagePosition() = 0;
	virtual int getPageHeight() = 0;
	virtual int getPageWidth() = 0;

	virtual void mouseMoved(int x, int y) = 0;
	virtual void mouseDragged(int x, int y, int button) = 0;
	virtual void mousePressed(int x, int y, int button, bool doubleClick = false) = 0;
	virtual void mouseReleased(int x, int y, int button) = 0;
	virtual void mouseScrolled(int scroll) = 0;

	virtual bool getMidiOutput() = 0;
	virtual bool getOscOutput() = 0;
	virtual bool getStringOutput() = 0;

	virtual void MIDIIn(string port, int control, int channel, float value) = 0;
	virtual void OSCIn(string address, float value) = 0;
	virtual map<string, float> getOSCOut() = 0;
	virtual map<string, float> getMidiOut() = 0;
	virtual map<string, float> getMidiDump() = 0;
	virtual string getAddress() = 0;
	virtual vector<string> getStringOut() = 0;
	virtual void clearMessages() = 0;

	virtual void setVisible(bool visible) = 0;
	virtual bool getVisible() = 0;

	virtual void load(ofJson& json) = 0;
	virtual ofJson save() = 0;
};