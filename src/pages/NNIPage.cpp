#include "NNIpage.h"

NNIPage::NNIPage()
{
	setUseGlobalParameters(true);
	setMidiOutput(true);
	setOscOutput(false);
	setStringOutput(false);
	setAddress("");
}

void NNIPage::setup(string name, int w, int h, int guiWidth, ofTrueTypeFont font, int maxMessages)
{
	_map.setup(w, h);
	_map.setActive(false);
	_map.setRandomize(0.);
	_map.setDrawInterpolation(true);
	_map.setDrawSelected(true);
	
	MapPage::setup(name, w, h, guiWidth, font, maxMessages);
	setupAnalysis();
	setupGui();
}

void NNIPage::setupGui()
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
	_gui->addTextInput("add");
	_gui->addToggle("Learn Parameters")->setName("parameterLearn");
	_gui->onButtonEvent(this, &NNIPage::buttonEvent);
	_gui->onToggleEvent(this, &NNIPage::toggleEvent);
	_gui->onSliderEvent(this, &NNIPage::sliderEvent);
	_gui->onTextInputEvent(this, &NNIPage::textInputEvent);
	_gui->setAutoDraw(false);
	_gui->setOpacity(0.5);
	_gui->setTheme(new ofxDatGuiThemeWireframe(), true);
	_gui->setWidth(_guiWidth, 0.3);
	_gui->setPosition(_position.x + _position.getWidth(), 0);
	_gui->setMaxHeight(ofGetHeight());
	_gui->setEnabled(false);
	_gui->setVisible(false);
	_gui->update();

	_sortGui->onButtonEvent(this, &NNIPage::buttonEvent);
	_sortGui->onDropdownEvent(this, &NNIPage::dropDownEvent);

	_gui->getToggle("active")->setBorder(_borderColor, 0);
	_controlFolder->setBorder(_borderColor, 0);
	_arrangeFolder->setBorder(_borderColor, 0);
	_gui->getLabel("Parameters")->setBorder(_borderColor, 0);
	_gui->getTextInput("add")->setBorder(_borderColor, 0);
	_gui->getToggle("parameterLearn")->setBorder(_borderColor, 0);
}

void NNIPage::setupAnalysis()
{
	//_dr.setup("../../ML/dr/mnt_analysis.py", "nni", "python"); //py
	_dr.setup("../ML/dr/mnt_analysis.exe", "nni"); //exe
	map<string, float> drParameters;
	drParameters["--perplexity"] = 5;
	drParameters["--learning_rate"] = 15;
	drParameters["--iterations"] = 1000;
	drParameters["--script"] = 1;
	_dr.setParameters(drParameters);
}

void NNIPage::update()
{
	if (_dr.getRunning())
	{
		if (_dr.getCompleted()) load(_dr.getData());
		else _dr.check();
	}

	MapPage::update();
}

