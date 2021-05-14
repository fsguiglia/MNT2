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
	setupGui(_map.getParameters(), false);
	_maxMessages = maxMessages;
}

void NNIPage::setupGui(map<string, float> parameters, bool toggleState)
{
	_gui = new ScrollGui();
	_gui->addHeader("NNI", false);
	_gui->addToggle("interpolate");
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
	_gui->onToggleEvent(this, &NNIPage::toggleEvent);
	for (auto parameter : parameters) _gui->addSlider(parameter.first, parameter.second, 0., 1.);
	_gui->onSliderEvent(this, &NNIPage::sliderEvent);
	_gui->setAutoDraw(false);
	_gui->setOpacity(0.5);
	_gui->setTheme(new ofxDatGuiThemeWireframe(), true);
	_gui->setWidth(_guiWidth, 0.3);
	_gui->setPosition(ofGetWidth() - _guiWidth, 0);
	_gui->setMaxHeight(ofGetHeight());
	_gui->update();
}

void NNIPage::update()
{
	_map.update();
	if (_map.getActive())
	{
		if (_map.getWeights() != previousWeights)
		{
			addMidiMessages(_map.getWeights(), _MIDIOutMessages);
			previousWeights = _map.getWeights();
		}
	}
	_gui->setVisible(_visible);
	_gui->setEnabled(_visible);
	_gui->update();
}

void NNIPage::setVisible(bool visible)
{
	_visible = visible;
	_inside = visible;
}

void NNIPage::draw(ofTrueTypeFont& font)
{
	ofPushStyle();
	_map.draw(_position.x, _position.y, _position.getWidth(), _position.getHeight(), font);
	_gui->draw();
	ofPopStyle();
}



void NNIPage::sliderEvent(ofxDatGuiSliderEvent e)
{
	string name = e.target->getName();
	if (name == "x" || name == "y")
	{
		_lastSelected = name;
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
		_map.setParameter(name, value);
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
	if (e.target->getName() == "interpolate") _map.setActive(e.checked);
	if (e.target->getName() == "randomize") _map.setRandomize(float(e.checked));
	if (e.target->getName() == "Mouse Control") _mouseControl = e.checked;
}

void NNIPage::updateNNISite(int selected, map<string, float> parameters)
{
	_gui->getLabel("Parameters")->setLabel("Parameters - Site: " + ofToString(selected));
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
		if (button == 0) _map.addSite(pos);
		if (button < 2)
		{
			int lastSelected = _map.getLastSelected();
			int curSelected = _map.getClosest(pos, true)[0];
			if (curSelected != lastSelected)
			{
				updateNNISite(curSelected, _map.getPoint(curSelected).getValues());
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
				if (curPoint > 0)
				{
					_map.setLastSelected(curPoint);
					updateNNISite(curPoint, _map.getPoint(curPoint).getValues());
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

void NNIPage::mouseScrolled(int scroll)
{
	_gui->scroll(scroll);
}

void NNIPage::resize(int w, int h)
{
	_position = centerSquarePosition(w - _guiWidth, h);
	_gui->setPosition(w - _guiWidth, 0);
	_gui->setMaxHeight(h);
	_gui->update();
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
		if (_lastSelected == "x")
		{
			_CCXY[0] = parameter;
			_gui->getSlider("x")->setLabel("x:cc" + control);
		}
		if (_lastSelected == "y")
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

void NNIPage::load(ofJson& json)
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
		_map.addSite(ofVec2f(site["pos"]["x"], site["pos"]["y"]));
	}
	//update gui
	_gui->update();
	if (_map.getPoints().size() != 0)
	{
		updateNNISite(0, _map.getPoint(0).getValues());
	}
}

ofJson NNIPage::save()
{
	ofJson jSave;
	map<string, float> nniParameters = _map.getParameters();
	for (auto element : nniParameters) jSave["parameters"].push_back(element.first);
	vector<Point> nniSites = _map.getPoints();
	for (int i = 0; i < nniSites.size(); i++)
	{
		ofJson curSite;
		curSite["id"] = i;
		curSite["pos"]["x"] = nniSites[i].getPosition().x;
		curSite["pos"]["y"] = nniSites[i].getPosition().y;
		for (auto parameter : nniSites[i].getValues())
		{
			curSite["parameters"][parameter.first] = parameter.second;
		}
		jSave["sites"].push_back(curSite);
	}
	return jSave;
}
