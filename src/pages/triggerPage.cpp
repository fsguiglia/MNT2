#include "triggerPage.h"

TriggerPage::TriggerPage()
{
	setUseGlobalParameters(false);
	setMidiOutput(true);
	setOscOutput(false);
	setStringOutput(false);
	setAddress("");
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
	setupAnalysis();
	setupGui();
}

void TriggerPage::setupGui()
{
	/*
	_arrangeFolder->addSlider("perplexity", 5, 50, _dr.getParameter("--perplexity"))->setName("--perplexity");
	_arrangeFolder->addSlider("learning rate", 10, 1000, _dr.getParameter("--learning_rate"))->setName("--learning_rate");
	_arrangeFolder->addSlider("iterations", 250, 2500, _dr.getParameter("--iterations"))->setName("--iterations");
	*/
	_arrangeFolder->addToggle("randomize");
	_arrangeFolder->addButton("generate");
	_gui->addLabel("Parameters")->setName("Parameters");
	_gui->getLabel("Parameters")->setLabelAlignment(ofxDatGuiAlignment::CENTER);
	_settingsFolder = _gui->addFolder("settings");
	_settingsFolder->addToggle("Switch");
	_settingsFolder->addSlider("Radius", 0., 1., _radius);
	_settingsFolder->addSlider("Threshold", 0., 1., _threshold);
	_settingsFolder->collapse();
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
	_gui->setMaxHeight(ofGetHeight());
	_gui->setVisible(false);
	_gui->setEnabled(false);
	_gui->update();

	_sortGui->onButtonEvent(this, &TriggerPage::buttonEvent);
	_sortGui->onDropdownEvent(this, &TriggerPage::dropDownEvent);

	_gui->getToggle("active")->setBorder(_borderColor, 0);
	_controlFolder->setBorder(_borderColor, 0);
	_arrangeFolder->setBorder(_borderColor, 0);
	_settingsFolder->setBorder(_borderColor, 0);
	_gui->getLabel("Parameters")->setBorder(_borderColor, 0);
	_gui->getTextInput("add")->setBorder(_borderColor, 0);
	_gui->getToggle("parameterLearn")->setBorder(_borderColor, 0);
	_gui->update();
}

void TriggerPage::setupAnalysis()
{
	//_dr.setup("../../ML/dr/mnt_analysis.py", "trg", "python"); //py
	_dr.setup("../ML/dr/mnt_analysis.exe", "trg"); //exe
	map<string, float> drParameters;
	drParameters["--perplexity"] = 5;
	drParameters["--learning_rate"] = 15;
	drParameters["--iterations"] = 1000;
	drParameters["--script"] = 1;
	_dr.setParameters(drParameters);
}

void TriggerPage::update()
{
	if (_dr.getRunning())
	{
		if (_dr.getCompleted()) load(_dr.getData());
		else _dr.check();
	}

	MapPage::update();
}

void TriggerPage::buttonEvent(ofxDatGuiButtonEvent e)
{
	if (e.target->getName() == "analyze")
	{
		if (_map.getPoints().size() <= _dr.getParameter("--perplexity") + 3)
		{
			_map.generatePoints(_dr.getParameter("--perplexity") - _map.getPoints().size() + 3);
		}
		if (!_dr.getRunning()) _dr.start(save());
	}
	if (e.target->getName() == "clearMIDI")
	{
		clearMidiMap();
	}
	else if (e.target->getName() == "generate")
	{
		_map.generatePoints(1);
	}
	else if (e.target->getName() == "closeSortGui")
	{
		_showSortGui = false;
		_gui->getToggle("showSortGui")->setChecked(false);
	}
}

