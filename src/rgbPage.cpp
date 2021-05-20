#include "rgbPage.h"

RGBPage::RGBPage()
{
}

void RGBPage::setup(int width, int height, int guiWidth, int maxMessages)
{
	_radius = 0.05;
	_map.setup(width, height);
	_guiWidth = guiWidth;
	_position = centerSquarePosition(ofGetWidth() - _guiWidth, ofGetHeight());
	_map.setActive(false);
	_map.setRandomize(0.);
	_map.setDrawSelected(true);
	_map.setRadius(_radius);
	_mouseControl = false;
	_inside = false;
	_visible = false;
	setupGui(_map.getParameters(), false);
	_maxMessages = maxMessages;
	vector<ofVec2f> initialCursors = { ofVec2f(0,0) };
	_map.setCursors(initialCursors);
}

void RGBPage::setupGui(map<string, float> parameters, bool toggleState)
{
	_gui = new ScrollGui();
	_gui->addHeader("Draw", false);
	_gui->addToggle("active");
	_gui->addToggle("randomize");
	_gui->addBreak();
	_gui->addLabel("Control")->setLabelAlignment(ofxDatGuiAlignment::CENTER);
	_gui->addToggle("learn", toggleState)->setName("controlLearn");
	_gui->addToggle("Mouse Control");
	_gui->addSlider("x", 0., 1.)->setName("x");
	_gui->addSlider("y", 0., 1.)->setName("y");
	_gui->addSlider("Radius", 0., 1., _radius);
	_gui->addBreak();
	_gui->addLabel("Parameters")->setName("Parameters");
	_gui->getLabel("Parameters")->setLabelAlignment(ofxDatGuiAlignment::CENTER);
	_gui->addToggle("Trigger");
	_gui->addSlider("Width", 0., 1.);
	_gui->addSlider("Height", 0., 1.);
	_gui->addToggle("learn", toggleState)->setName("parameterLearn");
	_gui->addBreak();
	_gui->onToggleEvent(this, &RGBPage::toggleEvent);
	_gui->onSliderEvent(this, &RGBPage::sliderEvent);
	_gui->setAutoDraw(false);
	_gui->setOpacity(0.5);
	_gui->setTheme(new ofxDatGuiThemeWireframe(), true);
	_gui->setWidth(_guiWidth, 0.3);
	_gui->setPosition(ofGetWidth() - _guiWidth, 0);
	_gui->setMaxHeight(ofGetHeight());
	_gui->update();
}

void RGBPage::update()
{
	_map.update();
	if (_map.getActive())
	{
		if (_map.getOutput() != _previousOutput)
		{
			addMidiMessages(_map.getOutput(), _MIDIOutMessages);
			_previousOutput = _map.getOutput();
		}
	}
	_gui->setVisible(_visible);
	_gui->setEnabled(_visible);
	_gui->update();
}

void RGBPage::draw(ofTrueTypeFont & font)
{
	ofPushStyle();
	_map.draw(_position.x, _position.y, _position.getWidth(), _position.getHeight(), font);
	_gui->draw();
	ofPopStyle();
}

void RGBPage::sliderEvent(ofxDatGuiSliderEvent e)
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
	else if (name == "Radius" || name == "Width" || name == "Height")
	{
		if (name == "Radius")
		{
			_radius = e.value;
			_map.setRadius(_radius);
		}
		if (name == "Width") {
			int w = e.value * _map.getWidth();
			int h = _gui->getSlider("Height")->getValue() * _map.getHeight();
			_map.resizePoint(_map.getLastSelected(), w, h);
		}
		if (name == "Height") {
			int w = _gui->getSlider("Width")->getValue() * _map.getWidth();
			int h = e.value * _map.getHeight();
			_map.resizePoint(_map.getLastSelected(), w, h);
		}
	}
	else
	{
		int lastSelected = _map.getLastSelected();
		int channel = ofToInt(ofSplitString(name, "/")[1]);
		int control = ofToInt(ofSplitString(name, "/")[2]);
		float value = e.value;
		if (lastSelected != -1) _map.setPointParameter(lastSelected, name, value);
		map<string, float> message;
		message[name] = value;
		addMidiMessages(message, _MIDIDumpMessages);
	}
}

void RGBPage::toggleEvent(ofxDatGuiToggleEvent e)
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
	if (e.target->getName() == "Trigger")
	{
		if (_map.getLastSelected() != -1)
		{
			_map.setTrigger(_map.getLastSelected(), e.checked);
		}
	}
}

