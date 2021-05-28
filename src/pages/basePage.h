#pragma once
#ifndef _BASEPAGE
#define _BASEPAGE
#include "ofMain.h"
#include "../gui/scrollGui.h"
#include "../utils/mntUtils.h"
#include "ofxDatGui.h"

template<typename T> class BasePage {
public:
	BasePage();

	virtual void setup(int width, int height, int guiWidth, int maxMessages = 20) = 0;
	void setMapColorPallete(vector<ofColor> colorPallete);
	void update();
	void draw(ofTrueTypeFont font);

	void resize(int w, int h);
	ofRectangle centerSquarePosition(int w, int h);

	map<string, float> getMidiDump(bool clear = false);
	map<string, float> getMidiOut(bool clear = false);
	void setVisible(bool visible);
	void clearMIDIMessages();

protected:
	void addMidiMessages(map<string, float> messages, map<string, float>& queue);
	void clearMIDIMessages(map<string, float>& queue);

	T _map;
	ScrollGui* _gui;
	ofxDatGuiFolder* _controlFolder;
	bool _mouseControl, _inside, _controlLearn, _parameterLearn, _visible;
	int _guiWidth, _maxMessages;
	string _lastSelectedControl;
	string _CCXY[2];
	ofRectangle _position;
	map<string, float> _previousOutput, _MIDIOutMessages, _MIDIDumpMessages;
};
#endif

template<typename T>
inline BasePage<T>::BasePage()
{
}

template<typename T>
inline void BasePage<T>::setMapColorPallete(vector<ofColor> colorPallete)
{
	_map.setColorPallete(colorPallete);
}

template<typename T>
inline void BasePage<T>::update()
{
	_map.update();
	if (_map.getActive())
	{
		if (_map.getOutput() != _previousOutput)
		{
			addMidiMessages(_map.getOutput(), _MIDIOutMessages);
			_previousOutput = _map.getOutput();
		}
	}
	_gui->setVisible(_visible);
	_gui->setEnabled(_visible);
	_gui->update();
}

template<typename T>
inline void BasePage<T>::draw(ofTrueTypeFont font)
{
	ofPushStyle();
	_map.draw(_position.x, _position.y, _position.getWidth(), _position.getHeight(), font);
	ofSetColor(50);
	ofDrawRectangle(_position.x + _position.getWidth(), 0, _guiWidth, _position.getHeight());
	_gui->draw();
	ofPopStyle();
}

template<typename T>
inline void BasePage<T>::resize(int w, int h)
{
	_position = centerSquarePosition(w - _guiWidth, h);
	_gui->setPosition(_position.x + _position.getWidth(), 0);
	_gui->setMaxHeight(h);
	_gui->update();
}

template<typename T>
inline ofRectangle BasePage<T>::centerSquarePosition(int w, int h)
{
	ofRectangle rect;
	int max = w;
	int min = h;

	if (h > w)
	{
		min = w;
		max = h;
	}
	rect.setWidth(min);
	rect.setHeight(min);
	rect.setX(float(w - min) * 0.5);
	rect.setY(float(h - min) * 0.5);

	return rect;
}

template<typename T>
inline map<string, float> BasePage<T>::getMidiDump(bool clearMessages)
{
	map<string, float> out = _MIDIDumpMessages;
	if (clearMessages) clearMIDIMessages(_MIDIDumpMessages);
	return out;
}

template<typename T>
inline map<string, float> BasePage<T>::getMidiOut(bool clearMessages)
{
	map<string, float> out = _MIDIOutMessages;
	if (clearMessages) clearMIDIMessages(_MIDIOutMessages);
	return out;
}

template<typename T>
inline void BasePage<T>::setVisible(bool visible)
{
	if (_visible && !visible)
	{
		_gui->getToggle("controlLearn")->setChecked(false);
		_gui->getToggle("parameterLearn")->setChecked(false);
		_controlLearn = false;
		_parameterLearn = false;
	}
	_visible = visible;
	_inside = visible;
}

template<typename T>
inline void BasePage<T>::clearMIDIMessages()
{
	_MIDIOutMessages.clear();
	_MIDIDumpMessages.clear();
}

template<typename T>
inline void BasePage<T>::addMidiMessages(map<string, float> messages, map<string, float>& queue)
{
	queue.insert(messages.begin(), messages.end());
	while (queue.size() > _maxMessages) queue.erase(queue.begin());;
}

template<typename T>
inline void BasePage<T>::clearMIDIMessages(map<string, float>& queue)
{
	queue.clear();
}
