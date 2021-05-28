#ifndef _SCROLLGUI
#define _SCROLLGUI
#include "ofMain.h"
#include "ofxDatGui.h"
class ScrollGui : public ofxDatGui
{
public:
	void setMaxHeight(int height);
	void scroll(int direction);
	void setRemovableSlider(string label);
	string inside(int x, int y);
	void removeSlider(string label);
	void updatePositions();
	void clearRemovableSliders();
private:
	int _maxHeight;
	map<string, ofRectangle> _removableSliders;
};
#endif
