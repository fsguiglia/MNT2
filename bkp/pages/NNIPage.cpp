#include "NNIpage.h"

NNIPage::NNIPage()
{
}

void NNIPage::setup(int width, int height, int guiWidth, int maxMessages)
{
	_map.setup(width, height);
	_guiWidth = guiWidth;
	_position = centerSquarePosition(ofGetWidth() - _guiWidth, ofGetHeight());
	_map.setActive(false);
	_map.setRandomize(0.);
	_map.setDrawInterpolation(true);
	_map.setDrawSelected(true);
	_mouseControl = false;
	_inside = false;
	_visible = false;
	setupGui();
	_maxMessages = maxMessages;
}

void NNIPage::setupGui()
{
	_gui = new ScrollGui();
	_gui->addHeader("NNI", false);
	_gui->addToggle("active");
	_gui->addToggle("randomize");
	_gui->addBreak();
	_controlFolder = _gui->addFolder("Control");
	_controlFolder->addToggle("learn")->setName("controlLearn");
	_controlFolder->addToggle("Mouse Control");
	_controlFolder->addSlider("x", 0., 1.)->setName("x");
	_controlFolder->addSlider("y", 0., 1.)->setName("y");
	_controlFolder->collapse();
	_gui->addBreak();
	_gui->addLabel("Parameters")->setName("Parameters");
	_gui->getLabel("Parameters")->setLabelAlignment(ofxDatGuiAlignment::CENTER);
	_gui->addToggle("learn")->setName("parameterLearn");
	_gui->onToggleEvent(this, &NNIPage::toggleEvent);
	_gui->onSliderEvent(this, &NNIPage::sliderEvent);
	_gui->setAutoDraw(false);
	_gui->setOpacity(0.5);
	_gui->setTheme(new ofxDatGuiThemeWireframe(), true);
	_gui->setWidth(_guiWidth, 0.3);
	_gui->setPosition(ofGetWidth() - _guiWidth, 0);
	_gui->setMaxHeight(ofGetHeight());
	_gui->setEnabled(false);
	_gui->setVisible(false);
	_gui->update();
}

void NNIPage::sliderEvent(ofxDatGuiSliderEvent e)
{
	string name = e.target->getName();
	if (name == "x" || name == "y")
	{
		_lastSelectedControl = name;
		if (!_controlLearn)
		{
			ofVec2f nniCursor = _map.getCursor();
			if (name == "x") nniCursor.x = e.value;
			if (name == "y") nniCursor.y = e.value;
			_map.setCursor(nniCursor);
		}
	}
	else
	{
		int channel = ofToInt(ofSplitString(name, "/")[1]);
		int control = ofToInt(ofSplitString(name, "/")[2]);
		float value = e.value;
		_map.setGlobalParameter(name, value);
		map<string, float> message;
		message[name] = value;
		addMidiMessages(message, _MIDIDumpMessages);
	}
}

void NNIPage::toggleEvent(ofxDatGuiToggleEvent e)
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

void NNIPage::updateSelected(int selected, Point point)
{
	map<string, float> parameters = point.getValues();
	_gui->getLabel("Parameters")->setLabel("Parameters: " + ofToString(selected));
	for (auto parameter : parameters) _gui->getSlider(parameter.first)->setValue(parameter.second);
	//for (auto port : _MIDIOutputs) sendMIDICC(parameters, port.second);
}

void NNIPage::mouseMoved(int x, int y)
{
	if (_visible)
	{
		_inside = _position.inside(x, y);
		if (_inside && _mouseControl) _map.setCursor(normalize(ofVec2f(x, y), _position));
	}
}

void NNIPage::mouseDragged(int x, int y, int button)
{
	ofRectangle guiPosition(_gui->getPosition(), _gui->getWidth(), _gui->getHeight());
	bool insideGui = guiPosition.inside(x, y);
	_inside = _position.inside(x, y);
	if (button < 2 && _inside && !insideGui) {
		ofVec2f normPosition = normalize(ofVec2f(x, y), _position);
		_map.movePoint(_map.getLastSelected(), normPosition);
	}
}

