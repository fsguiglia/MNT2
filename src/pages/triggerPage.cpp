#include "triggerPage.h"

TriggerPage::TriggerPage()
{
	setUseGlobalParameters(false);
	setMidiOutput(true);
	setOscOutput(false);
	setStringOutput(false);
	setAddress("cbcs/");
}

void TriggerPage::setup(string name, int w, int h, int guiWidth, ofTrueTypeFont font, int maxMessages)
{
	_radius = 0.05;
	_threshold = 0.75;
	_map.setup(w, h);
	_map.setActive(false);
	_map.setRandomize(0.);
	_map.setDrawSelected(true);
	vector<ofVec2f> initialCursors = { ofVec2f(0,0) };
	_map.setCursors(initialCursors);

	MapPage::setup(name, w, h, guiWidth, font, maxMessages);
	setupGui();
}

void TriggerPage::setupGui()
{
	_gui->addToggle("randomize");
	_gui->addBreak();
	_gui->addLabel("Parameters")->setName("Parameters");
	_gui->getLabel("Parameters")->setLabelAlignment(ofxDatGuiAlignment::CENTER);
	_gui->addToggle("Switch");
	_gui->addSlider("Radius", 0., 1., _radius);
	_gui->addSlider("Threshold", 0., 1., _threshold);
	_gui->addBreak();
	_gui->addTextInput("add");
	_gui->addToggle("Learn parameters")->setName("parameterLearn");
	_gui->onButtonEvent(this, &TriggerPage::buttonEvent);
	_gui->onToggleEvent(this, &TriggerPage::toggleEvent);
	_gui->onSliderEvent(this, &TriggerPage::sliderEvent);
	_gui->onTextInputEvent(this, &TriggerPage::textInputEvent);
	_gui->setAutoDraw(false);
	_gui->setOpacity(0.5);
	_gui->setTheme(new ofxDatGuiThemeWireframe(), true);
	_gui->setWidth(_guiWidth, 0.3);
	_gui->setPosition(_position.x + _position.getWidth(), 0);
	_gui->setMaxHeight(ofGetHeight());
	_gui->setVisible(false);
	_gui->setEnabled(false);
	_gui->update();
}

void TriggerPage::buttonEvent(ofxDatGuiButtonEvent e)
{
	if (e.target->getName() == "clearMIDI")
	{
		clearMidiMap();
	}
}

void TriggerPage::sliderEvent(ofxDatGuiSliderEvent e)
{
	string name = e.target->getName();
	if (name == "x" || name == "y")
	{
		_lastControl = "slider/" + name;
		if (!_controlLearn)
		{
			ofVec2f cursor = _map.getCursors()[0];
			if (name == "x") cursor.x = e.value;
			if (name == "y") cursor.y = 1 - e.value;
			_map.setCursor(cursor, 0);

		}
	}
	else if (name == "Radius" || name == "Threshold")
	{
		if (_map.getLastSelected() >= 0)
		{
			if (name == "Radius")
			{
				_radius = e.value;
				_map.setRadius(_map.getLastSelected(), _radius);
			}
			if (name == "Threshold")
			{
				_threshold = e.value;
				_map.setThreshold(_map.getLastSelected(), _threshold);
			}
		}
	}
	else
	{
		int lastSelected = _map.getLastSelected();
		int channel = ofToInt(ofSplitString(name, "/")[0]);
		int control = ofToInt(ofSplitString(name, "/")[1]);
		float value = e.value;
		if (lastSelected != -1) _map.setPointParameter(lastSelected, name, value);
		pair<string, float> message;
		message.first = name;
		message.second = value;
		addMessages(message, _MIDIOutMessages);
		addMessages(message, _MIDIDumpMessages);
	}
}

