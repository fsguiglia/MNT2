#include "noiseGenerator.h"

NoiseGenerator::NoiseGenerator()
{
	setMidiOutput(false);
	setOscOutput(true);
	setStringOutput(false);

	_xSpeed = 1;
	_ySpeed = 1;
	_center.set(0.5, 0.5); 
	_radius.set(0.5, 0.5);
	_active = false;
	_visible = false;
	_learn = false;
	_cursor.set(-1, -1);
	_seed = (int)ofRandom(10000);
	_xFrame = _seed;
	_yFrame = _seed + 1000;
}

void NoiseGenerator::setup(int w, int h, int guiWidth)
{
	_guiWidth = guiWidth;
	_position = centerSquarePosition(ofGetWidth() - _guiWidth, ofGetHeight());
	setupGui();
}

void NoiseGenerator::setupGui()
{
	_gui = new ofxDatGui();
	_gui->addHeader("Perlin Noise", false);
	_gui->addToggle("Active");
	_gui->addSlider("x speed", 0, 10, 1)->setName("x speed");
	_gui->addSlider("y speed", 0, 10, 1)->setName("y speed");
	_gui->addSlider("center x", 0, 1, 0)->setName("center x");
	_gui->addSlider("center y", 0, 1, 1)->setName("center y");
	_gui->addSlider("radius x", 0, 1, 0)->setName("radius x");
	_gui->addSlider("radius y", 0, 1, 1)->setName("radius y");
	_gui->addToggle("MIDI Learn")->setName("Learn");
	_gui->addButton("Clear mappings");
	_gui->onSliderEvent(this, &NoiseGenerator::sliderEvent);
	_gui->onButtonEvent(this, &NoiseGenerator::buttonEvent);
	_gui->onToggleEvent(this, &NoiseGenerator::toggleEvent);

	_gui->setAutoDraw(false);
	_gui->setOpacity(0.5);
	_gui->setTheme(new ofxDatGuiThemeWireframe(), true);
	_gui->setWidth(_guiWidth, 0.3);
	_gui->setPosition(_position.x + _position.getWidth(), 0);
	_gui->setEnabled(false);
	_gui->setVisible(false);
	_gui->update();
}

void NoiseGenerator::update()
{
	if (_active)
	{
		generate();
		if (_cursor.x >= 0 && _cursor.x <= 1 && _cursor.y >= 0 && _cursor.y <= 1)
		{
			_output["control/x"] = _cursor.x;
			_output["control/y"] = _cursor.y;
		}
		_xFrame += _xSpeed * 0.02;
		_yFrame += _ySpeed * 0.02;
	}
	
	_gui->setVisible(_visible);
	_gui->setEnabled(_visible);
	_gui->update();
}

void NoiseGenerator::draw(ofTrueTypeFont font)
{
	ofPushStyle();
	ofSetColor(0);
	ofDrawRectangle(_position);

	//scale center, radius and cursor for drawing
	ofVec2f scaledCenter = _center * ofVec2f(_position.width, _position.height);
	scaledCenter += ofVec2f(_position.x, _position.y);
	ofVec2f scaledRadius = _radius * ofVec2f(_position.width, _position.height);
	ofVec3f scaledCursor = _cursor * ofVec2f(_position.width, _position.height);
	scaledCursor += ofVec2f(_position.x, _position.y);

	//draw movement area
	ofSetColor(255); 
	ofDrawRectangle(scaledCenter - scaledRadius, scaledRadius.x * 2, scaledRadius.y * 2);

	//draw black rectangle to mask movement area
	ofSetColor(0);
	ofDrawRectangle(0, 0, _position.x, ofGetHeight());
	ofDrawRectangle(_position.x + _position.width + _guiWidth, 0, ofGetWidth(), ofGetHeight());

	//draw cross on center
	ofSetColor(160);
	ofDrawLine(scaledCenter.x - 10, scaledCenter.y, scaledCenter.x + 10, scaledCenter.y);
	ofDrawLine(scaledCenter.x, scaledCenter.y - 10, scaledCenter.x, scaledCenter.y + 10);

	//draw cursor
	ofSetColor(255, 100, 100);
	ofDrawEllipse(scaledCursor, 10, 10);

	//draw gui
	ofSetColor(50);
	ofDrawRectangle(_position.x + _position.width, 0, _guiWidth, _position.height);
	_gui->draw();
	ofPopStyle();
}

