#include "gesturePage.h"

GesturePage::GesturePage()
{
	setMidiOutput(false);
	setOscOutput(true);
	setStringOutput(false);

	_visible = false;
	_recording = false;
	_playing = false;
	_mouseControl = false;
	_inside = false;
	_learn = false;
	_cursor.set(-1, -1);
	_lastControl = "";
	_index = 0;
	_scrubPolySpacing = 0.02;
}

void GesturePage::setup(int w, int h, int guiWidth)
{
	_guiWidth = guiWidth;
	_position = centerSquarePosition(ofGetWidth() - _guiWidth, ofGetHeight());
	setupGui();
}

void GesturePage::setupGui()
{
	_gui = new ofxDatGui();
	_gui->addHeader("Gesture", false);
	_transportFolder = _gui->addFolder("Transport");
	_transportFolder->addToggle("Record")->setName("Record");
	_transportFolder->addButton("Play")->setName("Play");
	_transportFolder->addButton("Next")->setName("Next");
	_transportFolder->addButton("Previous")->setName("Previous");
	_transportFolder->addButton("Random")->setName("Random");
	_transportFolder->addSlider("Scrub", 0, 1, 0)->setName("Scrub");
	_transportFolder->addButton("Delete");
	_transportFolder->collapse();
	_generateFolder = _gui->addFolder("Generate");
	_generateFolder->addButton("Generate (not implemented)");
	_generateFolder->collapse();
	_gui->addSlider("x", 0, 1, 0)->setName("x");
	_gui->addSlider("y", 0, 1, 0)->setName("y");
	_gui->addToggle("MIDI Learn")->setName("Learn");
	_gui->addButton("Clear mappings");
	_gui->onSliderEvent(this, &GesturePage::sliderEvent);
	_gui->onButtonEvent(this, &GesturePage::buttonEvent);
	_gui->onToggleEvent(this, &GesturePage::toggleEvent);

	_gui->setAutoDraw(false);
	_gui->setOpacity(0.5);
	_gui->setTheme(new ofxDatGuiThemeWireframe(), true);
	_gui->getToggle("Record")->setLabelColor(ofColor(255, 100, 100));
	_gui->setWidth(_guiWidth, 0.3);
	_gui->setPosition(ofGetWidth() - _guiWidth, 0);
	_gui->setEnabled(false);
	_gui->setVisible(false);
	_gui->update();

	_guiHeight = _gui->getHeight();
	_scrollView = new ofxDatGuiScrollView("Gestures", 15);
	_scrollView->onScrollViewEvent(this, &GesturePage::scrollViewEvent);
	_scrollView->setOpacity(0.5);
	//_scrollView->setTheme(new ofxDatGuiThemeWireframe());
	_scrollView->setWidth(_guiWidth, 0.3);
	_scrollView->setPosition(ofGetWidth() - _guiWidth, _guiHeight);
	_scrollView->setHeight(_position.height - _guiHeight);
	_scrollView->setEnabled(false);
	_scrollView->setVisible(false);
	_scrollView->update();
}

void GesturePage::update()
{
	if (_playing) play();
	else if (_recording) record();
	if (_cursor != _prevCursor)
	{
		if (_cursor.x >= 0 && _cursor.x <= 1 && _cursor.y >= 0 && _cursor.y <= 1)
		{
			_output["control/x"] = _cursor.x;
			_output["control/y"] = _cursor.y;
		}
	}
	_prevCursor = _cursor;
	updateGui();
}

void GesturePage::updateGui()
{
	_gui->setVisible(_visible);
	_gui->setEnabled(_visible);
	_gui->update();

	if (_gui->getHeight() != _guiHeight)
	{
		_guiHeight = _gui->getHeight();
		_scrollView->setPosition(ofGetWidth() - _guiWidth, _guiHeight);
	}

	_scrollView->setVisible(_visible);
	_scrollView->setEnabled(_visible);
	_scrollView->update();
}

