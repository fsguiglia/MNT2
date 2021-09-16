#pragma once
#include "ofMain.h"
#include "../gui/scrollGui.h"
#include "../utils/mntUtils.h"
#include "ofxDatGui.h"

template<typename T> class BasePage {
public:
	BasePage();

	virtual void setup(int width, int height, int guiWidth, int maxMessages = 20) = 0;
	void setColorPallete(vector<ofColor> colorPallete);
	void update();
	void draw(ofTrueTypeFont font);

	ofVec2f getPosition();
	int getHeight();
	int getWidth();
	void resize(int w, int h);
	ofRectangle centerSquarePosition(int w, int h);

	void setMinClickDistance(float distance);

	void setAddress(string address);
	string getAddress();

	void setMidiOutput(bool midiOutput);
	void setOscOutput(bool oscOutput);
	void setStringOutput(bool stringOutput);
	bool getMidiOutput();
	bool getOscOutput();
	bool getStringOutput();

	void setUseGlobalParameters(bool globalParameters);
	bool getUseGlobalParameters();

	void MIDIIn(string port, int channel, int control, float value);
	void OSCIn(string address, float value);

	map<string, float> getOscOut(bool clear = false);
	vector<string> getStringOut(bool clear = false);
	map<string, float> getMidiDump(bool clear = false);
	map<string, float> getMidiOut(bool clear = false);
	
	void setVisible(bool visible);
	void clearMessages();

	virtual void load(ofJson& json) = 0;
	virtual ofJson save() = 0;

protected:
	void addMessages(map<string, float> messages, map<string, float>& queue);
	void setStringMessages(vector<string> messages);
	void clearMessages(map<string, float>& queue);

	T _map;
	ScrollGui* _gui;
	ofxDatGuiFolder* _controlFolder;
	ofxDatGuiFolder* _arrangeFolder;
	bool _mouseControl, _inside, _controlLearn, _parameterLearn, _visible, _useGlobalParameters;
	bool _midiOutput, _oscOutput, _stringOutput;
	int _guiWidth, _maxMessages, _lastSelectedPoint;
	float _minClickDistance;
	string _lastSelectedControl, _address;
	string _CCXY[2];
	ofRectangle _position;
	map<string, float> _previousOutput, _OSCOutMessages, _MIDIOutMessages, _MIDIDumpMessages;
	vector<string> _stringMessages;
};

template<typename T>
inline BasePage<T>::BasePage()
{
	_lastSelectedPoint = -1;
	_minClickDistance = 0.1;
}

template<typename T>
inline void BasePage<T>::setColorPallete(vector<ofColor> colorPallete)
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
			if (_stringOutput)
			{
				vector<string> curOutput;
				for (auto element : _map.getOutput()) curOutput.push_back(element.first + ";" + ofToString(element.second));
				setStringMessages(curOutput);
			}
			if (_oscOutput) addMessages(_map.getOutput(), _OSCOutMessages);
			if (_midiOutput) addMessages(_map.getOutput(), _MIDIOutMessages);
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
inline ofVec2f BasePage<T>::getPosition()
{
	return ofVec2f(_position.getPosition());
}

template<typename T>
inline int BasePage<T>::getHeight()
{
	return _position.getHeight();
}