void NoiseGenerator::resize(int w, int h)
{
	_position = centerSquarePosition(w - _guiWidth, h);
	_gui->setPosition(_position.x + _position.getWidth(), 0);
	_gui->update();
}

void NoiseGenerator::setColorPallete(vector<ofColor> colorPalette)
{
	_colorPallete = colorPalette;
}

void NoiseGenerator::setVisible(bool visible)
{
	_visible = visible;
}

void NoiseGenerator::buttonEvent(ofxDatGuiButtonEvent e)
{
	if (e.target->getName() == "Clear mappings") clearMappings();
}

void NoiseGenerator::toggleEvent(ofxDatGuiToggleEvent e)
{
	if (e.target->getName() == "Learn")	_learn = e.target->getChecked();
	if (e.target->getName() == "Active")
	{
		if (_learn) 
		{
			_lastControl = "toggle/Active";
			e.target->setChecked(false);
		}
		else _active = e.target->getChecked();
	}
}

void NoiseGenerator::sliderEvent(ofxDatGuiSliderEvent e)
{
	string name = e.target->getName();
	if (_learn)
	{
		_lastControl = "slider/" + name;
	}
	else
	{
		if (e.target->getName() == "center x") _center.x = e.value;
		if (e.target->getName() == "center y") _center.y = e.value;
		if (e.target->getName() == "radius x") _radius.x = e.value;
		if (e.target->getName() == "radius y") _radius.y = e.value;
		if (e.target->getName() == "x speed") _xSpeed = e.value;
		if (e.target->getName() == "y speed") _ySpeed = e.value;
	}
}

void NoiseGenerator::mouseMoved(int x, int y)
{
}

void NoiseGenerator::mouseDragged(int x, int y, int button)
{
}

void NoiseGenerator::mousePressed(int x, int y, int button, bool doubleClick)
{
}

void NoiseGenerator::mouseReleased(int x, int y, int button)
{
}

void NoiseGenerator::mouseScrolled(int scroll)
{
}

void NoiseGenerator::setMidiOutput(bool midiOutput)
{
	_midiOutput = midiOutput;
}

void NoiseGenerator::setOscOutput(bool oscOutput)
{
	_oscOutput = oscOutput;
}

void NoiseGenerator::setStringOutput(bool stringOutput)
{
	_stringOutput = stringOutput;
}

bool NoiseGenerator::getMidiOutput()
{
	return _midiOutput;
}

bool NoiseGenerator::getOscOutput()
{
	return _oscOutput;
}

bool NoiseGenerator::getStringOutput()
{
	return _stringOutput;
}

string NoiseGenerator::getAddress()
{
	return string();
}

void NoiseGenerator::MIDIIn(string port, int control, int channel, float value)
{
	string sControl = ofToString(control);
	string sChannel = ofToString(channel);
	string controlName = sChannel + "/" + sControl;
	string controlLabel = "ch" + sChannel + "/cc" + sControl;

	bool valid = true;
	valid = valid && channel >= 0 && channel < 128;
	valid = valid && control >= 0 && control < 128;
	valid = valid && value >= 0 && value <= 1;
	if (valid)
	{
		if (_learn)
		{
			vector<string> vLastControl = ofSplitString(_lastControl, "/");
			bool mapExists = false;
			for (auto element : _midiMap)
			{
				if (element.second == controlName) mapExists = true;
			}
			if (!mapExists)
			{
				_midiMap[_lastControl] = controlName;
				if (vLastControl[0] == "toggle")
				{
					string newName = vLastControl[1] + "(" + controlLabel + ")";
					_gui->getToggle(vLastControl[1])->setLabel(newName);
				}
				else if (vLastControl[0] == "slider")
				{
					string newName = vLastControl[1] + "(" + controlLabel + ")";
					_gui->getSlider(vLastControl[1])->setLabel(newName);
				}
			}
		}
		else
		{
			for (auto element : _midiMap)
			{
				if (element.second == controlName)
				{
					vector<string> name = ofSplitString(element.first, "/");
					if (name[0] == "toggle")
					{
						bool checked = value >= 0.75;
						_gui->getToggle(name[1])->setChecked(checked);
						if (name[1] == "Active") _active = checked;
					}
					else if (name[0] == "slider")
					{
						_gui->getSlider(name[1])->setValue(value);
					}
				}
			}
		}
	}
}

