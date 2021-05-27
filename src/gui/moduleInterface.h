#ifndef _MODULEINTERFACE
#define _MODULEINTERFACE
#include "ofMain.h"
#include "node.h"

class ModuleInterface : public Node {
public:
	virtual void setupPage(int w, int h, int guiWidth, vector<ofColor> colorPalette) = 0;
	virtual void update() = 0;
	virtual void drawPage(ofTrueTypeFont& font) = 0;

	virtual void mouseMoved(int x, int y) = 0;
	virtual void mouseDragged(int x, int y, int button) = 0;
	virtual void mousePressed(int x, int y, int button) = 0;
	virtual void mouseReleased(int x, int y, int button) = 0;
	virtual void mouseScrolled(int scroll) = 0;

	virtual void setVisible(bool visible) = 0;
	virtual bool getVisible() = 0;
};
#endif