void TriggerPage::sliderEvent(ofxDatGuiSliderEvent e)
{
	string name = e.target->getName();

	if (name == "perplexity" || name == "learning rate" || name == "iterations")
	{
		_dr.setParameter(name, e.value);
	}
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
	if (e.target->getName() == "showSortGui") _showSortGui = e.checked;
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

void TriggerPage::dropDownEvent(ofxDatGuiDropdownEvent e)
{
	string selected = e.target->getSelected()->getLabel();
	if (selected == _selSortParameterLabel)
	{
		if (e.target->getName() == "sort-x") _selSortParameter.first = !_selSortParameter.first;
		else if (e.target->getName() == "sort-y") _selSortParameter.second = !_selSortParameter.second;
	}
	else
	{
		pair<string, string> curFeatures = _map.getSelectedFeatures();
		if (e.target->getName() == "sort-x")
		{
			curFeatures.first = selected;
			selected = "x:" + selected;
		}
		else if (e.target->getName() == "sort-y")
		{
			curFeatures.second = selected;
			selected = "y:" + selected;
		}
		_selSortParameter = { false, false };
		e.target->setLabel(selected);
		_map.selectFeatures(curFeatures.first, curFeatures.second);
	}
}

void TriggerPage::updateSelected(int selected, Trigger trigger)
{
	_gui->clearRemovableSliders();
	for (auto& value : trigger.getParameters())
	{
		vector<string> split = ofSplitString(value.first, "/");
		string sliderLabel = "ch" + split[0] + "/cc" + split[1];
		ofxDatGuiSlider* slider = _gui->addSlider(sliderLabel, 0., 1., value.second);
		slider->setName(value.first);
		slider->setTheme(new ofxDatGuiThemeWireframe());
		_gui->setRemovableSlider(value.first);
	}
	_gui->getLabel("Parameters")->setLabel("Parameters: " + ofToString(selected));
	_gui->getToggle("Switch")->setChecked(_map.getPoint(selected).getSwitch());
	_gui->getSlider("Radius")->setValue(trigger.getRadius(), false);
	_gui->getSlider("Threshold")->setValue(trigger.getThreshold(), false);
	_gui->setWidth(_guiWidth, 0.3);
	_gui->setOpacity(0.5);
	_gui->update();
	//for (auto port : _MIDIOutputs) sendMIDICC(parameters, port.second);
}

void TriggerPage::mouseMoved(int x, int y)
{
	if (!_showSortGui)
	{
		if (_visible)
		{
			_inside = _position.inside(x, y);
			if (_inside && _mouseControl) _map.setCursor(normalize(ofVec2f(x, y), _position), 0);
		}
	}
}

void TriggerPage::mouseDragged(int x, int y, int button)
{
	if (!_showSortGui && !_map.getActive())
	{
		ofRectangle guiPosition(_gui->getPosition(), _gui->getWidth(), _gui->getHeight());
		bool insideGui = guiPosition.inside(x, y);
		_inside = _position.inside(x, y);
		if (button < 2 && _inside && !insideGui) {
			ofVec2f normPosition = normalize(ofVec2f(x, y), _position);
			if (_lastSelectedPoint >= 0) _map.movePoint(_lastSelectedPoint, normPosition);
		}
	}
}

void TriggerPage::mousePressed(int x, int y, int button, bool doubleClick)
{
	if (!_showSortGui && !_map.getActive())
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
}

void TriggerPage::mouseReleased(int x, int y, int button)
{
	ofVec2f normalized = normalize(ofVec2f(x, y), _position);
	_inside = _position.inside(x, y);
	if (button == 2)
	{
		if (_inside && !_showSortGui && !_map.getActive())
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
		_lastSelectedPoint = -1;
	}
	else if (button == 0)
	{
		if (_selSortParameter.first || _selSortParameter.second)
		{
			_gui->update();
			_gui->updatePositions();
			string removableSlider = _gui->inside(x, y);
			if (removableSlider != "")
			{
				pair<string, string> curFeatures = _map.getSelectedFeatures();
				if (_selSortParameter.first)
				{
					_sortGui->getDropdown("sort-x")->setLabel("x:" + removableSlider);
					_map.sortByParameter(0, removableSlider);
				}
				else if (_selSortParameter.second)
				{
					_sortGui->getDropdown("sort-y")->setLabel("y:" + removableSlider);
					_map.sortByParameter(1, removableSlider);
				}
			}
			_selSortParameter = { false, false };
		}
	}
}

void TriggerPage::mouseScrolled(int scroll)
{
	_gui->scroll(scroll);
}

void TriggerPage::load(ofJson& json)
{
	if (json.find("error") == json.end())
	{
		//clear current map
		_map.clearPoints();
		_gui->clearRemovableSliders();

		bool prevFeatures = _map.getFeatures().size() > 0;
		bool curFeatures = json.find("features") != json.end();
		vector<string> features;
		//features
		if (curFeatures)
		{
			for (auto& feature : json["features"]) features.push_back(feature);
			_map.setFeatures(features);
		}
		//load points
		for (ofJson point : json["points"])
		{
			Trigger curPoint;
			curPoint.setPosition(point["pos"]["x"], point["pos"]["y"]);
			curPoint.setRadius(point["radius"]);
			curPoint.setThreshold(point["threshold"]);
			curPoint.setSwitch(point["switch"]);
			if (point.find("parameters") != point.end())
			{
				auto obj = point["parameters"].get<ofJson::object_t>();
				for (auto parameter : obj) curPoint.setParameter(parameter.first, parameter.second);
			}
			if (curFeatures)
			{
				for (auto& feature : _map.getFeatures()) curPoint.setFeature(feature, point["features"][feature]);
			}
			_map.addPoint(curPoint);
		}
		//clear feature selection gui
		if (prevFeatures)
		{
			_sortGui->removeComponent(_sortGui->getDropdown("sort-x"));
			_sortGui->removeComponent(_sortGui->getDropdown("sort-y"));
		}
		_sortGui->removeComponent(_sortGui->getButton("closeSortGui"));
		//add parameters to gui
		if (curFeatures)
		{
			_map.selectFeatures(json["selected"][0], json["selected"][1]);
			vector<string> guiFeatures;
			for (auto& feature : _map.getFeatures())
			{
				if (ofSplitString(feature, "/").size() != 2) guiFeatures.push_back(feature);
			}
			guiFeatures.push_back(_selSortParameterLabel);
			_sortGui->addDropdown("x", guiFeatures)->setName("sort-x");
			_sortGui->addDropdown("y", guiFeatures)->setName("sort-y");
			_sortGui->setTheme(new ofxDatGuiThemeWireframe(), true);
			_sortGui->getDropdown("sort-x")->setLabel("x:" + _map.getSelectedFeatures().first);
			_sortGui->getDropdown("sort-y")->setLabel("y:" + _map.getSelectedFeatures().second);
			_sortGui->addButton("close")->setName("closeSortGui");
			_sortGui->setTheme(new ofxDatGuiThemeWireframe(), true);
			_sortGui->update();
		}
		//init
		if (_map.getPoints().size() != 0)
		{
			_map.setLastSelected(0, ofGetElapsedTimeMillis());
			Trigger point = _map.getPoint(0);
			updateSelected(0, point);
		}

		loadMidiMap(json);
		MapPage::load(json);
	}
}

ofJson TriggerPage::save()
{
	ofJson jSave = MapPage::save();
	map<string, float> parameters = _map.getParameters();
	
	for (auto &element : _map.getFeatures()) jSave["features"].push_back(element);
	jSave["selected"] = { _map.getSelectedFeatures().first, _map.getSelectedFeatures().second };
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
		
		map<string, float> curParameters = points[i].getParameters();
		for (auto parameter : curParameters)
		{
			curPoint["parameters"][parameter.first] = parameter.second;
		}
		for (auto& feature : points[i].getFeatures())
		{
			curPoint["features"][feature.first] = feature.second;
		}
		jSave["points"].push_back(curPoint);
		parameters.insert(curParameters.begin(), curParameters.end());
	}
	for (auto element : parameters) jSave["parameters"].push_back(element.first);
	saveMidiMap(jSave);
	return jSave;
}