#pragma once
#include "ofMain.h"

class Node
{
public:
	Node();
	void setup(float x, float y, int w, int h, int inputs, int outputs, string name, ofColor color);
	void setup(float x, float y, int w, int h);
	void setInputs(int inputs);
	void setOutputs(int outputs);
	void setColor(ofColor color);
	void draw(ofTrueTypeFont& font);

	void setSize(int w, int h);
	void setPosition(float x, float y);
	ofVec2f getPosition();
	int getWidth();
	int getHeight();
	int getInputs();
	int getOutputs();
	ofRectangle getInputConnector(int index);
	ofRectangle getOutputConnector(int index);
	void setName(string name, bool addId=false);
	string getName();
	void setId(int id);
	int getId();
	bool inside(int x, int y);
	void setAsInput(bool isInput);
	void setAsOutput(bool isOutput);
	bool isInput();
	bool isOutput();
	ofRectangle getBox();

private:
	bool _isInput, _isOutput;
	int _inputs, _outputs, _selectedIn, _selectedOut;
	ofRectangle _rect, _name_bb;
	vector<ofRectangle> _inPositions, _outPositions;
	ofColor _color;

	string _name;
	static int ID;
	int _id;
};