void TriggerPage::toggleEvent(ofxDatGuiToggleEvent e)
{
	if (e.target->getName() == "controlLearn")
	{
		_controlLearn = e.checked;
		if (_gui->getToggle("parameterLearn")->getChecked())
		{
			_gui->getToggle("parameterLearn")->setChecked(false);
			_parameterLearn = false;
		}
	}
	if (e.target->getName() == "parameterLearn")
	{
		_parameterLearn = e.checked;
		if (_gui->getToggle("controlLearn")->getChecked())
		{
			_gui->getToggle("controlLearn")->setChecked(false);
			_controlLearn = false;
		}
	}
	if (e.target->getName() == "active")
	{
		if (_controlLearn)
		{
			_lastControl = "toggle/active";
			e.target->setChecked(false);
			_map.setActive(e.checked);
		}
		else
		{
			_map.setActive(e.checked);
			_gui->getToggle("parameterLearn")->setChecked(false);
			_parameterLearn = false;
		}
	}
	if (e.target->getName() == "randomize") _map.setRandomize(float(e.checked));
	if (e.target->getName() == "Mouse Control") _mouseControl = e.checked;
	if (e.target->getName() == "Switch")
	{
		if (_map.getLastSelected() != -1)
		{
			_map.setSwitch(_map.getLastSelected(), e.checked);
		}
	}
}

void TriggerPage::textInputEvent(ofxDatGuiTextInputEvent e)
{
	bool prevLearn = _parameterLearn;
	_parameterLearn = true;
	vector<string> split = ofSplitString(e.text, "/");
	if (split.size() == 1)
	{
		bool isNumber = split[0].find_first_not_of("0123456789") == std::string::npos;
		if (isNumber) MIDIIn("text_input", 1, ofToInt(split[0]), 0);
	}
	else if (split.size() == 2)
	{
		bool isNumber = split[0].find_first_not_of("0123456789") == std::string::npos;
		isNumber = isNumber && split[1].find_first_not_of("0123456789") == std::string::npos;
		if(isNumber)  MIDIIn("text_input", ofToInt(split[0]), ofToInt(split[1]), 0);
	}
	e.target->setText("");
	_parameterLearn = prevLearn;
}

void TriggerPage::updateSelected(int selected, Trigger trigger)
{
	_gui->clearRemovableSliders();
	for (auto value : trigger.getParameters())
	{
		vector<string> split = ofSplitString(value.first, "/");
		string sliderLabel = "ch" + split[0] + "/cc" + split[1];
		_gui->addSlider(sliderLabel, 0., 1., value.second);
		_gui->getSlider(sliderLabel)->setName(value.first);
		_gui->setRemovableSlider(value.first);
		_gui->getSlider(value.first)->setTheme(new ofxDatGuiThemeWireframe());
		_gui->setWidth(_guiWidth, 0.3);
		_gui->setOpacity(0.5);
	}
	_gui->getLabel("Parameters")->setLabel("Parameters: " + ofToString(selected));
	_gui->getToggle("Switch")->setChecked(_map.getPoint(selected).getSwitch());
	_gui->getSlider("Radius")->setValue(trigger.getRadius(), false);
	_gui->getSlider("Threshold")->setValue(trigger.getThreshold(), false);
	_gui->update();
	//for (auto port : _MIDIOutputs) sendMIDICC(parameters, port.second);
}

void TriggerPage::mouseMoved(int x, int y)
{
	if (_visible)
	{
		_inside = _position.inside(x, y);
		if (_inside && _mouseControl) _map.setCursor(normalize(ofVec2f(x, y), _position), 0);
	}
}

void TriggerPage::mouseDragged(int x, int y, int button)
{
	ofRectangle guiPosition(_gui->getPosition(), _gui->getWidth(), _gui->getHeight());
	bool insideGui = guiPosition.inside(x, y);
	_inside = _position.inside(x, y);
	if (button < 2 && _inside && !insideGui) {
		ofVec2f normPosition = normalize(ofVec2f(x, y), _position);
		if (_lastSelectedPoint >= 0) _map.movePoint(_lastSelectedPoint, normPosition);
	}
}

