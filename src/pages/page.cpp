#include "page.h"

Page::Page()
{
	_controlLearn = false;
	_parameterLearn = false;
}

void Page::setHeader(string label)
{
	_gui->getHeader()->setLabel(label);
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
	string sControl = ofToString(control);
	string sChannel = ofToString(channel);
	string parameterName = sChannel + "/" + sControl;
	string controlName = port + "/" + parameterName;
	string sliderLabel = "ch" + sChannel + "/cc" + sControl;
	map<string, float> curParameters;

	bool valid = true;
	valid = valid && channel >= 0 && channel < 128;
	valid = valid && control >= 0 && control < 128;
	valid = valid && value >= 0 && value < 1;
	if (valid)
	{
		if (_controlLearn) {
			if (_lastSelectedControl == "x")
			{
				_CCXY[0] = controlName;
				_gui->getSlider("x")->setLabel("x:" + sliderLabel);
			}
			if (_lastSelectedControl == "y")
			{
				_CCXY[1] = controlName;
				_gui->getSlider("y")->setLabel("y:" + sliderLabel);
			}
		}
		else
		{
			if (controlName == _CCXY[0]) _gui->getSlider("x")->setValue(value);
			if (controlName == _CCXY[1]) _gui->getSlider("y")->setValue(value);
		}
		moduleMIDIIn(port, control, channel, value);
	}
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
	moduleOSCIn(address, value);
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
