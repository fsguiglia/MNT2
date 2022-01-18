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
	_cursor.set(-1, -1);
	_lastControl = "";
	_index = 0;
	_scrubPolySpacing = 0.02;
}

void GesturePage::setup(string name, int w, int h, int guiWidth, ofTrueTypeFont font, int maxMessages)
{
	_guiWidth = guiWidth;
	_position = centerSquarePosition(ofGetWidth() - _guiWidth, ofGetHeight());
	setupLSTM();
	setupGui(name);
	_font = font;
	//maxMessages does nothing...
}

void GesturePage::setupGui(string name)
{
	_gui = new ScrollGui();
	_gui->addHeader(name, false)->setName("Header");
	_transportFolder = _gui->addFolder("Transport");
	_transportFolder->addToggle("Record")->setName("Record");
	_transportFolder->addButton("Play")->setName("Play");
	_transportFolder->addButton("Next")->setName("Next");
	_transportFolder->addButton("Previous")->setName("Previous");
	_transportFolder->addButton("Random")->setName("Random");
	_transportFolder->addSlider("Scrub", 0, 1, 0)->setName("Scrub");
	_transportFolder->collapse();
	_generateFolder = _gui->addFolder("Generate");
	_generateFolder->addButton("Generate neural network model")->setName("Train");
	_generateFolder->addBreak();
	_generateFolder->addButton("Load model and generate gestures")->setName("Generate");
	_generateFolder->addSlider("Temperature", 0, 1, _lstmGen.getParameter("--temperature"));
	_generateFolder->collapse();
	_gui->addSlider("x", 0, 1, 0)->setName("x");
	_gui->addSlider("y", 0, 1, 0)->setName("y");
	_gui->addToggle("MIDI Learn")->setName("Learn");
	_gui->addButton("Clear mappings");
	_gui->addBreak();
	_gui->addButton("Delete gesture")->setName("Delete");
	_gui->addBreak();
	_gui->onSliderEvent(this, &GesturePage::sliderEvent);
	_gui->onButtonEvent(this, &GesturePage::buttonEvent);
	_gui->onToggleEvent(this, &GesturePage::toggleEvent);
	_gui->setAutoDraw(false);
	_gui->setOpacity(0.5);
	_gui->setTheme(new ofxDatGuiThemeWireframe(), true);
	_gui->getToggle("Record")->setLabelColor(ofColor(125, 30, 30));
	_gui->setWidth(_guiWidth, 0.3);
	_gui->setPosition(_position.x + _position.getWidth(), 0);
	_gui->setEnabled(false);
	_gui->setVisible(false);
	_gui->update();

	_guiHeight = _gui->getHeight();
	_scrollView = new ofxDatGuiScrollView("Gestures", 5);
	_scrollView->onScrollViewEvent(this, &GesturePage::scrollViewEvent);
	_scrollView->setOpacity(0.5);
	_scrollView->setPosition(_gui->getPosition().x + 1, _guiHeight);
	//_scrollView->setHeight(40);
	_scrollView->setTheme(new ofxDatGuiThemeWireframe());
	_scrollView->setBackgroundColor(140);
	_scrollView->setWidth(_guiWidth - 1, 0.3);
	_scrollView->setHeight(140);
	_scrollView->setEnabled(false);
	_scrollView->setVisible(false);
	_scrollView->update();
}

void GesturePage::setupLSTM()
{
	_lstmTrain.setup("../../ML/lstm/mnt_lstm.py", "gest", "python"); // py
	//_lstmTrain.setup("../ML/lstm/mnt_lstm.py", "gest", "python"); // exe
	map<string, float> trainParameters;
	trainParameters["--script"] = 0;
	_lstmTrain.setParameters(trainParameters);
	
	_lstmGen.setup("../../ML/lstm/mnt_lstm.py", "gest", "python"); //py
	//_lstmGen.setup("../ML/lstm/mnt_lstm.py", "gest", "python"); //exe
	map<string, float> genParameters;
	genParameters["--script"] = 1;
	genParameters["--temperature"] = 0.1;
	_lstmGen.setParameters(genParameters);
}