void GesturePage::draw(ofTrueTypeFont font)
{
	ofPushStyle();
	ofSetColor(255);
	ofDrawRectangle(_position);
	
	ofSetColor(_colorPallete[0]);
	ofPolyline curPolyline = _curGesture.getPolyline();
	curPolyline.scale(_position.width, _position.height);
	curPolyline.draw();
	
	ofSetColor(100, 20, 20);
	ofDrawEllipse(_cursor.x *_position.width + _position.x, _cursor.y * _position.height + _position.y, 10, 10);
	ofPolyline playPolylne = _playPoly;
	playPolylne.scale(_position.width, _position.height);
	playPolylne.draw();

	ofSetColor(50);
	font.drawString("Gesture: " + _curGestureName, _position.x + 10, _position.y + _position.height - 10);

	ofSetColor(50);
	ofDrawRectangle(_position.x + _position.width, 0, _guiWidth, _position.height);
	_scrollView->draw();
	_gui->draw();
	ofPopStyle();
}

void GesturePage::resize(int w, int h)
{
	_position = centerSquarePosition(w - _guiWidth, h);
	_gui->setPosition(_position.x + _position.getWidth(), 0);
	_gui->update();
	_scrollView->setPosition(_position.x + _position.getWidth(), _gui->getHeight());
	_scrollView->update();
}

void GesturePage::setColorPallete(vector<ofColor> colorPalette)
{
	_colorPallete = colorPalette;
}

void GesturePage::setVisible(bool visible)
{
	_visible = visible;
}

void GesturePage::record()
{
	if (_cursor.x >= 0 && _cursor.x <= 1 && _cursor.y >= 0 && _cursor.y <= 1)
	{
		_curGesture.addPoint(ofGetElapsedTimeMillis(), Point(_cursor));
	}
}

void GesturePage::startRecording()
{
	_recording = true;
	_curGesture.clear();
}

void GesturePage::endRecording()
{
	if (_curGesture.size() > 1)
	{
		string name = ofToString(_index);
		_curGesture.sort();
		addGesture(_curGesture, name);
		_index++;
		_recording = false;
		_cursor.set(-1, -1);
	}
}

void GesturePage::play()
{
	if (_playGestureIndex < _playGesture.size())
	{
		Point curPoint = _playGesture.getPoint(_playGestureIndex);
		bool playNext = false;
		if (_playGestureIndex == 0) playNext = true;
		else if (ofGetElapsedTimeMillis() - _lastPointTime >= curPoint.getValue("dt")) playNext = true;
		if (playNext)
		{
			_cursor = curPoint.getPosition();
			_playPoly.addVertex(_cursor.x, _cursor.y);
			_lastPointTime = ofGetElapsedTimeMillis();
			_playGestureIndex++;
		}
	}
	else
	{
		_playPoly.clear();
		_playGestureIndex = 0;
		_playing = false;
		_cursor.set(-1, -1);
	}
}

void GesturePage::next()
{
	if (_gestureNames.size() > 0)
	{
		if (_curGestureIndex >= 0)
		{
			_curGestureIndex++;
			if (_curGestureIndex > _gestureNames.size() - 1) _curGestureIndex = 0;
			_curGesture = _gestures[_gestureNames[_curGestureIndex]];
			_scrubPoly = _curGesture.getPolyline().getResampledByCount(20);
		}
	}
}

void GesturePage::previous()
{
	if (_gestureNames.size() > 0)
	{
		if (_curGestureIndex >= 0)
		{
			_curGestureIndex--;
			if (_curGestureIndex < 0) _curGestureIndex = _gestureNames.size() - 1;
			_curGesture = _gestures[_gestureNames[_curGestureIndex]];
			_scrubPoly = _curGesture.getPolyline().getResampledBySpacing(_scrubPolySpacing);
		}
	}
}

