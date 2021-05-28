#ifndef _TRIGGERPAGE
#define _TRIGGERPAGE
#include "ofMain.h"
#include "basePage.h"
#include "../maps/triggerMap.h"

class TriggerPage : public BasePage<TriggerMap> {
public:
	TriggerPage();
	void setup(int width, int height, int guiWidth, int maxMessages = 20);
	void setupGui();

	void sliderEvent(ofxDatGuiSliderEvent e);
	void toggleEvent(ofxDatGuiToggleEvent e);
	void updateSelected(int selected, Trigger trigger);

	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseScrolled(int scroll);

	void MIDIIn(string port, int channel, int control, float value);

	void load(ofJson& json);
	ofJson save();

private:
	float _radius, _threshold;
};

#endif