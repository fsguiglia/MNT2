#ifndef _NODE
#define _NODE
#include "ofMain.h"
#include "nodeInterface.h"

template<typename T> class Node : public NodeInterface {
public:
	Node();
	~Node();
	void setup(int x, int y, int w, int h, int inputs, int outputs);
	void setupPage(int w, int h, int guiWidth, vector<ofColor> colorPalette);
	void update();
	void draw(ofTrueTypeFont& font);
	void drawPage(ofTrueTypeFont& font);

	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseScrolled(int scroll);

	void setVisible(bool visible);
	bool getVisible();
	void setSize(int w, int h);
	void setPosition(int x, int y);
	int getInputs();
	int getOutputs();
	ofRectangle getInput(int index);
	ofRectangle getOutput(int index);
	void setName(string name);
	string getName();
	int getId();
	bool inside(int x, int y);

	
private:
	T* _page;
	bool _visible, _selected;
	int _inputs, _outputs, _selectedIn, _selectedOut;
	ofRectangle _rect;
	vector<ofRectangle> _inPositions, _outPositions;
	
	string _name;
	int _id;

};
#endif

template<typename T>
inline Node<T>::Node()
{
	_page = new T();
	_visible = false;
	_selected = false;
	_selectedIn = -1;
	_selectedOut = -1;
}

template<typename T>
inline Node<T>::~Node()
{
	delete _page;
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
	_page->setup(w, h, guiWidth);
	_page->setMapColorPallete(colorPalette);
	_page->setVisible(false);
}

template<typename T>
inline void Node<T>::setName(string name)
{
	_name = name;
}

template<typename T>
inline string Node<T>::getName()
{
	return _name;
}

template<typename T>
inline void Node<T>::update()
{
	_page->setVisible(_visible);
	_page->update();
}

template<typename T>
inline void Node<T>::draw(ofTrueTypeFont& font)
{
	ofSetColor(80);
	ofDrawRectangle(_rect);
	ofRectangle bb = font.getStringBoundingBox(_name, 0, 0);
	ofSetColor(255);
	font.drawString(_name, _rect.x + (_rect.width - bb.width) * 0.5, _rect.y + bb.height + (_rect.height - bb.height) * 0.5);
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
inline void Node<T>::drawPage(ofTrueTypeFont & font)
{
	_page->draw(font);
}

template<typename T>
inline void Node<T>::mouseMoved(int x, int y)
{
	_page->mouseMoved(x, y);
}

template<typename T>
inline void Node<T>::mouseDragged(int x, int y, int button)
{
	_page->mouseDragged(x, y, button);
}

template<typename T>
inline void Node<T>::mousePressed(int x, int y, int button)
{
	_page->mousePressed(x, y, button);
}

template<typename T>
inline void Node<T>::mouseReleased(int x, int y, int button)
{
	_page->mouseReleased(x, y, button);
}

template<typename T>
inline void Node<T>::mouseScrolled(int scroll)
{
	_page->mouseScrolled(scroll);
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
