#ifndef _RGBPAGES
#define _RGBPAGES
#include "ofMain.h"
#include "basePage.h"
#include "../maps/RGBMap.h"

class RGBPage : public BasePage<RGBMap> {
public:
	RGBPage();
	void setup(int width, int height, int guiWidth, int maxMessages = 20);
	void setupGui();

	void sliderEvent(ofxDatGuiSliderEvent e);
	void toggleEvent(ofxDatGuiToggleEvent e);
	void updateSelected(int selected, RGBPoint point);

	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button, bool doubleClick = false);
	void mouseReleased(int x, int y, int button);
	void mouseScrolled(int scroll);

	void MIDIIn(string port, int channel, int control, float value);

	void load(ofJson& json);
	ofJson save();

private:
	float _radius;
};
#endif