map<string, float> NoiseGenerator::getMidiOut()
{
	return map<string, float>();
}

map<string, float> NoiseGenerator::getMidiDump()
{
	return map<string, float>();
}

void NoiseGenerator::OSCIn(string address, float value)
{
}

map<string, float> NoiseGenerator::getOscOut()
{
	return _output;
}

vector<string> NoiseGenerator::getStringOut()
{
	return vector<string>();
}

void NoiseGenerator::clearMessages()
{
	_output.clear();
}

void NoiseGenerator::clearMappings()
{
	for (auto element : _midiMap)
	{
		vector<string> split = ofSplitString(element.first, "/");
		if (split[0] == "button")
		{
			_gui->getButton(split[1])->setLabel(_gui->getButton(split[1])->getName());
		}
		if (split[0] == "toggle")
		{
			_gui->getToggle(split[1])->setLabel(_gui->getToggle(split[1])->getName());
		}
		if (split[0] == "slider")
		{
			_gui->getSlider(split[1])->setLabel(_gui->getSlider(split[1])->getName());
		}
	}
	_midiMap.clear();
}

void NoiseGenerator::load(ofJson & json)
{
	_midiMap.clear();

	for (auto element : json["midi"])
	{
		string name = element["name"].get<string>();
		string value = element["value"].get<string>();
		string sChannel = ofSplitString(value, "/")[0];
		string sControl = ofSplitString(value, "/")[1];
		string controlName = sChannel + "/" + sControl;
		string controlLabel = "ch" + sChannel + "/cc" + sControl;

		vector<string> vLastControl = ofSplitString(name, "/");
		_midiMap[name] = controlName;
		if (vLastControl[0] == "toggle")
		{
			string newName = vLastControl[1] + "(" + controlLabel + ")";
			_gui->getToggle(vLastControl[1])->setLabel(newName);
		}
		else if (vLastControl[0] == "button")
		{
			string newName = vLastControl[1] + "(" + controlLabel + ")";
			_gui->getButton(vLastControl[1])->setLabel(newName);
		}
		else if (vLastControl[0] == "slider")
		{
			string newName = vLastControl[1] + "(" + controlLabel + ")";
			_gui->getSlider(vLastControl[1])->setLabel(newName);
		}
	}

	_center.x = json["center x"];
	_center.y = json["center y"];
	_radius.x = json["radius x"];
	_radius.y = json["radius y"];
	_xSpeed = json["xSpeed"];
	_ySpeed = json["ySpeed"];

	_gui->getSlider("center x")->setValue(_center.x, false);
	_gui->getSlider("center y")->setValue(_center.y, false);
	_gui->getSlider("radius x")->setValue(_radius.x, false);
	_gui->getSlider("radius y")->setValue(_radius.y, false);
	_gui->getSlider("x speed")->setValue(_xSpeed, false);
	_gui->getSlider("y speed")->setValue(_ySpeed, false);
}

ofJson NoiseGenerator::save()
{
	ofJson save;

	for (auto element : _midiMap)
	{
		ofJson mapping;
		mapping["name"] = element.first;
		mapping["value"] = element.second;
		save["midi"].push_back(mapping);
	}

	save["center x"] = _center.x;
	save["center y"] = _center.y;
	save["radius x"] = _radius.x;
	save["radius y"] = _radius.y;
	save["xSpeed"] = _xSpeed;
	save["ySpeed"] = _ySpeed;

	return save;
}

ofRectangle NoiseGenerator::centerSquarePosition(int w, int h)
{
	ofRectangle rect;
	int max = w;
	int min = h;

	if (h > w)
	{
		min = w;
		max = h;
	}
	rect.setWidth(min);
	rect.setHeight(min);
	rect.setX(float(w - min) * 0.5);
	rect.setY(float(h - min) * 0.5);

	return rect;
}

void NoiseGenerator::generate()
{
	_cursor.x = ofMap(ofNoise(_xFrame), 0, 1, _center.x - _radius.x, _center.x + _radius.x);
	if (_cursor.x < 0) _cursor.x = 0;
	if (_cursor.x > 1) _cursor.x = 1;
	_cursor.y = ofMap(ofNoise(_yFrame), 0, 1, _center.y - _radius.y, _center.y + _radius.y);
	if (_cursor.y < 0) _cursor.y = 0;
	if (_cursor.y > 1) _cursor.y = 1;
}
