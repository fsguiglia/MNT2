#include "CBCSPage.h"

CBCSPage::CBCSPage()
{
	setUseGlobalParameters(false);
	setMidiOutput(false);
	setOscOutput(true);
	setStringOutput(true);
	setAddress("/concatenate");
}

void CBCSPage::setup(string name, int w, int h, int guiWidth, ofTrueTypeFont font, int maxMessages)
{
	_map.setup(w, h);
	_map.setMaxSamples(15);
	_map.setRadius(0.05);
	_map.setActive(false);
	_map.setRandomize(0.);
	_map.setDrawSelected(true);
	_map.setCursor(ofVec2f(-1, -1));
	
	setAddress("/" + name);
	
	MapPage::setup(name, w, h, guiWidth, font, maxMessages);
	setupAnalysis();
	setupExport();
	setupGui();
}

void CBCSPage::setupGui()
{
	_arrangeFolder->addToggle("Analyze complete files", true)->setName("complete");
	_arrangeFolder->addSlider("unit length", 100, 1000, _dr.getParameter("--unit_length"))->setName("--unit_length");
	_arrangeFolder->addSlider("perplexity", 5, 50, _dr.getParameter("--perplexity"))->setName("--perplexity");
	_arrangeFolder->addSlider("learning rate", 10, 1000, _dr.getParameter("--learning_rate"))->setName("--learning_rate");
	_arrangeFolder->addSlider("iterations", 250, 2500, _dr.getParameter("--iterations"))->setName("--iterations");
	
	_gui->addSlider("radius", 0, 1, _map.getRadius() * 2);
	_gui->addSlider("max units", 1, 100, _map.getMaxSamples())->setName("units");
	_gui->getSlider("units")->setPrecision(0);
	_gui->addTextInput("address", getAddress());
	_gui->addBreak();
	_gui->addButton("Export single file")->setName("Export_file");
	_gui->addButton("Export file list")->setName("Export_list");
	_gui->onButtonEvent(this, &CBCSPage::buttonEvent);
	_gui->onToggleEvent(this, &CBCSPage::toggleEvent);
	_gui->onSliderEvent(this, &CBCSPage::sliderEvent);
	_gui->onTextInputEvent(this, &CBCSPage::textInputEvent);
	_gui->setAutoDraw(false);
	_gui->setOpacity(0.5);
	_gui->setTheme(new ofxDatGuiThemeWireframe(), true);
	_gui->setWidth(_guiWidth, 0.3);
	_gui->setPosition(_position.x + _position.getWidth(), 0);
	_gui->setMaxHeight(ofGetHeight());
	_gui->setEnabled(false);
	_gui->setVisible(false);

	_sortGui->onDropdownEvent(this, &CBCSPage::dropDownEvent);
}

void CBCSPage::setupAnalysis()
{
	_dr.setup("../../ML/dr/mnt_analysis.py", "cbcs", "python"); //py
	//_dr.setup("../ML/dr/mnt_analysis.exe", "cbcs"); //exe
	map<string, float> drParameters;
	drParameters["--perplexity"] = 30;
	drParameters["--learning_rate"] = 200;
	drParameters["--iterations"] = 1000;
	drParameters["--cbcs_mode"] = 1;
	drParameters["--unit_length"] = 500;
	drParameters["--script"] = 0;
	_dr.setParameters(drParameters);
}

void CBCSPage::setupExport()
{
	_export.setup("../../ML/dr/mnt_analysis.py", "export", "python"); //py
	//_dr.setup("../ML/dr/single_file.exe", "export"); //exe
	_export.setParameter("--unit_length", _dr.getParameter("--unit_length"));
	_export.setParameter("--script", 2);
}

void CBCSPage::update()
{
	if (_dr.getRunning())
	{
		if (_dr.getCompleted())
		{
			_map.clearPoints();
			loadData(_dr.getData());
			_export.setParameter("--unit_length", _dr.getParameter("--unit_length"));
		}
		else _dr.check();
	}

	if (_export.getRunning())
	{
		if (_export.getCompleted())
		{
			loadSingleFile(_export.getData());
		}
		else _export.check();
	}

	updateOutput();
}