void TriggerPage::mousePressed(int x, int y, int button, bool doubleClick)
{
	_inside = _position.inside(x, y);
	ofRectangle guiPosition(_gui->getPosition(), _gui->getWidth(), _gui->getHeight());
	bool insideGui = guiPosition.inside(x, y);
	if (_inside && !insideGui)
	{
		ofVec2f pos = normalize(ofVec2f(x, y), _position);
		if (doubleClick) _map.addPoint(pos, _radius, _threshold, true);
		if (button < 2)
		{
			int lastSelected = _map.getLastSelected();
			array<float, 2> selection = _map.getClosest(pos, false);
			if (selection[1] < _minClickDistance)
			{
				_lastSelectedPoint = int(selection[0]);
				_map.setLastSelected(_lastSelectedPoint, ofGetElapsedTimeMillis());
				if (int(selection[0]) != lastSelected)
				{
					Trigger point = _map.getPoint(int(selection[0]));
					updateSelected(int(selection[0]), point);
				}
			}
			else _lastSelectedPoint = -1;
		}
	}
}

void TriggerPage::mouseReleased(int x, int y, int button)
{
	ofVec2f normalized = normalize(ofVec2f(x, y), _position);
	_inside = _position.inside(x, y);
	if (button == 2)
	{
		if (_inside)
		{
			_map.removePoint(normalized);
			if (_map.getPoints().size() > 0)
			{
				int curSelected = _map.getPoints().size() - 1;
				if (curSelected > 0)
				{
					_map.setLastSelected(curSelected);
					Trigger point = _map.getPoint(curSelected);
					updateSelected(curSelected, point);
				}
				else _map.setLastSelected(-1);
			}
		}
		else
		{
			_gui->update();
			_gui->updatePositions();
			string removableSlider = _gui->inside(x, y);
			if (removableSlider != "")
			{
				int lastSelected = _map.getLastSelected();
				if(lastSelected != -1) _map.removePointParameter(lastSelected, removableSlider);
				_gui->removeSlider(removableSlider);
				_gui->setPosition(_gui->getPosition().x, _gui->getPosition().y);
				_gui->update();
			}
		}
	}
	_lastSelectedPoint = -1;
}

void TriggerPage::mouseScrolled(int scroll)
{
	_gui->scroll(scroll);
}

void TriggerPage::load(ofJson& json)
{
	//clear current map
	_map.clearPoints();
	_gui->clearRemovableSliders();
	//load parameters to map and GUI
	for (ofJson curPoint : json["points"])
	{
		ofVec2f position = ofVec2f(curPoint["pos"]["x"], curPoint["pos"]["y"]);
		float radius = curPoint["radius"];
		float threshold = curPoint["threshold"];
		bool isSwitch = curPoint["switch"];
		int index = _map.addPoint(position, radius, threshold, isSwitch);
		if (curPoint.find("parameters") != curPoint.end())
		{
			auto obj = curPoint["parameters"].get<ofJson::object_t>();
			for (auto parameter : obj) _map.addPointParameter(index, parameter.first, parameter.second);
		}
	}

	if (_map.getPoints().size() != 0)
	{
		_map.setLastSelected(0, ofGetElapsedTimeMillis());
		Trigger point = _map.getPoint(0);
		updateSelected(0, point);
	}

	loadMidiMap(json);
	MapPage::load(json);
}

ofJson TriggerPage::save()
{
	ofJson jSave = MapPage::save();
	map<string, float> parameters = _map.getParameters();
	for (auto element : parameters) jSave["parameters"].push_back(element.first);
	vector<Trigger> points = _map.getPoints();
	for (int i = 0; i < points.size(); i++)
	{
		ofJson curPoint;
		curPoint["id"] = i;
		curPoint["pos"]["x"] = points[i].getPosition().x;
		curPoint["pos"]["y"] = points[i].getPosition().y;
		curPoint["radius"] = points[i].getRadius();
		curPoint["threshold"] = points[i].getThreshold();
		curPoint["switch"] = points[i].getSwitch();
		for (auto parameter : points[i].getParameters())
		{
			curPoint["parameters"][parameter.first] = parameter.second;
		}
		jSave["points"].push_back(curPoint);
	}
	
	saveMidiMap(jSave);
	return jSave;
}