void GesturePage::random()
{
	if (_gestureNames.size() > 0)
	{
		int index = (int)ofRandom(0, _gestureNames.size() - 1);
		_curGesture = _gestures[_gestureNames[index]];
		_scrubPoly = _curGesture.getPolyline().getResampledBySpacing(_scrubPolySpacing);
	}
}

void GesturePage::startPlaying()
{
	if (_curGesture.size() > 0)
	{
		_playPoly.clear();
		_playGesture = _curGesture;
		_lastPointTime = 0;
		_playGestureIndex = 0;
		_playing = true;
	}
}

void GesturePage::getCursorAtPercent(float position)
{
	if (position >= 0 && position <= 1)
	{
		float index = position * (_scrubPoly.size() - 1);
		_cursor = _scrubPoly.getPointAtIndexInterpolated(index);
	}
}

void GesturePage::scrollViewEvent(ofxDatGuiScrollViewEvent e)
{
	string sIndex = ofSplitString(e.target->getLabel(), " ")[1];
	if (_gestures.find(sIndex) != _gestures.end())
	{
		_curGestureName = sIndex;
		_curGesture = _gestures[sIndex];
		_scrubPoly = _curGesture.getPolyline().getResampledBySpacing(_scrubPolySpacing);
	}
}

void GesturePage::buttonEvent(ofxDatGuiButtonEvent e)
{
	if (e.target->getName() == "Delete")
	{
		removeGesture(_curGestureName);
		_curGesture.clear();
		_scrubPoly.clear();
	}
	if (e.target->getName() == "Play")
	{
		if(_learn) _lastControl = "button/Play";
		else startPlaying();
	}
	if (e.target->getName() == "Next")
	{
		if (_learn) _lastControl = "button/Next";
		else next();
	}
	if (e.target->getName() == "Previous")
	{
		if (_learn) _lastControl = "button/Previous";
		else previous();
	}
	if (e.target->getName() == "Random")
	{
		if (_learn) _lastControl = "button/Play random";
		else random();
	}
	if (e.target->getName() == "Clear mappings")
	{
		clearMappings();
	}
}

void GesturePage::toggleEvent(ofxDatGuiToggleEvent e)
{
	if (e.target->getName() == "Record")
	{
		if (_learn)
		{
			_lastControl = "toggle/Record";
			e.target->setChecked(false);
		}
		else
		{
			if (e.target->getChecked()) startRecording();
			else endRecording();
		}
	}
	if (e.target->getName() == "Learn")
	{
		_learn = e.target->getChecked();
	}
}

void GesturePage::sliderEvent(ofxDatGuiSliderEvent e)
{
	string name = e.target->getName();
	if (name == "x" || name == "y" || name == "Scrub")
	{
		if (_learn) _lastControl = "slider/" + name;
		else
		{
			if (name == "x") _cursor.x = e.value;
			if (name == "y") _cursor.y = e.value;
			if (name == "Scrub") getCursorAtPercent(e.value);
		}
	}
}

void GesturePage::mouseMoved(int x, int y)
{
}

void GesturePage::mouseDragged(int x, int y, int button)
{
	_inside = _position.inside(x, y);
	if (button == 0)
	{
		if (_inside) {
			_cursor = normalize(ofVec2f(x, y), _position);
			/*
			//esto hace caer el framerate sensiblemente
			_gui->getSlider("x")->setValue(_cursor.x, false);
			_gui->getSlider("y")->setValue(_cursor.y, false);
			*/
		}
	}
}

void GesturePage::mousePressed(int x, int y, int button, bool doubleClick)
{

}

void GesturePage::mouseReleased(int x, int y, int button)
{
	bool inside = _position.inside(x, y);
	if (button = 0)
	{
		if(inside) _cursor.set(-1, -1);
	}
}

void GesturePage::mouseScrolled(int scroll)
{
}