void CBCSPage::updateOutput()
{
	_map.update();
	if (_map.getActive())
	{
		if (_map.getOutput() != _previousOutput)
		{
			vector<string> stringOutput;
			vector<pair<string, float>> oscOutput;
			vector<pair<string, float>> curOutput = _map.getOutput();
			string address = _address + "/single-file-position";
			for (auto& element : curOutput)
			{
				if (element.first == "single-file-position") oscOutput.push_back(make_pair(address, element.second));
				else stringOutput.push_back(element.first + ";" + ofToString(element.second));
			}
			if (_oscOutput) addMessages(oscOutput, _OSCOutMessages);
			if (_stringOutput) setStringMessages(stringOutput);
		}
		_previousOutput = _map.getOutput();
	}
	_gui->setVisible(_visible);
	_gui->setEnabled(_visible);
	_gui->update();

	_sortGui->setVisible(_visible && _showSortGui);
	_sortGui->setEnabled(_visible && _showSortGui);
	_sortGui->update();
}

void CBCSPage::buttonEvent(ofxDatGuiButtonEvent e)
{
	if (e.target->getName() == "analyze")	if (!_dr.getRunning() && !_export.getRunning()) _dr.start();
	else if (e.target->getName() == "Normalize")
	{
		_map.normalize();
	}
	else if (e.target->getName() == "Export_list")
	{
		exportFileList();
	}
	else if (e.target->getName() == "Export_file")
	{
		if (!_dr.getRunning() && !_export.getRunning())
		{
			if(_map.getPoints().size() > 0)	_export.start(getFileList());
		}
	}
	else if (e.target->getName() == "clearMIDI")
	{
		clearMidiMap();
	}
}

void CBCSPage::sliderEvent(ofxDatGuiSliderEvent e)
{
	string name = e.target->getName();
	if (name == "--unit_length" || name == "--perplexity" || name == "--learning rate" || name == "--iterations")
	{
		_dr.setParameter(name, e.value);
	}
	else if (name == "x" || name == "y")
	{
		_lastControl = "slider/" + name;
		if (!_controlLearn)
		{
			ofVec2f cursor = _map.getCursor();
			if (name == "x") cursor.x = e.value;
			if (name == "y") cursor.y = e.value;
			_map.setCursor(cursor);
		}
	}
	else if (name == "radius")
	{
		_map.setRadius(e.value * 0.5);
	}
	else if (name == "units")
	{
		_map.setMaxSamples(int(e.value));
	}
}

void CBCSPage::toggleEvent(ofxDatGuiToggleEvent e)
{
	if (e.target->getName() == "controlLearn") _controlLearn = e.checked;
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
		}
	}
	if (e.target->getName() == "Mouse Control") _mouseControl = e.checked;
	if (e.target->getName() == "complete") {
		if (e.checked) _dr.setParameter("--cbcs_mode", 1);
		else _dr.setParameter("--cbcs_mode", 0);
	}
}

void CBCSPage::textInputEvent(ofxDatGuiTextInputEvent e)
{
	_address = e.text;
}

void CBCSPage::dropDownEvent(ofxDatGuiDropdownEvent e)
{
	pair<string, string> curFeatures = _map.getSelectedFeatures();
	if (e.target->getName() == "sort-x")
	{
		e.target->setLabel("x:" + e.target->getSelected()->getLabel());
		curFeatures.first = e.target->getSelected()->getLabel();
	}
	else if (e.target->getName() == "sort-y")
	{
		e.target->setLabel("y:" + e.target->getSelected()->getLabel());
		curFeatures.second = e.target->getSelected()->getLabel();
	}
	_map.selectFeatures(curFeatures.first, curFeatures.second);
}

void CBCSPage::mouseMoved(int x, int y)
{
	if (_visible)
	{
		_inside = _position.inside(x, y);
		if (_inside && _mouseControl) _map.setCursor(normalize(ofVec2f(x, y), _position));
	}
}

void CBCSPage::mouseDragged(int x, int y, int button)
{
	_inside = _position.inside(x, y);
	if (_inside)
	{
		if (button == 1)
		{
			//pan
		}
	}
}

void CBCSPage::mousePressed(int x, int y, int button, bool doubleClick)
{
	
}

void CBCSPage::mouseReleased(int x, int y, int button)
{
	_inside = _position.inside(x, y);
	if (_inside)
	{
		if (button == 2)
		{
			_map.removeSelection();
		}
	}
}

void CBCSPage::mouseScrolled(int scroll)
{
	//zoom
}

