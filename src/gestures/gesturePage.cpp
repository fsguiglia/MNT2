#include "gesturePage.h"

GesturePage::GesturePage()
{
	_visible = false;
	_recording = false;
	_mouseControl = false;
	_inside = false;
	_learn = false;
	_curSelected = NULL;
	_cursor.set(-1, -1);
	_lastControl = "";
	_index = 0;
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
	_gui->addToggle("Record")->setName("Record");
	_gui->addButton("Play");
	_gui->addButton("Delete");
	_gui->addSlider("x", 0, 1, 0)->setName("x");
	_gui->addSlider("y", 0, 1, 0)->setName("y");
	_gui->addToggle("MIDI Learn")->setName("Learn");
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

	_scrollView = new ofxDatGuiScrollView("Gestures", 15);
	_scrollView->onScrollViewEvent(this, &GesturePage::scrollViewEvent);
	_scrollView->setOpacity(0.5);
	//_scrollView->setTheme(new ofxDatGuiThemeWireframe());
	_scrollView->setWidth(_guiWidth, 0.3);
	_scrollView->setPosition(ofGetWidth() - _guiWidth, _gui->getHeight());
	_scrollView->setHeight(_position.height - _gui->getHeight());
	_scrollView->setEnabled(false);
	_scrollView->setVisible(false);
	_scrollView->update();
}

void GesturePage::update()
{
	if (_recording) record();
	else if (_playing) play();

	_scrollView->setVisible(_visible);
	_scrollView->setEnabled(_visible);
	_scrollView->update();
	_gui->setVisible(_visible);
	_gui->setEnabled(_visible);
	_gui->update();

	_prevCursor = _cursor;
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
	ofSetColor(50);
	ofDrawRectangle(_position.x + _position.width, 0, _guiWidth, _position.height);
	_scrollView->draw();
	_gui->draw();
	ofSetColor(255, 100, 100);
	ofDrawEllipse(_cursor.x *_position.width + _position.x, _cursor.y * _position.height + _position.y, 10, 10);
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
	if (_curGesture.getPoints().size() > 1)
	{
		string name = ofToString(_index);
		//_curGesture.sort();
		_gestures[name] = _curGesture;
		_scrollView->add("gesture " + name);
		_index++;
		_recording = false;
		_cursor.set(-1, -1);
	}
}

void GesturePage::play()
{
}

void GesturePage::scrollViewEvent(ofxDatGuiScrollViewEvent e)
{
	string sIndex = ofSplitString(e.target->getLabel(), " ")[1];
	if (_gestures.find(sIndex) != _gestures.end())
	{
		_curGestureIndex = sIndex;
		_curGesture = _gestures[sIndex];
	}
}

void GesturePage::buttonEvent(ofxDatGuiButtonEvent e)
{
	if (e.target->getName() == "Delete")
	{
		_gestures.erase(_curGestureIndex);
		_scrollView->remove(_scrollView->getItemByName("gesture " + _curGestureIndex));
		_curGesture.clear();
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
	if (name == "x" || name == "y")
	{
		if (_learn) _lastControl = "slider/" + name;
		else
		{
			if (name == "x") _cursor.x = e.value;
			if (name == "y") _cursor.y = e.value;
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
			_gui->getSlider("x")->setValue(_cursor.x, false);
			_gui->getSlider("y")->setValue(_cursor.y, false);
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
						if (checked) startRecording();
						else endRecording();
					}
					else if (name[0] == "slider")
					{
						_gui->getSlider(name[1])->setValue(value, false);
						if (name[1] == "x") _cursor.x = value;
						else if(name[1] == "y") _cursor.y = value;
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

void GesturePage::clearMIDIMessages()
{
}

void GesturePage::OSCIn(string address, float value)
{
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
		cout << name << ", " << value;
		vector<string> vLastControl = ofSplitString(name, "/");
		_midiMap[name] = controlName;
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

	for (auto element : json["gestures"])
	{
		Gesture gesture;
		string name = element["name"].get<string>();
		if (ofToInt(name) > _index) _index = ofToInt(name);
		gesture.load(element["data"]);
		_gestures[name] = gesture;
		_scrollView->add("gesture " + name);
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
