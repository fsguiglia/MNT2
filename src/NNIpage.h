#ifndef _NNIPAGE
#define _NNIPAGE
#include "ofMain.h"
#include "page.h"
#include "NNI.h"

class NNIPage : public PageBase<NNI> {
public:
	NNIPage();
	void setup(int width, int height, int guiWidth, int maxMessages = 1);
	void setupGui();
	
	void sliderEvent(ofxDatGuiSliderEvent e);
	void toggleEvent(ofxDatGuiToggleEvent e);
	void updateSelected(int selected, Point point);

	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseScrolled(int scroll);

	void MIDIIn(string port, int control, int channel, float value);
	
	void load(ofJson& json);
	ofJson save();
};
#endif