void CBCSPage::moduleOSCIn(string address, float value)
{
	vector<string> split = ofSplitString(address, "/");
	if (split.size() > 0)
	{
		if (split[0] == "radius")
		{
			if (value > 0 && value <= 1)
			{
				_gui->getSlider("radius")->setValue(value, false);
				_map.setRadius(value * 0.5);
			}
		}
	}
	MapPage::moduleOSCIn(address, value);
}

ofJson CBCSPage::getFileList()
{
	vector<Point> points = _map.getPoints();
	ofJson jFiles;
	if (points.size() > 0)
	{
		for (auto point : points)
		{
			bool pathExists = false;
			string curPath = point.getName();
			for (string path : jFiles["files"])
			{
				if (path == curPath)
				{
					pathExists = true;
					break;
				}
			}
			if (!pathExists)
			{
				jFiles["files"].push_back(curPath);
			}
		}
	}
	return jFiles;
}

void CBCSPage::exportFileList()
{
	//exports list of filenames to load in synth
	ofFileDialogResult saveFile = ofSystemSaveDialog("untitled.cbcs", "Export list of files");
	if (saveFile.bSuccess)
	{
		ofJson jFiles = getFileList();
		string path = saveFile.getPath();
		path = ofSplitString(path, ".")[0];
		path += ".cbcs";
		ofSavePrettyJson(path, jFiles);
	}
}

void CBCSPage::exportSingleFile()
{
	_export.start();
}

void CBCSPage::load(ofJson & json)
{
	_map.clearPoints();
	loadData(json);
	loadMidiMap(json);
	MapPage::load(json);
}

void CBCSPage::loadData(ofJson & json)
{
	if (json.find("error") == json.end())
	{
		bool hasFeatures = _map.getFeatures().size() > 0;
		vector<string> features;
		ofJson featureList = json["features"];
		ofJson selected = json["selected"];

		for (auto& feature : featureList) features.push_back(feature);
		_map.setFeatures(features);

		for (ofJson point : json["points"])
		{
			Point curPoint;
			//info
			curPoint.setName(point["name"].get<string>());
			curPoint.setFeature("position", point["position"]);
			curPoint.setFeature("single-file-position", point["single-file-position"]);
			for (auto& feature : _map.getFeatures()) curPoint.setFeature(feature, point[feature]);
			_map.addPoint(curPoint);
		}

		_map.selectFeatures(selected[0], selected[1]);

		if (hasFeatures) {
			_sortGui->removeComponent(_sortGui->getDropdown("sort-x"));
			_sortGui->removeComponent(_sortGui->getDropdown("sort-y"));
		}
		_sortGui->addDropdown("x", _map.getFeatures())->setName("sort-x");
		_sortGui->addDropdown("y", _map.getFeatures())->setName("sort-y");
		_sortGui->setTheme(new ofxDatGuiThemeWireframe(), true);
		_sortGui->getDropdown("sort-x")->setLabel("x:" + _map.getSelectedFeatures().first);
		_sortGui->getDropdown("sort-y")->setLabel("y:" + _map.getSelectedFeatures().second);
		_sortGui->update();
		_showSortGui = true;
	}
}

void CBCSPage::loadSingleFile(ofJson & json)
{
	for (int i = 0; i < _map.size(); i++)
	{
		Point curPoint = _map.getPoint(i);
		int offset = json["files"][curPoint.getName()];
		int curPosition = offset + curPoint.getFeature("position");
		_map.setPointParameter(i, "single-file-position", curPosition);
	}
}

ofJson CBCSPage::save()
{
	ofJson jSave = MapPage::save();
	vector<Point> points = _map.getPoints();
	vector<string> features = _map.getFeatures();

	for (auto& feature : features) jSave["features"].push_back(feature);
	jSave["selected"].push_back(_map.getSelectedFeatures().first);
	jSave["selected"].push_back(_map.getSelectedFeatures().second);
	for (int i = 0; i < points.size(); i++)
	{
		ofJson curPoint;
		curPoint["x"] = points[i].getPosition().x;
		curPoint["y"] = points[i].getPosition().y;
		curPoint["name"] = points[i].getName();
		curPoint["position"] = points[i].getFeature("position");
		curPoint["single-file-position"] = points[i].getFeature("single-file-position");
		for (auto& feature : features) curPoint[feature] = points[i].getFeature(feature);
		jSave["points"].push_back(curPoint);
	}

	saveMidiMap(jSave);
	return jSave;
}