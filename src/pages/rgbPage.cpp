#include "rgbPage.h"

RGBPage::RGBPage()
{
	setUseGlobalParameters(false);
	setMidiOutput(true);
	setOscOutput(false);
	setStringOutput(false);
	setAddress("");
}

void RGBPage::setup(string name, int w, int h, int guiWidth, ofTrueTypeFont font, int maxMessages)
{
	_radius = 0.05;
	_map.setup(w, h);
	_map.setActive(false);
	_map.setRandomize(0.);
	_map.setDrawSelected(true);
	_map.setRadius(_radius);
	vector<ofVec2f> initialCursors = { ofVec2f(0,0) };
	_map.setCursors(initialCursors);
	
	MapPage::setup(name, w, h, guiWidth, font, maxMessages);
	setupGui();
}

void RGBPage::setupGui()
{
	_gui->addToggle("randomize");
	_gui->addSlider("Radius", 0., 1., _radius)->setName("Radius");
	_gui->addBreak();
	_gui->addLabel("Parameters")->setName("Parameters");
	_gui->getLabel("Parameters")->setLabelAlignment(ofxDatGuiAlignment::CENTER);
	_gui->addToggle("Trigger");
	_gui->addSlider("x", 0., 1.)->setName("posX");
	_gui->addSlider("y", 0., 1.)->setName("posY");
	_gui->addSlider("Width", 0., 1.);
	_gui->addSlider("Height", 0., 1.);
	_gui->addBreak();
	_gui->addTextInput("add");
	_gui->addToggle("Learn parameters")->setName("parameterLearn");
	_gui->onButtonEvent(this, &RGBPage::buttonEvent);
	_gui->onToggleEvent(this, &RGBPage::toggleEvent);
	_gui->onSliderEvent(this, &RGBPage::sliderEvent);
	_gui->onTextInputEvent(this, &RGBPage::textInputEvent);
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

void RGBPage::buttonEvent(ofxDatGuiButtonEvent e)
{
	if (e.target->getName() == "clearMIDI")
	{
		clearMidiMap();
	}
}

void RGBPage::sliderEvent(ofxDatGuiSliderEvent e)
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
	else if (name == "Radius" || name == "posX" || name == "posY" || name == "Width" || name == "Height")
	{
			if (name == "Radius")
			{
				_radius = e.value;
				_map.setRadius(_radius);
			}
		if (_map.getLastSelected() >= 0)
		{
			if (name == "posX")
			{
				float x = e.value;
				float y = _gui->getSlider("posY")->getValue();
				_map.movePoint(_map.getLastSelected(), ofVec2f(x, y));
			}
			if (name == "posY")
			{
				float x = _gui->getSlider("posX")->getValue();
				float y = e.value;
				_map.movePoint(_map.getLastSelected(), ofVec2f(x, y));
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
	}
	else
	{
		int lastSelected = _map.getLastSelected();
		int channel = ofToInt(ofSplitString(name, "/")[0]);
		int control = ofToInt(ofSplitString(name, "/")[1]);
		float value = e.value;
		if (lastSelected != -1) _map.setPointParameter(lastSelected, name, value);
		map<string, float> message;
		message[name] = value;
		addMessages(message, _MIDIOutMessages);
		addMessages(message, _MIDIDumpMessages);
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
	if (e.target->getName() == "Trigger")
	{
		if (_map.getLastSelected() != -1)
		{
			_map.setTrigger(_map.getLastSelected(), e.checked);
		}
	}
}

void RGBPage::textInputEvent(ofxDatGuiTextInputEvent e)
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
		if (isNumber)  MIDIIn("text_input", ofToInt(split[0]), ofToInt(split[1]), 0);
	}
	e.target->setText("");
	_parameterLearn = prevLearn;
}

void RGBPage::updateSelected(int selected, RGBPoint point)
{
	_gui->clearRemovableSliders();
	for (auto value : point.getValues())
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
	_gui->getToggle("Trigger")->setChecked(point.getTrigger());
	_gui->getSlider("posX")->setValue(point.getPosition().x, false);
	_gui->getSlider("posY")->setValue(point.getPosition().y, false);
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
		if(_lastSelectedPoint >= 0) _map.movePoint(_lastSelectedPoint, normPosition);
	}
}

