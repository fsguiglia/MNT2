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
	_cursor.set(-1, -1);
	_seed = (int)ofRandom(10000);
	_xFrame = _seed;
	_yFrame = _seed + 1000;
}

void NoiseGenerator::setup(string name, int w, int h, int guiWidth, ofTrueTypeFont font, int maxMessages)
{
	_width = w;
	_height = h;
	_guiWidth = guiWidth;
	_position = centerSquarePosition(ofGetWidth() - _guiWidth, ofGetHeight());
	setupGui(name);
	_font = font;
	
	_fbo.allocate(_width, _height);
	_fbo.begin();
	ofClear(255);
	_fbo.end();
	//maxMessage does nothing...
}

void NoiseGenerator::setupGui(string name)
{
	_gui = new ScrollGui();
	_gui->addHeader(name, false)->setName("Header");
	_gui->addToggle("active");
	_gui->addSlider("x speed", 0, 10, 1)->setName("x speed");
	_gui->addSlider("y speed", 0, 10, 1)->setName("y speed");
	_gui->addSlider("center x", 0, 1, _center.x)->setName("center x");
	_gui->addSlider("center y", 0, 1, _center.y)->setName("center y");
	_gui->addSlider("radius x", 0, 1, _radius.x)->setName("radius x");
	_gui->addSlider("radius y", 0, 1, _radius.y)->setName("radius y");
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
			_OSCOutMessages["global/control/x"] = _cursor.x;
			_OSCOutMessages["global/control/y"] = _cursor.y;
		}
		_xFrame += _xSpeed * 0.02;
		_yFrame += _ySpeed * 0.02;
	}
	
	_gui->setVisible(_visible);
	_gui->setEnabled(_visible);
	_gui->update();
	updateFbo();
}

void NoiseGenerator::updateFbo()
{
	ofPushStyle();
	_fbo.begin();
	ofSetColor(0);
	ofDrawRectangle(0, 0, _width, _height);

	//scale center, radius and cursor for drawing
	ofVec2f scaledCenter = _center * ofVec2f(_width, _height);
	//scaledCenter += ofVec2f(x, y);
	ofVec2f scaledRadius = _radius * ofVec2f(_width, _height);
	ofVec3f scaledCursor = _cursor * ofVec2f(_width, _height);
	//scaledCursor += ofVec2f(_width, _height);

	//draw movement area
	ofSetColor(255);
	ofDrawRectangle(scaledCenter - scaledRadius, scaledRadius.x * 2, scaledRadius.y * 2);

	//draw black rectangle to mask movement area
	ofSetColor(0);
	ofDrawRectangle(0, 0, 0, _height);
	//ofDrawRectangle(x + _position.width + _guiWidth, 0, ofGetWidth(), ofGetHeight());

	//draw cross on center
	ofSetColor(120);
	ofSetLineWidth(8);
	ofDrawLine(scaledCenter.x - 20, scaledCenter.y, scaledCenter.x + 20, scaledCenter.y);
	ofDrawLine(scaledCenter.x, scaledCenter.y - 20, scaledCenter.x, scaledCenter.y + 20);

	//draw cursor
	ofSetColor(255, 100, 100);
	ofDrawEllipse(scaledCursor, 15, 15);
	_fbo.end();
	ofPopStyle();
}

void NoiseGenerator::draw()
{
	ofPushStyle();
	drawNoise(_position.x, _position.y, _position.width, _position.height);
	//draw gui
	ofSetColor(50);
	ofDrawRectangle(_position.x + _position.width, 0, _guiWidth, _position.height);
	_gui->draw();
	ofPopStyle();
}

void NoiseGenerator::drawTile(int x, int y, int w, int h, int margin)
{
	ofPushStyle();
	ofSetColor(0, 100);
	ofDrawRectangle(x, y, w, h);
	ofSetColor(0);
	drawNoise(x + margin / 2, y + margin / 2, w - margin, h - margin);
	ofPopStyle();
}

void NoiseGenerator::drawNoise(int x, int y, int w, int h)
{
	ofSetColor(255);
	_fbo.draw(x, y, w, h);
}

void NoiseGenerator::setColorPallete(vector<ofColor> colorPalette)
{
	_colorPallete = colorPalette;
}

void NoiseGenerator::buttonEvent(ofxDatGuiButtonEvent e)
{
	if (e.target->getName() == "Clear mappings") clearMidiMap();
}

void NoiseGenerator::toggleEvent(ofxDatGuiToggleEvent e)
{
	if (e.target->getName() == "Learn")	_controlLearn = e.target->getChecked();
	if (e.target->getName() == "active")
	{
		if (_controlLearn) 
		{
			_lastControl = "toggle/active";
			e.target->setChecked(false);
		}
		else _active = e.target->getChecked();
	}
}

void NoiseGenerator::sliderEvent(ofxDatGuiSliderEvent e)
{
	string name = e.target->getName();
	if (_controlLearn)
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

void NoiseGenerator::moduleMIDIIn(string port, int control, int channel, float value)
{
}

void NoiseGenerator::moduleMIDIMap(string port, int control, int channel, float value)
{
	string sControl = ofToString(control);
	string sChannel = ofToString(channel);
	string controlName = sChannel + "/" + sControl;
	string controlLabel = "ch" + sChannel + "/cc" + sControl;

	bool valid = true;
	valid = valid && channel >= 0 && channel < 16;
	valid = valid && control >= 0 && control < 128;
	valid = valid && value >= 0 && value <= 1;
	if (valid)
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
					if (name[1] == "active") _active = checked;
				}
				else if (name[0] == "slider")
				{
					if (name[1] == "center y") _gui->getSlider(name[1])->setValue(1 - value);
					else if (name[1] == "x speed") _gui->getSlider(name[1])->setValue(10.0 * value);
					else if (name[1] == "y speed") _gui->getSlider(name[1])->setValue(10.0 * value);
					else _gui->getSlider(name[1])->setValue(value);
				}
			}
		}
	}
}

void NoiseGenerator::moduleOSCIn(string address, float value)
{
	vector<string> split = ofSplitString(address, "/");
	if (split.size() > 0)
	{
		if (split[0] == "active")
		{
			bool curActiveState = (value == 1);
			_gui->getToggle("active")->setChecked(curActiveState);
			_active = curActiveState;
		}
		if (split.size() > 1)
		{
			if (split[0] == "control")
			{
				ofVec2f cursor = _center;
				if (value > 1) value = 1;
				if (value < 0) value = 0;
				if (split[1] == "x") cursor.x = value;
				if (split[1] == "y") cursor.y = value;
				_center = cursor;
			}
		}
	}
}

void NoiseGenerator::load(ofJson & json)
{
	_midiMap.clear();

	loadMidiMap(json);

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

	_active = json["active"];
	_gui->getToggle("active")->setChecked(json["active"]);
}

ofJson NoiseGenerator::save()
{
	ofJson jSave;

	jSave["active"] = _active;
	jSave["center x"] = _center.x;
	jSave["center y"] = _center.y;
	jSave["radius x"] = _radius.x;
	jSave["radius y"] = _radius.y;
	jSave["xSpeed"] = _xSpeed;
	jSave["ySpeed"] = _ySpeed;

	saveMidiMap(jSave);
	return jSave;
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
