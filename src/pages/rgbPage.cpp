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
	_radius = 0.01;
	_map.setup(w, h);
	_map.setActive(false);
	_map.setRandomize(0.);
	_map.setDrawSelected(true);
	_map.setRadius(_radius);
	vector<ofVec2f> initialCursors = { ofVec2f(0,0) };
	_map.setCursors(initialCursors);
	
	MapPage::setup(name, w, h, guiWidth, font, maxMessages);
	setupAnalysis();
	setupGui();
}

void RGBPage::setupGui()
{
	_controlFolder->addSlider("Radius", 0., 1., _radius)->setName("Radius");
	/*
	_arrangeFolder->addSlider("perplexity", 5, 50, _dr.getParameter("--perplexity"))->setName("--perplexity");
	_arrangeFolder->addSlider("learning rate", 10, 1000, _dr.getParameter("--learning_rate"))->setName("--learning_rate");
	_arrangeFolder->addSlider("iterations", 250, 2500, _dr.getParameter("--iterations"))->setName("--iterations");
	*/
	_gui->addLabel("Parameters")->setName("Parameters");
	_gui->getLabel("Parameters")->setLabelAlignment(ofxDatGuiAlignment::CENTER);
	_settingsFolder = _gui->addFolder("settings");
	_settingsFolder->addToggle("Trigger");
	_settingsFolder->addSlider("x", 0., 1.)->setName("posX");
	_settingsFolder->addSlider("y", 0., 1.)->setName("posY");
	_settingsFolder->addSlider("Width", 0., 1.);
	_settingsFolder->addSlider("Height", 0., 1.);
	_gui->addTextInput("add");
	_gui->addToggle("Learn parameters")->setName("parameterLearn");
	_gui->addButton("Random")->setName("randomParameters");
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

	_gui->update();
	_sortGui->onButtonEvent(this, &RGBPage::buttonEvent);
	_sortGui->onDropdownEvent(this, &RGBPage::dropDownEvent);

	_gui->getToggle("active")->setBorder(_borderColor, 0);
	_controlFolder->setBorder(_borderColor, 0);
	_arrangeFolder->setBorder(_borderColor, 0);
	_settingsFolder->setBorder(_borderColor, 0);
	_gui->getLabel("Parameters")->setBorder(_borderColor, 0);
	_gui->getTextInput("add")->setBorder(_borderColor, 0);
	_gui->getToggle("parameterLearn")->setBorder(_borderColor, 0);
	_gui->getButton("randomParameters")->setBorder(_borderColor, 0);
	_gui->update();
}

void RGBPage::setupAnalysis()
{
	_dr.setup("../../ML/dr/mnt_analysis.py", "rgb", "python"); //py
	//_dr.setup("../ML/dr/mnt_analysis.exe", "rgb"); //exe
	map<string, float> drParameters;
	drParameters["--perplexity"] = 5;
	drParameters["--learning_rate"] = 15;
	drParameters["--iterations"] = 1000;
	drParameters["--script"] = 1;
	_dr.setParameters(drParameters);
}

void RGBPage::update()
{
	if (_dr.getRunning())
	{
		if (_dr.getCompleted()) load(_dr.getData());
		else _dr.check();
	}

	MapPage::update();
}

void RGBPage::buttonEvent(ofxDatGuiButtonEvent e)
{
	if (e.target->getName() == "analyze")
	{
		if (!_dr.getRunning()) _dr.start(save());
	}
	if (e.target->getName() == "clearMIDI")
	{
		clearMidiMap();
	}
	else if (e.target->getName() == "closeSortGui")
	{
		_showSortGui = false;
		_gui->getToggle("showSortGui")->setChecked(false);
	}
	else if (e.target->getName() == "randomParameters")
	{
		if (_map.getPoints().size() > 0)
		{
			map<string, float> curParameters = _map.getPoint(_lastSelectedPoint).getParameters();
			for (auto parameter : curParameters)
			{
				float value = abs(ofRandomf());
				_gui->getSlider(parameter.first)->setValue(value);
			}
		}
	}
}

void RGBPage::sliderEvent(ofxDatGuiSliderEvent e)
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
		pair<string, float> message;
		message.first = name;
		message.second = value;
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
	if (e.target->getName() == "showSortGui") _showSortGui = e.checked;
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

void RGBPage::dropDownEvent(ofxDatGuiDropdownEvent e)
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

void RGBPage::updateSelected(int selected, RGBPoint point)
{
	_gui->clearRemovableSliders();
	for (auto value : point.getParameters())
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
	if (!_showSortGui)
	{
		if (_visible)
		{
			_inside = _position.inside(x, y);
			if (_inside && _mouseControl) _map.setCursor(normalize(ofVec2f(x, y), _position), 0);
		}
	}
}

void RGBPage::mouseDragged(int x, int y, int button)
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

void RGBPage::mousePressed(int x, int y, int button, bool doubleClick)
{
	if (!_showSortGui & !_map.getActive())
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
}

void RGBPage::mouseReleased(int x, int y, int button)
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
		_lastSelectedPoint = -1;
	}
	else if (button == 1)
	{
		if (!_showSortGui)
		{
			RGBPoint point = _map.getPoint(_map.getLastSelected());
			updateSelected(_map.getLastSelected(), point);
		}
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

void RGBPage::mouseScrolled(int scroll)
{
	_gui->scroll(scroll);
}

void RGBPage::load(ofJson & json)
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
		//load points (bug when using addPoint(RGBPoint point) ????)
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
			if (curFeatures)
			{
				for (auto& feature : _map.getFeatures()) _map.addPointFeature(index, feature, curPoint["features"][feature]);
			}
		}
		_radius = json["radius"];
		_map.setRadius(_radius);
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
}

ofJson RGBPage::save()
{
	ofJson jSave = MapPage::save();
	map<string, float> parameters = _map.getParameters();

	for (auto& feature : _map.getFeatures()) jSave["features"].push_back(feature);
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