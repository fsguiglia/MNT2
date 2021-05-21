#pragma once
#ifndef _PAGEBASE
#define _PAGEBASE
#include "ofMain.h"
#include "scrollGui.h"
#include "ofxDatGui.h"
#include "mntUtils.h"

template<typename T> class PageBase {
public:
	PageBase();

	virtual void setup(int width, int height, int guiWidth, int maxMessages = 20) = 0;
	void update();
	void draw(ofTrueTypeFont font);

	void resize(int w, int h);
	ofRectangle centerSquarePosition(int w, int h);

	map<string, float> getMidiDump(bool clear = true);
	map<string, float> getMidiOut(bool clear = true);
	void setVisible(bool visible);

protected:
	void addMidiMessages(map<string, float> messages, map<string, float>& queue);
	void clearMIDIMessages(map<string, float>& queue);

	T _map;
	ScrollGui *_gui;
	bool _mouseControl, _inside, _controlLearn, _parameterLearn, _visible;
	int _guiWidth, _maxMessages;
	string _lastSelectedControl;
	string _CCXY[2];
	ofRectangle _position;
	map<string, float> _previousOutput, _MIDIOutMessages, _MIDIDumpMessages;
};
#endif

template<typename T>
inline PageBase<T>::PageBase()
{
}

template<typename T>
inline void PageBase<T>::update()
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
inline void PageBase<T>::draw(ofTrueTypeFont font)
{
	ofPushStyle();
	_map.draw(_position.x, _position.y, _position.getWidth(), _position.getHeight(), font);
	ofSetColor(50);
	ofDrawRectangle(_position.x + _position.getWidth(), 0, _guiWidth, _position.getHeight());
	_gui->draw();
	ofPopStyle();
}

template<typename T>
inline void PageBase<T>::resize(int w, int h)
{
	_position = centerSquarePosition(w - _guiWidth, h);
	_gui->setPosition(_position.x + _position.getWidth(), 0);
	_gui->setMaxHeight(h);
	_gui->update();
}

template<typename T>
inline ofRectangle PageBase<T>::centerSquarePosition(int w, int h)
{
	ofRectangle rect;
	int max = w;
	int min = h;

	if (w < h)
	{
		max = w;
		min = h;
	}
	rect.setWidth(min);
	rect.setHeight(min);
	rect.setX((w - min) * 0.5);
	rect.setY((h - min) * 0.5);

	return rect;
}

template<typename T>
inline map<string, float> PageBase<T>::getMidiDump(bool clearMessages)
{
	map<string, float> out = _MIDIDumpMessages;
	if (clearMessages) clearMIDIMessages(_MIDIDumpMessages);
	return out;
}

template<typename T>
inline map<string, float> PageBase<T>::getMidiOut(bool clearMessages)
{
	map<string, float> out = _MIDIOutMessages;
	if (clearMessages) clearMIDIMessages(_MIDIOutMessages);
	return out;
}

template<typename T>
inline void PageBase<T>::setVisible(bool visible)
{
	_visible = visible;
	_inside = visible;
	if (!visible)
	{
		_gui->getToggle("controlLearn")->setChecked(false);
		_gui->getToggle("parameterLearn")->setChecked(false);
		_controlLearn = false;
		_parameterLearn = false;
	}
}

template<typename T>
inline void PageBase<T>::addMidiMessages(map<string, float> messages, map<string, float>& queue)
{
	queue.insert(messages.begin(), messages.end());
	while (queue.size() > _maxMessages) queue.erase(queue.begin());;
}

template<typename T>
inline void PageBase<T>::clearMIDIMessages(map<string, float>& queue)
{
	queue.clear();
}