void RGBPage::updateSelected(int selected, RGBPoint point)
{
	_gui->clearRemovableSliders();
	for (auto value : point.getValues())
	{
		vector<string> split = ofSplitString(value.first, "/");
		string sliderLabel = "cc" + split[split.size() - 1];
		_gui->addSlider(sliderLabel, 0., 1., value.second);
		_gui->getSlider(sliderLabel)->setName(value.first);
		_gui->setRemovableSlider(value.first);
		_gui->getSlider(value.first)->setTheme(new ofxDatGuiThemeWireframe());
		_gui->setWidth(300, 0.3);
		_gui->setOpacity(0.5);
	}
	_gui->getLabel("Parameters")->setLabel("Parameters: " + ofToString(selected));
	_gui->getToggle("Trigger")->setChecked(point.getTrigger());
	_gui->getSlider("Width")->setValue((float)point.getWidth() / _map.getWidth(), false);
	_gui->getSlider("Height")->setValue((float)point.getHeight() / _map.getHeight(), false);
	_gui->update();
}

void RGBPage::mouseMoved(int x, int y)
{
	if (_visible)
	{
		_inside = _position.inside(x, y);
		if (_inside && _mouseControl) _map.setCursor(normalize(ofVec2f(x, y), _position), 0);
	}
}

void RGBPage::mouseDragged(int x, int y, int button)
{
	ofRectangle guiPosition(_gui->getPosition(), _gui->getWidth(), _gui->getHeight());
	bool insideGui = guiPosition.inside(x, y);
	_inside = _position.inside(x, y);
	if (button < 2 && _inside && !insideGui) {
		ofVec2f normPosition = normalize(ofVec2f(x, y), _position);
		_map.movePoint(_map.getLastSelected(), normPosition);
	}
}

void RGBPage::mousePressed(int x, int y, int button)
{
	_inside = _position.inside(x, y);
	ofRectangle guiPosition(_gui->getPosition(), _gui->getWidth(), _gui->getHeight());
	bool insideGui = guiPosition.inside(x, y);
	if (_inside && !insideGui)
	{
		ofVec2f pos = normalize(ofVec2f(x, y), _position);
		if (button == 0)
		{
			ofFileDialogResult openFile = ofSystemLoadDialog("load image", false);
			if (openFile.bSuccess)
			{
				ofImage img;
				if(img.load(openFile.filePath)) _map.addPoint(pos, img);
			}
		}
		if (button < 2)
		{
			int lastSelected = _map.getLastSelected();
			int curSelected = _map.getClosest(pos, true)[0];
			if (curSelected != lastSelected)
			{
				RGBPoint point = _map.getPoint(curSelected);
				updateSelected(curSelected, point);
			}
		}
	}
}

void RGBPage::mouseReleased(int x, int y, int button)
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
					RGBPoint point = _map.getPoint(curSelected);
					updateSelected(curSelected, point);
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
				int lastSelected = _map.getLastSelected();
				if (lastSelected != -1) _map.removePointParameter(lastSelected, removableSlider);
				_gui->removeSlider(removableSlider);
				_gui->setPosition(_gui->getPosition().x, _gui->getPosition().y);
				_gui->update();
			}
		}
	}
}

void RGBPage::mouseScrolled(int scroll)
{
	_gui->scroll(scroll);
}

void RGBPage::resize(int w, int h)
{
	_position = centerSquarePosition(w - _guiWidth, h);
	_gui->setPosition(w - _guiWidth, 0);
	_gui->setMaxHeight(h);
	_gui->update();
}

void RGBPage::MIDIIn(string port, int control, int channel, float value)
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
		int lastSelected = _map.getLastSelected();
		if (lastSelected != -1)
		{
			if (!_map.getPoint(lastSelected).hasValue(parameter))
			{
				_map.addPointParameter(lastSelected, parameter, value);
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
				_map.setPointParameter(lastSelected, parameter, value);
			}
		}
	}
	else
	{
		if (parameter == _CCXY[0]) _gui->getSlider("x")->setValue(value);
		if (parameter == _CCXY[1]) _gui->getSlider("y")->setValue(value);
	}
}

void RGBPage::load(ofJson & json)
{
	//clear current map
	_map.clearPoints();
	_gui->clearRemovableSliders();
	//load parameters to map and GUI
	for (ofJson point : json["points"])
	{
		//_map.addPoint(ofVec2f(point["pos"]["x"], site["pos"]["y"]), point["radius"], point["threshold"]);
	}
	//update gui
	_gui->update();
	if (_map.getPoints().size() != 0)
	{
		RGBPoint point = _map.getPoint(0);
		updateSelected(0, point);
	}
}

ofJson RGBPage::save()
{
	ofJson jSave;
	map<string, float> parameters = _map.getParameters();
	for (auto element : parameters) jSave["parameters"].push_back(element.first);
	vector<RGBPoint> points = _map.getPoints();
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
	return jSave;
}