void GesturePage::update()
{
	if (_lstmTrain.getRunning())
	{
		if (_lstmTrain.getCompleted()) _lstmTrain.end();
		else _lstmTrain.check();
	}
	if (_lstmGen.getRunning())
	{
		if (_lstmGen.getCompleted()) load(_lstmGen.getData());
		else _lstmGen.check();
	}

	if (_playing) play();
	else if (_recording) record();
	if (_cursor != _prevCursor)
	{
		if (_cursor.x >= 0 && _cursor.x <= 1 && _cursor.y >= 0 && _cursor.y <= 1)
		{
			_OSCOutMessages["global/control/x"] = _cursor.x;
			_OSCOutMessages["global/control/y"] = _cursor.y;
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
		_scrollView->setPosition(_gui->getPosition().x + 1, _guiHeight);
	}

	_scrollView->setVisible(_visible);
	_scrollView->setEnabled(_visible);
	_scrollView->update();
}

void GesturePage::draw()
{
	ofPushStyle();
	drawGestures(_position.x, _position.y, _position.width, _position.height);
	ofSetColor(50);
	ofDrawRectangle(_position.x + _position.width, 0, _guiWidth, _position.height);
	_gui->draw();
	ofSetColor(140);
	ofDrawRectangle(_scrollView->getX(), _scrollView->getY(), _guiWidth - 2, _scrollView->getHeight() + 5);
	_scrollView->draw();
	if (_scrollView->getNumItems() == 0) {
		ofSetColor(110);
		_font.drawString("gestures", _scrollView->getX() + _guiWidth / 2 - 30, _scrollView->getY() + _scrollView->getHeight() / 2);
	}
	ofPopStyle();
}

void GesturePage::drawTile(int x, int y, int w, int h, int margin)
{
	ofPushStyle();
	ofSetColor(0);
	ofDrawRectangle(x, y, w, h);
	drawGestures(x + margin / 2, y + margin / 2, w - margin, h - margin);
	ofPopStyle();
}

void GesturePage::drawGestures(int x, int y, int w, int h)
{
	ofPushStyle();
	ofSetColor(255);
	ofDrawRectangle(x, y, w, h);
	//draw current gesture polyline
	ofSetColor(_colorPallete[0]);
	ofPolyline curPolyline = _curGesture.getPolyline();
	curPolyline.scale(w, h);
	curPolyline.translate(ofVec2f(x, y));
	curPolyline.draw();
	//draw played polyline
	ofSetColor(100, 20, 20);
	ofDrawEllipse(_cursor.x * w + x, _cursor.y * h + y, 10, 10);
	ofPolyline playPolyline = _playPoly;
	playPolyline.scale(w, h);
	playPolyline.translate(ofVec2f(x, y));
	playPolyline.draw();
	//draw gesture name
	ofSetColor(50);
	if (!_recording && _curGestureName != "")
	{
		string text = "Gesture: " + _curGestureName;
		text += ": " + ofToString(_curGesture.getPoints().size()) + " points";
		_font.drawString(text, x + 10, y + h - 10);
	}
	ofPopStyle();
}

void GesturePage::setColorPallete(vector<ofColor> colorPalette)
{
	_colorPallete = colorPalette;
}

void GesturePage::resize(int w, int h)
{
	Page::resize(w, h);
	_scrollView->setPosition(_position.x + _position.getWidth(), _gui->getHeight());
	_scrollView->update();
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
		if (name.size() == 1) name = "0" + name;
		_curGesture.sort();
		addGesture(_curGesture, name);
		_index++;
		_recording = false;
		_cursor.set(-1, -1);
		_scrubPoly = _curGesture.getPolyline().getResampledByCount(20);
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
			selectGesture(_curGestureIndex);
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
			selectGesture(_curGestureIndex);
		}
	}
}

