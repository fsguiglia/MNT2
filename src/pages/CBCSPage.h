#pragma once
#include "ofMain.h"
#include "mapPage.h"
#include "../ML/pythonML.h"
#include "../maps/CBCS.h"

class CBCSPage : public MapPage<CBCS>
{
public:
	CBCSPage();
	void setup(string name, int w, int h, int guiWidth, ofTrueTypeFont font, int maxMessages = 50);
	void setupGui();
	void setupTsne();

	void update();

	void buttonEvent(ofxDatGuiButtonEvent e);
	void sliderEvent(ofxDatGuiSliderEvent e);
	void toggleEvent(ofxDatGuiToggleEvent e);
	void textInputEvent(ofxDatGuiTextInputEvent e);

	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button, bool doubleClick = false);
	void mouseReleased(int x, int y, int button);
	void mouseScrolled(int scroll);

	void moduleOSCIn(string address, float value);

	void exportFileList();
	void load(ofJson& json);
	void loadData(ofJson& json);
	ofJson save();
private:
	PythonML _dr;
};
