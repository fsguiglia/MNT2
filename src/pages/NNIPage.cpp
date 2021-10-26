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
	setupPca();
	setupTsne();
	setupGui();
}

void NNIPage::setupGui()
{
	_arrangeFolder = _gui->addFolder("arrange");
	_arrangeFolder->addButton("PCA")->setName("pca");
	_arrangeFolder->addButton("PCA (Audio)")->setName("pca_audio");
	_arrangeFolder->addButton("t-SNE (Audio)")->setName("tsne_audio");
	_arrangeFolder->addSlider("perplexity", 5, 50, _audioDr.getParameter("--perplexity"))->setName("--perplexity");
	_arrangeFolder->addSlider("learning rate", 10, 1000, _audioDr.getParameter("--learning_rate"))->setName("--learning_rate");
	_arrangeFolder->addSlider("iterations", 250, 2500, _audioDr.getParameter("--iterations"))->setName("--iterations");
	_arrangeFolder->addBreak();
	_arrangeFolder->addToggle("randomize");
	_arrangeFolder->collapse();
	_gui->addBreak();
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
}

void NNIPage::setupTsne()
{
	_audioDr.setup("../../ML/dr/mnt_analysis.py", "nni_audio", "python"); //ver valores por defecto
	map<string, float> audioDrParameters;
	audioDrParameters["--script"] = 1;
	audioDrParameters["--perplexity"] = 5;
	audioDrParameters["--learning_rate"] = 15;
	audioDrParameters["--iterations"] = 1000;
	_audioDr.setParameters(audioDrParameters);
}

void NNIPage::setupPca()
{
	_pca.setup("../../ML/dr/mnt_analysis.py", "pca_nni", "python"); //ver valores por defecto
	map<string, float> audioDrParameters;
	audioDrParameters["--script"] = 2;
	_pca.setParameters(audioDrParameters);
}

void NNIPage::update()
{
	if (_audioDr.getRunning())
	{
		if (_audioDr.getCompleted()) load(_audioDr.getData());
		else _audioDr.check();
	}
	
	if (_pca.getRunning())
	{
		if (_pca.getCompleted()) load(_pca.getData());
		else _pca.check();
	}

	MapPage::update();
}

void NNIPage::buttonEvent(ofxDatGuiButtonEvent e)
{
	if (e.target->getName() == "pca")
	{
		if (!_pca.getRunning()) _pca.start(save());
	}
	else if (e.target->getName() == "tsne_audio")
	{
		_audioDr.setParameter("--technique", 0);
		if (!_audioDr.getRunning()) _audioDr.start(save());
	}
	else if (e.target->getName() == "pca_audio")
	{
		_audioDr.setParameter("--technique", 1);
		if (!_audioDr.getRunning()) _audioDr.start(save());
	}
	else if (e.target->getName() == "clearMIDI")
	{
		clearMidiMap();
	}
}

void NNIPage::sliderEvent(ofxDatGuiSliderEvent e)
{
	string name = e.target->getName();

	if (name == "perplexity" || name == "learning rate" || name == "iterations")
	{
		_audioDr.setParameter(name, e.value);
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
		map<string, float> message;
		message[name] = value;
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

void NNIPage::updateSelected(int selected, Point point)
{
	map<string, float> parameters = point.getValues();
	_gui->getLabel("Parameters")->setLabel("Parameters: " + ofToString(selected));
	for (auto parameter : parameters) _gui->getSlider(parameter.first)->setValue(parameter.second, false);
	map<string, float> curMessage;
	addMessages(parameters, _MIDIOutMessages);
	addMessages(parameters, _MIDIDumpMessages);
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
		if(_lastSelectedPoint >= 0)	_map.movePoint(_lastSelectedPoint, normPosition);
	}
}

void NNIPage::mousePressed(int x, int y, int button, bool doubleClick)
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
			array<float,2> selection = _map.getClosest(pos, false);
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
			//_map.getClosest(normalized, true);
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
	}
	_lastSelectedPoint = -1;
}

void NNIPage::mouseScrolled(int scroll)
{
	_gui->scroll(scroll);
}


void NNIPage::load(ofJson& json)
{
	//clear current NNI
	_map.clearPoints();
	_map.clearGlobalParameters();
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
		_gui->setWidth(_guiWidth, 0.3);
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
	
	if (_map.getPoints().size() != 0)
	{
		_map.setLastSelected(0, ofGetElapsedTimeMillis());
		updateSelected(0, _map.getPoint(_map.getPoints().size() - 1));
	}

	loadMidiMap(json);
	MapPage::load(json);
}

ofJson NNIPage::save()
{
	ofJson jSave = MapPage::save();
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

	saveMidiMap(jSave);
	return jSave;
}