template<typename T>
inline int BasePage<T>::getWidth()
{
	return (_position.getWidth() + _guiWidth);
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
inline void BasePage<T>::setMinClickDistance(float distance)
{
	_minClickDistance = distance;
}

template<typename T>
inline void BasePage<T>::setAddress(string address)
{
	_address = address;
}

template<typename T>
inline string BasePage<T>::getAddress()
{
	return _address;
}

template<typename T>
inline void BasePage<T>::setMidiOutput(bool midiOutput)
{
	_midiOutput = midiOutput;
}

template<typename T>
inline void BasePage<T>::setOscOutput(bool oscOutput)
{
	_oscOutput = oscOutput;
}

template<typename T>
inline void BasePage<T>::setStringOutput(bool stringOutput)
{
	_stringOutput = stringOutput;
}

template<typename T>
inline bool BasePage<T>::getMidiOutput()
{
	return _midiOutput;
}

template<typename T>
inline bool BasePage<T>::getOscOutput()
{
	return _oscOutput;
}

template<typename T>
inline bool BasePage<T>::getStringOutput()
{
	return _stringOutput;
}

template<typename T>
inline void BasePage<T>::setUseGlobalParameters(bool globalParameters)
{
	_useGlobalParameters = globalParameters;
}

template<typename T>
inline bool BasePage<T>::getUseGlobalParameters()
{
	return _useGlobalParameters;
}

template<typename T>
inline void BasePage<T>::MIDIIn(string port, int channel, int control, float value)
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
		if (!_map.getActive())
		{
			if (_controlLearn)
			{
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
			else if (_parameterLearn)
			{
				int lastSelected = _map.getLastSelected();
				if (lastSelected != -1)
				{
					if (_useGlobalParameters) curParameters = _map.getParameters();
					else curParameters = _map.getPoint(lastSelected).getValues();

					if (curParameters.find(parameterName) == curParameters.end())
					{
						if (_useGlobalParameters) _map.addGlobalParameter(parameterName, value);
						else _map.addPointParameter(lastSelected, parameterName, value);

						_gui->addSlider(sliderLabel, 0., 1.);
						_gui->getSlider(sliderLabel)->setName(parameterName);
						_gui->setRemovableSlider(parameterName);
						_gui->getSlider(parameterName)->setTheme(new ofxDatGuiThemeWireframe());
						_gui->setWidth(300, 0.3);
						_gui->setOpacity(0.5);
						_gui->update();
					}
					else if (curParameters[parameterName] != value)
					{
						if (_useGlobalParameters) _map.setGlobalParameter(parameterName, value);
						_map.setPointParameter(_map.getLastSelected(), parameterName, value);
						_gui->getSlider(parameterName)->setValue(value, false);

						map<string, float> curMessage;
						curMessage[parameterName] = value;
						addMessages(curMessage, _MIDIOutMessages);
					}
				}
			}
		}
		if (!_controlLearn)
		{
			if (controlName == _CCXY[0]) _gui->getSlider("x")->setValue(value);
			if (controlName == _CCXY[1]) _gui->getSlider("y")->setValue(value);
		}
	}
}

template<typename T>
inline void BasePage<T>::OSCIn(string address, float value)
{
	vector<string> split = ofSplitString(address, "/");
	if (split.size() > 1)
	{
		if (split[0] == "control")
		{
			ofVec2f cursor = _map.getCursors()[0];
			if (value > 1) value = 1;
			if (value < 0) value = 0;
			if (split[1] == "x") cursor.x = value;
			if (split[1] == "y") cursor.y = value;
			_map.setCursor(cursor, 0);
		}
		else if (split[0] == "point")
		{
			int index = ofToInt(split[1]);
			if (index < _map.getPoints().size())
			{
				ofVec2f position = _map.getPoint(index).getPosition();
				if (value > 1) value = 1;
				if (value < 0) value = 0;
				if (split[2] == "x") position.x = value;
				if (split[2] == "y") position.y = value;
				_map.movePoint(index, position);
			}
		}
	}
}

template<typename T>
inline map<string, float> BasePage<T>::getOscOut(bool clear)
{
	map<string, float> out = _OSCOutMessages;
	if (clear) clearMessages(_OSCOutMessages);
	return out;
}

template<typename T>
inline vector<string> BasePage<T>::getStringOut(bool clear)
{
	vector<string> out = _stringMessages;
	if (clear) _stringMessages.clear();
	return out;
}

template<typename T>
inline map<string, float> BasePage<T>::getMidiDump(bool clear)
{
	map<string, float> out = _MIDIDumpMessages;
	if (clear) clearMessages(_MIDIDumpMessages);
	return out;
}

template<typename T>
inline map<string, float> BasePage<T>::getMidiOut(bool clear)
{
	map<string, float> out = _MIDIOutMessages;
	if (clear) clearMessages(_MIDIOutMessages);
	return out;
}

template<typename T>
inline void BasePage<T>::setVisible(bool visible)
{
	if (_visible && !visible)
	{
		if(_controlLearn) _gui->getToggle("controlLearn")->setChecked(false);
		if(_parameterLearn) _gui->getToggle("parameterLearn")->setChecked(false);
		_controlLearn = false;
		_parameterLearn = false;
	}
	_visible = visible;
	_inside = visible;
}

template<typename T>
inline void BasePage<T>::clearMessages()
{
	_MIDIOutMessages.clear();
	_MIDIDumpMessages.clear();
	_OSCOutMessages.clear();
	_stringMessages.clear();
}

template<typename T>
inline void BasePage<T>::addMessages(map<string, float> messages, map<string, float>& queue)
{
	queue.insert(messages.begin(), messages.end());
	while (queue.size() > _maxMessages) queue.erase(queue.begin());;
}

template<typename T>
inline void BasePage<T>::setStringMessages(vector<string> messages)
{
	_stringMessages = messages;
}

template<typename T>
inline void BasePage<T>::clearMessages(map<string, float>& queue)
{
	queue.clear();
}
