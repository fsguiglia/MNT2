#ifndef _NODEINTERFACE
#define _NODEINTERFACE
#include "ofMain.h"

class NodeInterface
{
public:
	NodeInterface();
	virtual void setup(int x, int y, int w, int h, int inputs, int outputs) = 0;
	virtual void setupPage(int w, int h, int guiWidth, vector<ofColor> colorPalette) = 0;
	virtual void update() = 0;
	virtual void draw(ofTrueTypeFont& font) = 0;
	virtual void drawPage(ofTrueTypeFont& font) = 0;

	virtual void mouseMoved(int x, int y) = 0;
	virtual void mouseDragged(int x, int y, int button) = 0;
	virtual void mousePressed(int x, int y, int button) = 0;
	virtual void mouseReleased(int x, int y, int button) = 0;
	virtual void mouseScrolled(int scroll) = 0;

	virtual void setVisible(bool visible) = 0;
	virtual bool getVisible() = 0;
	virtual void setSize(int w, int h) = 0;
	virtual void setPosition(int x, int y) = 0;
	virtual int getInputs() = 0;
	virtual int getOutputs() = 0;
	virtual ofRectangle getInput(int index) = 0;
	virtual ofRectangle getOutput(int index) = 0;
	virtual void setName(string name) = 0;
	virtual string getName() = 0;
	virtual int getId() = 0;
	virtual bool inside(int x, int y) = 0;
};

#endif