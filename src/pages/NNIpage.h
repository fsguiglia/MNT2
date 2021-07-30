#ifndef _NNIPAGE
#define _NNIPAGE
#include "ofMain.h"
#include "basePage.h"
#include "../ML/dimensionalityReduction.h"
#include "../maps/NNI.h"

class NNIPage : public BasePage<NNI> {
public:
	NNIPage();
	void setup(int width, int height, int guiWidth, int maxMessages = 20);
	void setupGui();
	void setupTsne();
	void setupPca();

	void update();

	void buttonEvent(ofxDatGuiButtonEvent e);
	void sliderEvent(ofxDatGuiSliderEvent e);
	void toggleEvent(ofxDatGuiToggleEvent e);
	void textInputEvent(ofxDatGuiTextInputEvent e);
	void updateSelected(int selected, Point point);

	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button, bool doubleClick = false);
	void mouseReleased(int x, int y, int button);
	void mouseScrolled(int scroll);
	
	void load(ofJson& json);
	ofJson save();

private:
		DimensionalityReduction _tsne;
		DimensionalityReduction _pca;
};
#endif
