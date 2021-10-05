#pragma once
#include "ofMain.h"
#include "page.h"

template<typename T> class BasePage : public Page {
public:
	BasePage();

	void setup(string name, int w, int h, int guiWidth, int maxMessages);
	void setupGui(string name);

	void update();
	void draw(ofTrueTypeFont font);

	void setColorPallete(vector<ofColor> colorPallete);
	void setMinClickDistance(float distance);

	void setUseGlobalParameters(bool globalParameters);
	bool getUseGlobalParameters();

	void moduleMIDIIn(string port, int control, int channel, float value);
	void moduleMIDIMap (string port, int control, int channel, float value);
	void moduleOSCIn(string address, float value);

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
	_mouseControl = false;
	_controlLearn = false;
	_parameterLearn = false;
	_inside = false;
	_visible = false;
}

template<typename T>
inline void BasePage<T>::setup(string name, int w, int h, int guiWidth, int maxMessages)
{
	_guiWidth = guiWidth;
	_position = centerSquarePosition(ofGetWidth() - _guiWidth, ofGetHeight());
	_maxMessages = maxMessages;
	setupGui(name);
}

template<typename T>
inline void BasePage<T>::setupGui(string name)
{
	_gui = new ScrollGui();
	_gui->addHeader(name, false)->setName("Header");
	_gui->addToggle("active");
	_gui->addToggle("MIDI learn")->setName("controlLearn");
	_gui->addButton("clear MIDI")->setName("clearMIDI");
	_controlFolder = _gui->addFolder("Control");
	_controlFolder->addToggle("Mouse Control");
	_controlFolder->addSlider("x", 0., 1.)->setName("x");
	_controlFolder->addSlider("y", 0., 1.)->setName("y");
	_controlFolder->collapse();
	_gui->addBreak();
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
inline void BasePage<T>::moduleMIDIIn(string port, int control, int channel, float value)
{
	string sControl = ofToString(control);
	string sChannel = ofToString(channel);
	string parameterName = sChannel + "/" + sControl;
	string controlName = port + "/" + parameterName;
	string sliderLabel = "ch" + sChannel + "/cc" + sControl;
	map<string, float> curParameters;

	if (_parameterLearn)
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

template<typename T>
inline void BasePage<T>::moduleMIDIMap(string port, int control, int channel, float value)
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
		for (auto element : _midiMap)
		{
			if (element.second == controlName)
			{
				vector<string> name = ofSplitString(element.first, "/");
				if (name[0] == "toggle")
				{
					bool checked = value >= 0.75;
					_gui->getToggle(name[1])->setChecked(checked);
					if (name[1] == "active") _map.setActive(checked);
				}
				else if (name[0] == "slider")
				{
					_gui->getSlider(name[1])->setValue(value);
				}
			}
		}
	}
}

template<typename T>
inline void BasePage<T>::moduleOSCIn(string address, float value)
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