void GesturePage::random()
{
	if (_gestureNames.size() > 0)
	{
		_curGestureIndex = std::rand() % _gestureNames.size();
		selectGesture(_curGestureIndex);
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
	int index = -1;
	for (int i = 0; i < _gestureNames.size(); i++)
	{
		if (_gestureNames[i] == sIndex)
		{
			index = i;
			break;
		}
	}
	if (index != -1) selectGesture(index);
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
		if(_controlLearn) _lastControl = "button/Play";
		else startPlaying();
	}
	if (e.target->getName() == "Next")
	{
		if (_controlLearn) _lastControl = "button/Next";
		else next();
	}
	if (e.target->getName() == "Previous")
	{
		if (_controlLearn) _lastControl = "button/Previous";
		else previous();
	}
	if (e.target->getName() == "Random")
	{
		if (_controlLearn) _lastControl = "button/Random";
		else random();
	}
	if (e.target->getName() == "Clear mappings")
	{
		clearMidiMap();
	}
	//------------------------------
	if (e.target->getName() == "Train") _lstmTrain.start(save());
	if (e.target->getName() == "Generate") _lstmGen.start(save());
}

void GesturePage::toggleEvent(ofxDatGuiToggleEvent e)
{
	if (e.target->getName() == "Record")
	{
		if (_controlLearn)
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
		_controlLearn = e.target->getChecked();
	}
}

void GesturePage::sliderEvent(ofxDatGuiSliderEvent e)
{
	string name = e.target->getName();
	if (name == "x" || name == "y" || name == "Scrub")
	{
		if (_controlLearn) _lastControl = "slider/" + name;
		else
		{
			if (name == "x") _cursor.x = e.value;
			if (name == "y") _cursor.y = 1 - e.value;
			if (name == "Scrub") getCursorAtPercent(e.value);
		}
	}
	if (name == "Temperature")
	{
		_lstmGen.setParameter("--temperature", e.value);
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
	if (button == 0)
	{
		if(inside) _cursor.set(-1, -1);
	}
}

void GesturePage::mouseScrolled(int scroll)
{
}

void GesturePage::moduleMIDIIn(string port, int control, int channel, float value)
{
}

void GesturePage::moduleMIDIMap(string port, int control, int channel, float value)
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
					else if (name[1] == "y") _cursor.y = 1 - value;
					else if (name[1] == "Scrub") getCursorAtPercent(value);
				}
			}
		}
	}
}

void GesturePage::moduleOSCIn(string address, float value)
{
	vector<string> split = ofSplitString(address, "/");
	if (split.size() > 0)
	{
		if (split.size() > 1)
		{
			if (split[0] == "control")
			{
				ofVec2f cursor = _cursor;
				if (value > 1) value = 1;
				if (value < 0) value = 0;
				if (split[1] == "x") cursor.x = value;
				if (split[1] == "y") cursor.y = value;
				_cursor = cursor;
			}
		}
	}
}

void GesturePage::load(ofJson & json)
{
	_midiMap.clear();
	_index = 0;

	loadMidiMap(json);

	for (auto element : json["gestures"])
	{
		
		Gesture gesture;
		gesture.load(element["data"]);

		string name = element["name"].get<string>();
		if (ofToInt(name) > _index) _index = ofToInt(name);
		
		addGesture(gesture, name);
		next();
	}

	_index++;
}

ofJson GesturePage::save()
{
	ofJson jSave;

	for (auto element : _gestures)
	{
		ofJson gesture;
		gesture["name"] = element.first;
		gesture["data"] = element.second.save();
		jSave["gestures"].push_back(gesture);
	}
	
	saveMidiMap(jSave);
	return jSave;
}

void GesturePage::selectGesture(int index)
{
	if (index >= 0 && index < _gestureNames.size())
	{
		_curGestureIndex = index;
		_curGestureName = _gestureNames[_curGestureIndex];
		_curGesture = _gestures[_curGestureName];
		_scrubPoly = _curGesture.getPolyline().getResampledByCount(20);
	}
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
	previous();
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
