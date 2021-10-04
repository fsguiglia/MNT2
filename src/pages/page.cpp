#include "page.h"

void Page::setup(int w, int h, int guiWidth, int maxMessages)
{
	_guiWidth = guiWidth;
	_position = centerSquarePosition(ofGetWidth() - _guiWidth, ofGetHeight());
	_maxMessages = maxMessages;
}

void Page::setHeader(string label)
{
	_gui->getHeader()->setLabel(label);
}

Page::Page()
{
}

ofVec2f Page::getPosition()
{
	return ofVec2f(_position.getPosition());
}

int Page::getHeight()
{
	return _position.getHeight();
}

int Page::getWidth()
{
	return (_position.getWidth() + _guiWidth);
}

void Page::resize(int w, int h)
{
	_position = centerSquarePosition(w - _guiWidth, h);
	_gui->setPosition(_position.x + _position.getWidth(), 0);
	_gui->update();
}

void Page::setVisible(bool visible)
{
	if (_visible && !visible)
	{
		if (_controlLearn) _gui->getToggle("controlLearn")->setChecked(false);
		if (_parameterLearn) _gui->getToggle("parameterLearn")->setChecked(false);
		_controlLearn = false;
		_parameterLearn = false;
	}
	_visible = visible;
	_inside = visible;
}

void Page::setMidiOutput(bool midiOutput)
{
	_midiOutput = midiOutput;
}

void Page::setOscOutput(bool oscOutput)
{
	_oscOutput = oscOutput;
}

void Page::setStringOutput(bool stringOutput)
{
	_stringOutput = stringOutput;
}

bool Page::getMidiOutput()
{
	return _midiOutput;
}

bool Page::getOscOutput()
{
	return _oscOutput;
}

bool Page::getStringOutput()
{
	return _stringOutput;
}

void Page::MIDIIn(string port, int control, int channel, float value)
{
	/*
		aca separar entre mapeo de sliders y etc con acciones al interior del mapa-> esto ultimo en un metodo privado
	*/
}

map<string, float> Page::getMidiOut(bool clear)
{
	map<string, float> out = _MIDIOutMessages;
	if (clear) clearMessages(_MIDIOutMessages);
	return out;
}

map<string, float> Page::getMidiDump(bool clear)
{
	map<string, float> out = _MIDIDumpMessages;
	if (clear) clearMessages(_MIDIDumpMessages);
	return out;
}

void Page::OSCIn(string address, float value)
{
	/*
		aca separar active y control de especifico de cada pagina y hacer una nueva funcion como en midi
	*/
}

map<string, float> Page::getOscOut(bool clear)
{
	map<string, float> out = _OSCOutMessages;
	if (clear) clearMessages(_OSCOutMessages);
	return out;
}

void Page::setAddress(string address)
{
	_address = address;
}

string Page::getAddress()
{
	return _address;
}

vector<string> Page::getStringOut(bool clear)
{
	vector<string> out = _stringMessages;
	if (clear) _stringMessages.clear();
	return out;
}

void Page::clearMessages()
{
	_MIDIOutMessages.clear();
	_MIDIDumpMessages.clear();
	_OSCOutMessages.clear();
	_stringMessages.clear();
}

void Page::clearMappings()
{
	/*
		ver
	*/
}

void Page::addMessages(map<string, float> messages, map<string, float>& queue)
{
	queue.insert(messages.begin(), messages.end());
	while (queue.size() > _maxMessages) queue.erase(queue.begin());;
}

void Page::setStringMessages(vector<string> messages)
{
	_stringMessages = messages;
}

void Page::clearMessages(map<string, float>& queue)
{
	queue.clear();
}

ofRectangle Page::centerSquarePosition(int w, int h)
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
