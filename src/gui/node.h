#ifndef _NODE
#define _NODE
#include "ofMain.h"

template<typename T> class Node {
public:
	Node();
	void setup(int x, int y, int w, int h, int inputs, int outputs);
	void setupPage(int w, int h, int guiWidth, vector<ofColor> colorPalette);
	void setTitle(string title);
	void update();
	void draw(ofTrueTypeFont& font);

	void setVisible(bool visible);
	bool getVisible();
	void setSize(int w, int h);
	void setPosition(int x, int y);
	int getInputs();
	int getOutputs();
	ofRectangle getInput(int index);
	ofRectangle getOutput(int index);
	int getId();
	bool inside(int x, int y);

	T _page;
private:

	bool _visible, _selected;
	int _inputs, _outputs, _selectedIn, _selectedOut;
	ofRectangle _rect;
	string _title;
	vector<ofRectangle> _inPositions, _outPositions;
	
	int _id;
	static int ID;

};
#endif

template<class T>
int Node<T>::ID = 0;

template<typename T>
inline Node<T>::Node()
{
	_page = new T();
	_visible = false;
	_selected = false;
	_selectedIn = -1;
	_selectedOut = -1;
	_id = ID++;
}

template<typename T>
inline void Node<T>::setup(int x, int y, int w, int h, int inputs, int outputs)
{
	_rect.setPosition(x, y);
	_rect.setWidth(w);
	_rect.setHeight(h);
	_inputs = inputs;
	_outputs = outputs;
	 
	for (int i = 0; i < inputs; i++)
	{
		float x = 0;
		float y = float(i + 1) / (inputs + 1);
		float r = 0.1;
		_inPositions.push_back(ofRectangle(x, y, r, r));
	}
	for (int i = 0; i < outputs; i++)
	{
		float x = 0.95;
		float y = float(i + 1) / (outputs + 1);
		float r = 0.1;
		_outPositions.push_back(ofRectangle(x, y, r, r));
	}
}

template<typename T>
inline void Node<T>::setupPage(int w, int h, int guiWidth, vector<ofColor> colorPalette)
{
	_page.setup(w, h, guiWidth);
	_page.setMapColorPallete(colorPalette);
	_page.setVisible(false);
}

template<typename T>
inline void Node<T>::setTitle(string title)
{
	_title = title;
}

template<typename T>
inline void Node<T>::update()
{
	_page.setVisible(_visible);
	_page.update();
}

template<typename T>
inline void Node<T>::draw(ofTrueTypeFont& font)
{
	ofSetColor(80);
	ofDrawRectangle(_rect);
	ofRectangle bb = font.getStringBoundingBox(_title, 0, 0);
	ofSetColor(255);
	font.drawString(_title, _rect.x + (_rect.width - bb.width) * 0.5, _rect.y + bb.height + (_rect.height - bb.height) * 0.5);
	for (int i = 0; i < _inputs; i++)
	{
		if (i == _selectedIn) ofSetColor(150, 0, 0);
		else ofSetColor(150);
		ofDrawCircle(_rect.x, _rect.y + _inPositions[i].y * _rect.height, _rect.height * _inPositions[i].width);
	}
	for (int i = 0; i < _outputs; i++)
	{
		if (i == _selectedOut) ofSetColor(150, 0, 0);
		else ofSetColor(150);
		ofDrawCircle(_rect.x + _rect.getWidth(), _rect.y + _outPositions[i].y * _rect.height, _rect.height * _outPositions[i].width);
	}
}

template<typename T>
inline void Node<T>::setVisible(bool visible)
{
	_visible = visible;
}

template<typename T>
inline bool Node<T>::getVisible()
{
	return _visible;
}

template<typename T>
inline void Node<T>::setSize(int w, int h)
{
	_rect.setWidth(w);
	_rect.setHeight(h);
}

template<typename T>
inline void Node<T>::setPosition(int x, int y)
{
	_rect.setPosition(x, y);
}

template<typename T>
inline int Node<T>::getInputs()
{
	return _inputs;
}

template<typename T>
inline int Node<T>::getOutputs()
{
	return _outputs;
}

template<typename T>
inline ofRectangle Node<T>::getInput(int index)
{
	ofRectangle connector = _inPositions[index];
	connector.x *= _rect.width;
	connector.x += _rect.x;
	connector.y *= _rect.height;
	connector.y += _rect.y;
	connector.width *= _rect.width;
	connector.height *= _rect.height;
	return connector;
}

template<typename T>
inline ofRectangle Node<T>::getOutput(int index)
{
	ofRectangle connector = _outPositions[index];
	connector.x *= _rect.width;
	connector.x += _rect.x;
	connector.y *= _rect.height;
	connector.y += _rect.y;
	connector.width *= _rect.width;
	connector.height *= _rect.height;
	return connector;
}

template<typename T>
inline int Node<T>::getId()
{
	return _id;
}

template<typename T>
inline bool Node<T>::inside(int x, int y)
{
	bool in = _rect.inside(x, y);
	return in;
}