void NNIPage::mousePressed(int x, int y, int button)
{
	_inside = _position.inside(x, y);
	ofRectangle guiPosition(_gui->getPosition(), _gui->getWidth(), _gui->getHeight());
	bool insideGui = guiPosition.inside(x, y);
	if (_inside && !insideGui)
	{
		ofVec2f pos = normalize(ofVec2f(x, y), _position);
		if (button == 0) _map.addPoint(pos);
		if (button < 2)
		{
			int lastSelected = _map.getLastSelected();
			int curSelected = _map.getClosest(pos, true)[0];
			if (curSelected != lastSelected)
			{
				updateSelected(curSelected, _map.getPoint(curSelected));
			}
		}
	}
}

void NNIPage::mouseReleased(int x, int y, int button)
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
				int curPoint = _map.getPoints().size() - 1;
				if (curPoint >= 0)
				{
					_map.setLastSelected(curPoint);
					updateSelected(curPoint, _map.getPoint(curPoint));
				}
				else _map.setLastSelected(-1);
			}
		}
		else
		{
			//_map.getClosest(normalized, true);
			_gui->update();
			_gui->updatePositions();
			string removableSlider = _gui->inside(x, y);
			cout << removableSlider << endl;
			if (removableSlider != "")
			{
				_map.removeGlobalParameter(removableSlider);
				_gui->removeSlider(removableSlider);
				_gui->setPosition(_gui->getPosition().x, _gui->getPosition().y);
				_gui->update();
			}
		}
	}
}

void NNIPage::mouseScrolled(int scroll)
{
	_gui->scroll(scroll);
}

void NNIPage::MIDIIn(string port, int control, int channel, float value)
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
			_map.addGlobalParameter(parameter, value);
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
			_map.setGlobalParameter(parameter, value);
			if (_map.getLastSelected() >= 0) _map.setPointParameter(_map.getLastSelected(), parameter, value);
		}
	}
	else
	{
		if (parameter == _CCXY[0]) _gui->getSlider("x")->setValue(value);
		if (parameter == _CCXY[1]) _gui->getSlider("y")->setValue(value);
	}
}

void NNIPage::load(ofJson& json)
{
	//clear current NNI
	_map.clearPoints();
	_gui->clearRemovableSliders();
	//load parameters to NNI and GUI
	for (auto parameter : json["parameters"])
	{
		_map.addGlobalParameter(parameter, 0);
		//GUI
		string sliderLabel = "cc" + ofSplitString(parameter, "/").back();
		_gui->addSlider(sliderLabel, 0., 1.);
		_gui->getSlider(sliderLabel)->setName(parameter);
		_gui->setRemovableSlider(parameter);
		_gui->getSlider(parameter)->setTheme(new ofxDatGuiThemeWireframe());
		_gui->setWidth(300, 0.3);
		_gui->setOpacity(0.5);
	}
	for (ofJson site : json["points"])
	{
		for (auto parameter : _map.getParameters())
		{
			_map.setGlobalParameter(parameter.first, site["parameters"][parameter.first]);
		}
		_map.addPoint(ofVec2f(site["pos"]["x"], site["pos"]["y"]));
	}
	//gui
	_CCXY[0] = json["MIDIMap"]["x"].get<string>();
	_CCXY[1] = json["MIDIMap"]["y"].get<string>();
	_gui->update();
	if (_map.getPoints().size() != 0)
	{
		updateSelected(0, _map.getPoint(0));
	}
}

ofJson NNIPage::save()
{
	ofJson jSave;
	map<string, float> nniParameters = _map.getParameters();
	for (auto element : nniParameters) jSave["parameters"].push_back(element.first);
	vector<Point> points = _map.getPoints();
	for (int i = 0; i < points.size(); i++)
	{
		ofJson curPoint;
		curPoint["id"] = i;
		curPoint["pos"]["x"] = points[i].getPosition().x;
		curPoint["pos"]["y"] = points[i].getPosition().y;
		for (auto parameter : points[i].getValues())
		{
			curPoint["parameters"][parameter.first] = parameter.second;
		}
		jSave["points"].push_back(curPoint);
	}
	jSave["MIDIMap"]["x"] = _CCXY[0];
	jSave["MIDIMap"]["y"] = _CCXY[1];
	return jSave;
}