void RGBPage::mousePressed(int x, int y, int button, bool doubleClick)
{
	_inside = _position.inside(x, y);
	ofRectangle guiPosition(_gui->getPosition(), _gui->getWidth(), _gui->getHeight());
	bool insideGui = guiPosition.inside(x, y);
	if (_inside && !insideGui)
	{
		ofVec2f pos = normalize(ofVec2f(x, y), _position);
		if (doubleClick)
		{
			ofFileDialogResult openFile = ofSystemLoadDialog("load image", false);
			ofFile file = openFile.getPath();
			if (openFile.bSuccess)
			{
				if (ofToLower(file.getExtension()) == "png")
				{
					ofImage img;
					if (img.load(openFile.filePath)) _map.addPoint(pos, img, openFile.filePath);
				}
			}
		}
		if (button < 2)
		{
			int lastSelected = _map.getLastSelected();
			array<float, 2> selection = _map.getClosest(pos, false);
			if (selection[1] < _minClickDistance)
			{
				_lastSelectedPoint = int(selection[0]);
				_map.setLastSelected(_lastSelectedPoint, ofGetElapsedTimeMillis());
				if (int(selection[0]) != lastSelected && _map.getPoints().size() > 0)
				{
					RGBPoint point = _map.getPoint(int(selection[0]));
					updateSelected(int(selection[0]), point);
				}
			}
			else _lastSelectedPoint = -1;
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
	else if (button == 1)
	{
		RGBPoint point = _map.getPoint(_map.getLastSelected());
		updateSelected(_map.getLastSelected(), point);
	}
	_lastSelectedPoint = -1;
}

void RGBPage::mouseScrolled(int scroll)
{
	_gui->scroll(scroll);
}


void RGBPage::load(ofJson & json)
{
	//clear current map
	_map.clearPoints();
	_gui->clearRemovableSliders();
	//load parameters to map and GUI
	for (ofJson curPoint : json["points"])
	{
		
		ofVec2f position = ofVec2f(curPoint["pos"]["x"], curPoint["pos"]["y"]);
		bool isTrigger = curPoint["trigger"].get<bool>();
		ofImage img;
		bool imageLoaded = img.load(curPoint["image_path"].get<string>());
		if (!imageLoaded)
		{
			img.allocate(100, 100, ofImageType::OF_IMAGE_GRAYSCALE);
			img.setColor(200);
		}
		int index = _map.addPoint(position, img, curPoint["image_path"].get<string>());
		_map.setTrigger(index, isTrigger);
		_map.resizePoint(index, curPoint["width"], curPoint["height"]);
		if (curPoint.find("parameters") != curPoint.end())
		{
			auto obj = curPoint["parameters"].get<ofJson::object_t>();
			for (auto parameter : obj) _map.addPointParameter(index, parameter.first, parameter.second);
		}
	}
	_radius = json["radius"];
	_map.setRadius(_radius);

	if (_map.getPoints().size() != 0)
	{
		_map.setLastSelected(0, ofGetElapsedTimeMillis());
		RGBPoint point = _map.getPoint(0);
		updateSelected(0, point);
	}

	loadMidiMap(json);
	MapPage::load(json);
}

ofJson RGBPage::save()
{
	ofJson jSave = MapPage::save();
	map<string, float> parameters = _map.getParameters();
	for (auto element : parameters) jSave["parameters"].push_back(element.first);
	jSave["radius"] = _radius;
	vector<RGBPoint> points = _map.getPoints();
	for (int i = 0; i < points.size(); i++)
	{
		ofJson curPoint;
		curPoint["id"] = i;
		curPoint["pos"]["x"] = points[i].getPosition().x;
		curPoint["pos"]["y"] = points[i].getPosition().y;
		curPoint["width"] = points[i].getWidth();
		curPoint["height"] = points[i].getHeight();
		curPoint["trigger"] = points[i].getTrigger();
		curPoint["image_path"] = points[i].getImagePath();
		for (auto parameter : points[i].getValues())
		{
			curPoint["parameters"][parameter.first] = parameter.second;
		}
		jSave["points"].push_back(curPoint);
	}
	
	saveMidiMap(jSave);
	return jSave;
}
