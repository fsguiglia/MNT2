#pragma once
#include "ofMain.h"
#include "page.h"

template<typename T> class BasePage : public Page {
public:
	BasePage();

	void update();
	void draw(ofTrueTypeFont font);

	void setColorPallete(vector<ofColor> colorPallete);
	void setMinClickDistance(float distance);

	void setUseGlobalParameters(bool globalParameters);
	bool getUseGlobalParameters();

	void MIDIIn(string port, int channel, int control, float value);
	void handleMIDIIn();
	void OSCIn(string address, float value);
	void handleOSCIn();

protected:
	T _map;
	ofxDatGuiFolder* _controlFolder;
	ofxDatGuiFolder* _arrangeFolder;
	bool _mouseControl, _useGlobalParameters;
	int _lastSelectedPoint;
	float _minClickDistance;
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
inline void BasePage<T>::setMinClickDistance(float distance)
{
	_minClickDistance = distance;
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
inline void BasePage<T>::handleMIDIIn()
{
}

template<typename T>
inline void BasePage<T>::OSCIn(string address, float value)
{
	vector<string> split = ofSplitString(address, "/");
	if (split.size() > 0)
	{
		if (split[0] == "active")
		{
			bool curActiveState = (value == 1);
			_gui->getToggle("active")->setChecked(curActiveState);
			_map.setActive(curActiveState);
		}
	}
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
inline void BasePage<T>::handleOSCIn()
{
}
