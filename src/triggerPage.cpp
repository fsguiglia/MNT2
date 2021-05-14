#include "triggerPage.h"

TriggerPage::TriggerPage()
{
}

void TriggerPage::setup(int width, int height, int guiWidth, int maxMessages)
{
	_map.setup(width, height);
	_guiWidth = guiWidth;
	_position = centerSquarePosition(ofGetWidth() - _guiWidth, ofGetHeight());
	_map.setActive(false);
	_map.setRandomize(0.);
	_map.setDrawSelected(true);
	_mouseControl = false;
	_inside = false;
	_visible = false;
	setupGui(_map.getParameters(), false);
	_maxMessages = maxMessages;
	vector<ofVec2f> initialCursors = { ofVec2f(0,0) };
	_map.setCursors(initialCursors);
	_radius = 0.05;
	_threshold = 0.75;
}

void TriggerPage::setupGui(map<string, float> parameters, bool toggleState)
{
	_gui = new ScrollGui();
	_gui->addHeader("Trigger", false);
	_gui->addToggle("active");
	_gui->addToggle("randomize");
	_gui->addBreak();
	_gui->addLabel("Control")->setLabelAlignment(ofxDatGuiAlignment::CENTER);
	_gui->addToggle("learn", toggleState)->setName("controlLearn");
	_gui->addToggle("Mouse Control");
	_gui->addSlider("x", 0., 1.)->setName("x");
	_gui->addSlider("y", 0., 1.)->setName("y");
	_gui->addBreak();
	_gui->addLabel("Parameters")->setName("Parameters");
	_gui->getLabel("Parameters")->setLabelAlignment(ofxDatGuiAlignment::CENTER);
	_gui->addToggle("learn", toggleState)->setName("parameterLearn");
	_gui->onToggleEvent(this, &TriggerPage::toggleEvent);
	_gui->addSlider("Radius", 0., 1., _radius);
	_gui->addSlider("Threshold", 0., 1., _threshold);
	for (auto parameter : parameters) _gui->addSlider(parameter.first, parameter.second, 0., 1.);
	_gui->onSliderEvent(this, &TriggerPage::sliderEvent);
	_gui->setAutoDraw(false);
	_gui->setOpacity(0.5);
	_gui->setTheme(new ofxDatGuiThemeWireframe(), true);
	_gui->setWidth(_guiWidth, 0.3);
	_gui->setPosition(ofGetWidth() - _guiWidth, 0);
	_gui->setMaxHeight(ofGetHeight());
	_gui->update();
}

void TriggerPage::update()
{
	_map.update();
	if (_map.getActive())
	{
		if (_map.getOutput() != _previousOutput)
		{
			//addMidiMessages(_map.getOutput(), _MIDIOutMessages);
			//_previousOutput = _map.getOutput();
		}
	}
	_gui->setVisible(_visible);
	_gui->setEnabled(_visible);
	_gui->update();
}

void TriggerPage::draw(ofTrueTypeFont & font)
{
	ofPushStyle();
	_map.draw(_position.x, _position.y, _position.getWidth(), _position.getHeight(), font);
	_gui->draw();
	ofPopStyle();
}

void TriggerPage::sliderEvent(ofxDatGuiSliderEvent e)
{
	string name = e.target->getName();
	if (name == "x" || name == "y")
	{
		_lastSelectedControl = name;
		if (!_controlLearn)
		{
			ofVec2f nniCursor = _map.getCursors()[0];
			if (name == "x") nniCursor.x = e.value;
			if (name == "y") nniCursor.y = e.value;
			_map.setCursor(nniCursor, 0);

		}
	}
	else if (name == "Radius" || name == "Threshold")
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
	else
	{
		int channel = ofToInt(ofSplitString(name, "/")[1]);
		int control = ofToInt(ofSplitString(name, "/")[2]);
		float value = e.value;
		_map.setParameter(name, value);
		map<string, float> message;
		message[name] = value;
		addMidiMessages(message, _MIDIDumpMessages);
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
	if (e.target->getName() == "active") _map.setActive(e.checked);
	if (e.target->getName() == "randomize") _map.setRandomize(float(e.checked));
	if (e.target->getName() == "Mouse Control") _mouseControl = e.checked;
}

void TriggerPage::updateSelected(int selected, map<string, float> parameters, float radius, float threshold)
{
	_gui->getLabel("Parameters")->setLabel("Parameters - Site: " + ofToString(selected));
	for (auto parameter : parameters) _gui->getSlider(parameter.first)->setValue(parameter.second);
	_gui->getSlider("Radius")->setValue(radius);
	_gui->getSlider("Threshold")->setValue(threshold);
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
		_map.movePoint(_map.getLastSelected(), normPosition);
	}
}

