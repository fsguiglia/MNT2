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
	map<string, float> _MIDIOutMessages, _MIDIDumpMessages;
};
#endif

template<typename T>
inline PageBase<T>::PageBase()
{
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