void GesturePage::setMidiOutput(bool midiOutput)
{
	_midiOutput = midiOutput;
}

void GesturePage::setOscOutput(bool oscOutput)
{
	_oscOutput = oscOutput;
}

void GesturePage::setStringOutput(bool stringOutput)
{
	_stringOutput = stringOutput;
}

bool GesturePage::getMidiOutput()
{
	return _midiOutput;
}

bool GesturePage::getOscOutput()
{
	return _oscOutput;
}

bool GesturePage::getStringOutput()
{
	return _stringOutput;
}

void GesturePage::MIDIIn(string port, int control, int channel, float value)
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
			if (_lastControl != "")
			{
				vector<string> vLastControl = ofSplitString(_lastControl, "/");
				_midiMap[_lastControl] = controlName;
				
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
						if (name[1] == "Record")
						{
							if (checked) startRecording();
							else endRecording();
						}
					}
					if (name[0] == "button")
					{
						if (value >= 0.75)
						{
							if (name[1] == "Next") next();
							if (name[1] == "Previous") previous();
							if (name[1] == "Random") random();
							if (name[1] == "Play") startPlaying();
						}
					}
					else if (name[0] == "slider")
					{
						_gui->getSlider(name[1])->setValue(value, false);
						if (name[1] == "x") _cursor.x = value;
						else if(name[1] == "y") _cursor.y = value;
						else if (name[1] == "Scrub") getCursorAtPercent(value);
					}
				}
			}
		}
	}
}

map<string, float> GesturePage::getMidiOut()
{
	return map<string, float>();
}

map<string, float> GesturePage::getMidiDump()
{
	return map<string, float>();
}

void GesturePage::OSCIn(string address, float value)
{
}

map<string, float> GesturePage::getOscOut()
{
	return _output;
}

string GesturePage::getAddress()
{
	return string();
}

vector<string> GesturePage::getStringOut()
{
	return vector<string>();
}

void GesturePage::clearMessages()
{
	_output.clear();
}

void GesturePage::clearMappings()
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

void GesturePage::load(ofJson & json)
{
	_scrollView->clear();
	_midiMap.clear();
	_gestures.clear();
	_index = 0;

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

	for (auto element : json["gestures"])
	{
		
		Gesture gesture;
		gesture.load(element["data"]);

		string name = element["name"].get<string>();
		if (ofToInt(name) > _index) _index = ofToInt(name);
		
		addGesture(gesture, name);
	}

	_index++;
}

ofJson GesturePage::save()
{
	ofJson save;

	for (auto element : _midiMap)
	{
		ofJson mapping;
		mapping["name"] = element.first;
		mapping["value"] = element.second;
		save["midi"].push_back(mapping);
	}

	for (auto element : _gestures)
	{
		ofJson gesture;
		gesture["name"] = element.first;
		gesture["data"] = element.second.save();
		save["gestures"].push_back(gesture);
	}
	
	return save;
}

void GesturePage::addGesture(Gesture gesture, string name)
{
	_gestures[name] = gesture;
	_gestureNames.push_back(name);
	_scrollView->add("gesture " + name);
	bool nameExists = false;
	for (auto curName : _gestureNames)
	{
		if (curName == name) nameExists = true;
	}
	if (!nameExists) _gestureNames.push_back(name);
	_curGestureIndex = _gestureNames.size() - 1;
}

void GesturePage::removeGesture(string name)
{
	_gestures.erase(_curGestureName);
	_scrollView->remove(_scrollView->getItemByName("gesture " + _curGestureName));
	int index = -1;
	for (int i = 0; i < _gestureNames.size(); i++)
	{
		if (_gestureNames[i] == name)
		{
			index = i;
			break;
		}
	}
	if (index != -1) _gestureNames.erase(_gestureNames.begin() + index);
	_curGestureIndex = _gestureNames.size() - 1;
}

ofRectangle GesturePage::centerSquarePosition(int w, int h)
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