void NNIPage::buttonEvent(ofxDatGuiButtonEvent e)
{
	if (e.target->getName() == "analyze")
	{
		if (_map.getPoints().size() <= _dr.getParameter("--perplexity") + 3)
		{
			_map.generatePoints(_dr.getParameter("--perplexity") - _map.getPoints().size() + 3);
		}
		if (!_dr.getRunning()) _dr.start(save());
	}
	else if (e.target->getName() == "clearMIDI")
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

void NNIPage::sliderEvent(ofxDatGuiSliderEvent e)
{
	string name = e.target->getName();

	if (name == "perplexity" || name == "learning rate" || name == "iterations")
	{
		_dr.setParameter(name, e.value);
	}
	else if (name == "x" || name == "y")
	{
		_lastControl = "slider/" + name;
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
		int lastSelected = _map.getLastSelected();
		int channel = ofToInt(ofSplitString(name, "/")[0]);
		int control = ofToInt(ofSplitString(name, "/")[1]);
		float value = e.value;
		_map.setGlobalParameter(name, value);
		if (lastSelected != -1) _map.setPointParameter(lastSelected, name, value);
		pair<string, float> message;
		message.first = name;
		message.second = value;
		addMessages(message, _MIDIDumpMessages);
		addMessages(message, _MIDIOutMessages);
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
	if (e.target->getName() == "showSortGui") _showSortGui = e.checked;
}

void NNIPage::textInputEvent(ofxDatGuiTextInputEvent e)
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

void NNIPage::dropDownEvent(ofxDatGuiDropdownEvent e)
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

void NNIPage::updateSelected(int selected, Point point)
{
	map<string, float> parameters = point.getParameters();
	_gui->getLabel("Parameters")->setLabel("Parameters: " + ofToString(selected));
	for (auto parameter : parameters) _gui->getSlider(parameter.first)->setValue(parameter.second, false);
	vector<pair<string, float>> curMessage;
	curMessage.assign(parameters.begin(), parameters.end());
	addMessages(curMessage, _MIDIOutMessages);
	addMessages(curMessage, _MIDIDumpMessages);
}

void NNIPage::mouseMoved(int x, int y)
{
	if (!_showSortGui)
	{
		if (_visible)
		{
			_inside = _position.inside(x, y);
			if (_inside && _mouseControl) _map.setCursor(normalize(ofVec2f(x, y), _position));
		}
	}
}

void NNIPage::mouseDragged(int x, int y, int button)
{
	if (!_showSortGui && !_map.getActive())
	{
		ofRectangle guiPosition(_gui->getPosition(), _gui->getWidth(), _gui->getHeight());
		bool insideGui = guiPosition.inside(x, y);
		_inside = _position.inside(x, y);
		if (button < 2 && _inside && !insideGui) {
			ofVec2f normPosition = normalize(ofVec2f(x, y), _position);
			if (_lastSelectedPoint >= 0)	_map.movePoint(_lastSelectedPoint, normPosition);
		}
	}
}

void NNIPage::mousePressed(int x, int y, int button, bool doubleClick)
{
	if (!_showSortGui && !_map.getActive())
	{
		_inside = _position.inside(x, y);
		ofRectangle guiPosition(_gui->getPosition(), _gui->getWidth(), _gui->getHeight());
		bool insideGui = guiPosition.inside(x, y);
		if (_inside && !insideGui)
		{
			ofVec2f pos = normalize(ofVec2f(x, y), _position);
			if (doubleClick) _map.addPoint(pos);
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
						Point point = _map.getPoint(int(selection[0]));
						updateSelected(int(selection[0]), point);
					}
				}
				else _lastSelectedPoint = -1;
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
		if (_inside && !_showSortGui && !_map.getActive())
		{
			_map.removePoint(normalized);
			if (_map.getPoints().size() > 0)
			{
				int curPoint = _map.getPoints().size() - 1;
				if (curPoint >= 0 && _map.getPoints().size() > 0)
				{
					_map.setLastSelected(curPoint);
					updateSelected(curPoint, _map.getPoint(curPoint));
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
				_map.removeGlobalParameter(removableSlider);
				_gui->removeSlider(removableSlider);
				_gui->setPosition(_gui->getPosition().x, _gui->getPosition().y);
				_gui->update();
			}
		}
		_lastSelectedPoint = -1;
	}
	else if(button == 0)
	{
		if (_selSortParameter.first || _selSortParameter.second)
		{
			_gui->update();
			_gui->updatePositions();
			string removableSlider = _gui->inside(x, y);
			if (removableSlider != "")
			{
				pair<string, string> curFeatures = _map.getSelectedFeatures();
				map<string, float> parameters = _map.getParameters();
				if (parameters.find(removableSlider) != parameters.end())
				{
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
			}
			_selSortParameter = { false, false };
		}
	}	
}

void NNIPage::mouseScrolled(int scroll)
{
	_gui->scroll(scroll);
}

void NNIPage::load(ofJson& json)
{
	if (json.find("error") == json.end())
	{
		//clear current map
		_map.clearPoints();
		_map.clearGlobalParameters();
		_gui->clearRemovableSliders();

		bool prevFeatures = _map.getFeatures().size() > 0;
		bool curFeatures = json.find("features") != json.end();
		vector<string> features;
		//load parameters to NNI and GUI
		for (auto parameter : json["parameters"])
		{
			_map.addGlobalParameter(parameter, 0);
			//GUI
			string sliderLabel = "ch" + ofSplitString(parameter, "/")[0] + "/cc" + ofSplitString(parameter, "/")[1];
			_gui->addSlider(sliderLabel, 0., 1.);
			_gui->getSlider(sliderLabel)->setName(parameter);
			_gui->setRemovableSlider(parameter);
			_gui->getSlider(parameter)->setTheme(new ofxDatGuiThemeWireframe());
			_gui->setWidth(_guiWidth, 0.3);
			_gui->setOpacity(0.5);
		}
		//features
		if (curFeatures)
		{
			for (auto& feature : json["features"]) features.push_back(feature);
			_map.setFeatures(features);
		}
		//load points
		for (ofJson point : json["points"])
		{
			Point curPoint;
			for (auto& parameter : _map.getParameters())
			{
				curPoint.setParameter(parameter.first, point["parameters"][parameter.first]);
			}
			if (curFeatures)
			{
				for (auto& feature : _map.getFeatures())
				{
					curPoint.setFeature(feature, point["features"][feature]);
				}
			}

			curPoint.setPosition(point["pos"]["x"], point["pos"]["y"]);
			_map.addPoint(curPoint);
		}
		//clear feature selection gui
		if (prevFeatures) 
		{
			_sortGui->removeComponent(_sortGui->getDropdown("sort-x"));
			_sortGui->removeComponent(_sortGui->getDropdown("sort-y"));
			_sortGui->removeComponent(_sortGui->getButton("closeSortGui"));
			_sortGui->update();
		}
		
		//add parameters to gui
		if (curFeatures)
		{
			_map.selectFeatures(json["selected"][0], json["selected"][1]);
			vector<string> guiFeatures;
			for(auto& feature : _map.getFeatures())
			{
				if(ofSplitString(feature, "/").size() != 2) guiFeatures.push_back(feature);
			}
			guiFeatures.push_back(_selSortParameterLabel);
			_sortGui->addDropdown("x", guiFeatures)->setName("sort-x");
			_sortGui->addDropdown("y", guiFeatures)->setName("sort-y");
			_sortGui->addButton("close")->setName("closeSortGui");
			_sortGui->setTheme(new ofxDatGuiThemeWireframe(), true);
			_sortGui->getDropdown("sort-x")->setLabel("x:" + _map.getSelectedFeatures().first);
			_sortGui->getDropdown("sort-y")->setLabel("y:" + _map.getSelectedFeatures().second);
			_sortGui->update();
		}
		//init
		if (_map.getPoints().size() != 0)
		{
			_map.setLastSelected(0, ofGetElapsedTimeMillis());
			updateSelected(0, _map.getPoint(_map.getPoints().size() - 1));
		}

		loadMidiMap(json);
		MapPage::load(json);
	}
}

ofJson NNIPage::save()
{
	ofJson jSave = MapPage::save();
	for (auto& element : _map.getParameters()) jSave["parameters"].push_back(element.first);
	for (auto &element : _map.getFeatures()) jSave["features"].push_back(element);
	jSave["selected"] = { _map.getSelectedFeatures().first, _map.getSelectedFeatures().second };
	vector<Point> points = _map.getPoints();
	for (int i = 0; i < points.size(); i++)
	{
		ofJson curPoint;
		curPoint["id"] = i;
		curPoint["pos"]["x"] = points[i].getPosition().x;
		curPoint["pos"]["y"] = points[i].getPosition().y;
		for (auto& parameter : points[i].getParameters())
		{
			curPoint["parameters"][parameter.first] = parameter.second;
		}
		for (auto& feature : points[i].getFeatures())
		{
			curPoint["features"][feature.first] = feature.second;
		}
		jSave["points"].push_back(curPoint);
	}

	saveMidiMap(jSave);
	return jSave;
}
