#ifndef _TRIGGERMAP
#define _TRIGGERMAP
#include "ofMain.h"
#include "page.h"
#include "NNI.h"
#include "ofxDatGui.h"
/*
probar midi!!!
load no carga los valores
*/
class NNIPage : public PageBase<NNI> {
public:
	NNIPage();
	void setup(int width, int height, int guiWidth, int maxMessages = 1);
	void setupGui(map<string, float> parameters, bool toggleState);
	void update();
	void setVisible(bool visible);
	void draw(ofTrueTypeFont& font);
	
	void sliderEvent(ofxDatGuiSliderEvent e);
	void toggleEvent(ofxDatGuiToggleEvent e);
	void updateNNISite(int selected, map<string, float> parameters);

	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseScrolled(int scroll);
	void resize(int w, int h);

	void MIDIIn(string port, int control, int channel, float value);
	
	void load(ofJson& json);
	ofJson save();

private:
	map<string, float> previousWeights;
};
#endif
