#pragma once
#include "ofMain.h"
#include "basePage.h"
#include "../ML/pythonML.h"
#include "../maps/CBCS.h"

class CBCSPage : public BasePage<CBCS>
{
public:
	CBCSPage();
	void setup(int width, int height, int guiWidth, int maxMessages = 50);
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

	void exportFileList();
	void load(ofJson& json);
	ofJson save();
private:
	PythonML _dr;
};
