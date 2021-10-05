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
	string controlName = sChannel + "/" + sControl;
	string controlLabel = "ch" + sChannel + "/cc" + sControl;

	bool valid = true;
	valid = valid && channel >= 0 && channel < 128;
	valid = valid && control >= 0 && control < 128;
	valid = valid && value >= 0 && value <= 1;
	if (valid)
	{
		if (_controlLearn)
		{
			vector<string> vLastControl = ofSplitString(_lastControl, "/");
			bool mapExists = false;
			for (auto element : _midiMap)
			{
				if (element.second == controlName) mapExists = true;
			}
			if (!mapExists)
			{
				if (_lastControl != "")
				{
					_midiMap[_lastControl] = controlName;
					if (vLastControl[0] == "toggle")
					{
						string newName = vLastControl[1] + "(" + controlLabel + ")";
						_gui->getToggle(vLastControl[1])->setLabel(newName);
					}
					else if (vLastControl[0] == "slider")
					{
						string newName = vLastControl[1] + "(" + controlLabel + ")";
						_gui->getSlider(vLastControl[1])->setLabel(newName);
					}
					else if (vLastControl[0] == "button")
					{
						string newName = vLastControl[1] + "(" + controlLabel + ")";
						_gui->getButton(vLastControl[1])->setLabel(newName);
					}
				}
			}
		}
		else
		{
			moduleMIDIMap(port, control, channel, value);
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

void Page::clearMidiMap()
{
	for (auto element : _midiMap)
	{
		vector<string> split = ofSplitString(element.first, "/");
		if (split[0] == "button")
		{
			_gui->getButton(split[1])->setLabel(_gui->getButton(split[1])->getName());
		}
		if (split[0] == "toggle")
		{
			_gui->getToggle(split[1])->setLabel(_gui->getToggle(split[1])->getName());
		}
		if (split[0] == "slider")
		{
			_gui->getSlider(split[1])->setLabel(_gui->getSlider(split[1])->getName());
		}
	}
	_midiMap.clear();
}

void Page::saveMidiMap(ofJson & json)
{
	for (auto element : _midiMap)
	{
		ofJson mapping;
		mapping["name"] = element.first;
		mapping["value"] = element.second;
		json["midi"].push_back(mapping);
	}
}

void Page::loadMidiMap(ofJson & json)
{
	for (auto element : json["midi"])
	{
		string name = element["name"].get<string>();
		string value = element["value"].get<string>();
		string sChannel = ofSplitString(value, "/")[0];
		string sControl = ofSplitString(value, "/")[1];
		string controlName = sChannel + "/" + sControl;
		string controlLabel = "ch" + sChannel + "/cc" + sControl;

		vector<string> vLastControl = ofSplitString(name, "/");
		_midiMap[name] = controlName;
		if (vLastControl[0] == "toggle")
		{
			string newName = vLastControl[1] + "(" + controlLabel + ")";
			_gui->getToggle(vLastControl[1])->setLabel(newName);
		}
		else if (vLastControl[0] == "button")
		{
			string newName = vLastControl[1] + "(" + controlLabel + ")";
			_gui->getButton(vLastControl[1])->setLabel(newName);
		}
		else if (vLastControl[0] == "slider")
		{
			string newName = vLastControl[1] + "(" + controlLabel + ")";
			_gui->getSlider(vLastControl[1])->setLabel(newName);
		}
	}
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