void TriggerPage::mousePressed(int x, int y, int button)
{
	_inside = _position.inside(x, y);
	ofRectangle guiPosition(_gui->getPosition(), _gui->getWidth(), _gui->getHeight());
	bool insideGui = guiPosition.inside(x, y);
	if (_inside && !insideGui)
	{
		ofVec2f pos = normalize(ofVec2f(x, y), _position);
		if (button == 0) _map.addPoint(pos, _radius, _threshold);
		if (button < 2)
		{
			int lastSelected = _map.getLastSelected();
			int curSelected = _map.getClosest(pos, true)[0];
			if (curSelected != lastSelected)
			{
				Trigger point = _map.getPoint(curSelected);
				updateSelected(curSelected, point.getValues(), point.getRadius(), point.getThreshold());
			}
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
					updateSelected(curSelected, point.getValues(), point.getRadius(), point.getThreshold());
				}
				else _map.setLastSelected(-1);
			}
		}
		else
		{
			_map.getClosest(normalized, true);
			_gui->update();
			_gui->updatePositions();
			string removableSlider = _gui->inside(x, y);
			if (removableSlider != "")
			{
				_map.removeParameter(removableSlider);
				_gui->removeSlider(removableSlider);
				_gui->setPosition(_gui->getPosition().x, _gui->getPosition().y);
				_gui->update();
			}
		}
	}
}

void TriggerPage::mouseScrolled(int scroll)
{
	_gui->scroll(scroll);
}

void TriggerPage::resize(int w, int h)
{
	_position = centerSquarePosition(w - _guiWidth, h);
	_gui->setPosition(w - _guiWidth, 0);
	_gui->setMaxHeight(h);
	_gui->update();
}

void TriggerPage::MIDIIn(string port, int control, int channel, float value)
{
	string sControl = ofToString(control);
	string sChannel = ofToString(channel);
	string parameter = port + "/" + sChannel + "/" + sControl;
	string sliderLabel = "cc" + sControl;
	map<string, float> curParameters = _map.getParameters();
	if (_controlLearn)
	{
		if (_lastSelectedControl == "x")
		{
			_CCXY[0] = parameter;
			_gui->getSlider("x")->setLabel("x:cc" + control);
		}
		if (_lastSelectedControl == "y")
		{
			_CCXY[1] = parameter;
			_gui->getSlider("y")->setLabel("y:cc" + control);
		}
	}
	if (_parameterLearn)
	{
		if (curParameters.find(parameter) == curParameters.end())
		{
			_map.addParameter(parameter, value);
			_gui->addSlider(sliderLabel, 0., 1.);
			_gui->getSlider(sliderLabel)->setName(parameter);
			_gui->setRemovableSlider(parameter);
			_gui->getSlider(parameter)->setTheme(new ofxDatGuiThemeWireframe());
			_gui->setWidth(300, 0.3);
			_gui->setOpacity(0.5);
			_gui->update();
		}
		else
		{
			_gui->getSlider(parameter)->setValue(value, false);
			_map.setParameter(parameter, value);
			if (_map.getLastSelected() > 0) _map.setParameter(_map.getLastSelected(), parameter, value);
		}
	}
	else
	{
		if (parameter == _CCXY[0]) _gui->getSlider("x")->setValue(value);
		if (parameter == _CCXY[1]) _gui->getSlider("y")->setValue(value);
	}
}

void TriggerPage::load(ofJson& json)
{
	//clear current NNI
	_map.clearPoints();
	_gui->clearRemovableSliders();
	//load parameters to NNI and GUI
	for (auto parameter : json["parameters"])
	{
		_map.addParameter(parameter, 0);
		//GUI
		string sliderLabel = "cc" + ofSplitString(parameter, "/").back();
		_gui->addSlider(sliderLabel, 0., 1.);
		_gui->getSlider(sliderLabel)->setName(parameter);
		_gui->setRemovableSlider(parameter);
		_gui->getSlider(parameter)->setTheme(new ofxDatGuiThemeWireframe());
		_gui->setWidth(300, 0.3);
		_gui->setOpacity(0.5);
	}
	for (ofJson site : json["sites"])
	{
		for (auto parameter : _map.getParameters())
		{
			_map.setParameter(parameter.first, site["parameters"][parameter.first]);
		}
		_map.addPoint(ofVec2f(site["pos"]["x"], site["pos"]["y"]), _radius, _threshold);
	}
	//update gui
	_gui->update();
	if (_map.getPoints().size() != 0)
	{
		Trigger point = _map.getPoint(0);
		updateSelected(0, point.getValues(), point.getRadius(), point.getThreshold());
	}
}

ofJson TriggerPage::save()
{
	ofJson jSave;
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
		for (auto parameter : points[i].getValues())
		{
			curPoint["parameters"][parameter.first] = parameter.second;
		}
		jSave["sites"].push_back(curPoint);
	}
	return jSave;
}

