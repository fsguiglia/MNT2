#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	//NNI
	setupNNI();
	
	//MIDI
	setupMIDI();

	//GUI
	_page = 0;
}

//--------------------------------------------------------------
void ofApp::update() {
	if (_nniRandom) _nni.randomize(1);
	if (_nniInterpolate) _nniWeights = _nni.interpolate(_nniCursor, true);
	updateGuis();
}

void ofApp::updateGuis()
{
	switch (_page)
	{
	case 0:
		_gNNI->setVisible(true);
		_gNNI->setEnabled(true);
		_gNNI->update();
		break;
	case 1:
		_gMIDIIn->setVisible(true);
		_gMIDIIn->setEnabled(true);
		_gMIDIIn->update();
		
		_gMIDIOut->setVisible(true);
		_gMIDIOut->setEnabled(true);
		_gMIDIOut->update();
	}

	cout << _gNNI->getWidth() << endl;
}
//--------------------------------------------------------------
void ofApp::draw(){
	ofClear(50);
	ofSetColor(0);
	switch (_page)
	{
	case 0:
		drawNNI();
		break;
	case 1:
		drawMIDI();
		break;
	}
}

void ofApp::exit()
{
	for (auto port : _MIDIInputs)
	{
		if (port.second.isOpen())
		{
			port.second.closePort();
			port.second.removeListener(this);
		}
	}
	for (auto port : _MIDIOutputs)
	{
		if (port.second.isOpen()) port.second.closePort();
	}
}

ofRectangle ofApp::centerMapPosition(int w, int h)
{
	ofRectangle rect;
	int max = w;
	int min = h;

	if (ofGetWidth() < ofGetHeight())
	{
		max = w;
		min = h;
	}
	rect.setWidth(min);
	rect.setHeight(min);
	rect.setX((w - min) * 0.5);
	rect.setY((h - min) * 0.5);
	
	return rect;
}

ofVec2f ofApp::normalizeMapCursor(int x, int y, ofRectangle mapPosition)
{
	ofVec2f normalized;
	normalized.x = (float)(x - mapPosition.x) / mapPosition.getWidth();
	normalized.y = (float)(y - mapPosition.y) / mapPosition.getHeight();
	return normalized;
}

void ofApp::setupNNI()
{
	_nni.setup(1024, 1024);
	_nniPosition = centerMapPosition(ofGetWidth() - _guiWidth, ofGetHeight());
	_selNNISite = -1;
	_nniInterpolate = false;
	_nniMouseControl = false;
	_nniRandom = false;
	_nniInside = false;

	setupNNIGui(_nni.getParameters(), false);
}

void ofApp::setupNNIGui(map<string, float> parameters, bool toggleState)
{
	_gNNI = new ofxDatGui();
	_gNNI->addHeader("NNI", false);
	_gNNI->addToggle("interpolate");
	_gNNI->addToggle("randomize");
	_gNNI->addBreak();
	_gNNI->addLabel("Control")->setLabelAlignment(ofxDatGuiAlignment::CENTER);
	_gNNI->addToggle("learn", toggleState)->setName("controlLearn");
	_gNNI->addToggle("Mouse Control");
	_gNNI->addSlider("x", 0., 1.)->setName("x");
	_gNNI->addSlider("y", 0., 1.)->setName("y");
	_gNNI->addBreak();
	_gNNI->addLabel("Parameters")->setName("Parameters");
	_gNNI->getLabel("Parameters")->setLabelAlignment(ofxDatGuiAlignment::CENTER);
	_gNNI->addToggle("learn", toggleState)->setName("parameterLearn");
	_gNNI->onToggleEvent(this, &ofApp::NNIToggle);
	for (auto parameter : parameters) _gNNI->addSlider(parameter.first, parameter.second, 0., 1.);
	_gNNI->onSliderEvent(this, &ofApp::NNISlider);
	_gNNI->setAutoDraw(false);
	_gNNI->setOpacity(0.5);
	_gNNI->setTheme(new ofxDatGuiThemeWireframe(), true);
	_gNNI->setWidth(_guiWidth, 0.3);
	_gNNI->setPosition(ofGetWidth() - _guiWidth, 0);
	_gNNI->update();
}

void ofApp::addNNISite(float x, float y, int id)
{
	map<string, float> curValues;
	_nni.add(ofVec2f(x, y));
}

void ofApp::selectNNISite(float x, float y)
{
	float closest = _nni.getClosest(ofVec2f(x, y))[0];
	if (closest != _selNNISite)
	{
		_selNNISite = closest;
		_gNNI->getLabel("Parameters")->setLabel("Parameters - Site: " + ofToString(_selNNISite));
		map<string, float> parameters = _nni.getSites()[_selNNISite].getValues();
		for (auto parameter : parameters)
		{
			_gNNI->getSlider(parameter.first)->setValue(parameter.second);
		}
		for (auto port : _MIDIOutputs) sendMIDICC(parameters, port.second);
	}
}

void ofApp::NNIToggle(ofxDatGuiToggleEvent e)
{
	if (e.target->getName() == "controlLearn")
	{
		_nniControlLearn = e.checked;
		if (_gNNI->getToggle("parameterLearn")->getChecked())
		{
			_gNNI->getToggle("parameterLearn")->setChecked(false);
			_nniParameterLearn = false;
		}
	}
	if (e.target->getName() == "parameterLearn")
	{
		_nniParameterLearn = e.checked;
		if (_gNNI->getToggle("controlLearn")->getChecked())
		{
			_gNNI->getToggle("controlLearn")->setChecked(false);
			_nniControlLearn = false;
		}
	}
	if (e.target->getName() == "interpolate") _nniInterpolate = e.checked;
	if (e.target->getName() == "randomize") _nniRandom = e.checked;
	if (e.target->getName() == "Mouse Control") _nniMouseControl = e.checked;
}

void ofApp::NNISlider(ofxDatGuiSliderEvent e)
{
	string name = e.target->getName();
	if (name == "x" || name == "y")
	{
		_nniLastSelected = name;
		if (!_nniControlLearn)
		{
			if (name == "x") _nniCursor.x = e.value;
			if (name == "y") _nniCursor.y = e.value;
		}
	}
	else
	{
		int channel = ofToInt(ofSplitString(name, "/")[1]);
		int control = ofToInt(ofSplitString(name, "/")[2]);
		float value = e.value;
		_nni.setParameter(name, value);

		for (auto port : _MIDIOutputs)
		{
			if (port.second.isOpen()) {
				port.second.sendControlChange(channel, control, int(value * 127));
			}
		}
	}
}

void ofApp::NNIMIDI(ofxMidiMessage& msg)
{
	if (msg.status == MIDI_CONTROL_CHANGE)
	{
		string control = ofToString(msg.control);
		string channel = ofToString(msg.channel);
		string curParameter = msg.portName + "/" + channel + "/" + control;
		string sliderLabel = "cc" + control;
		float value = msg.value / 127.;
		map<string, float> curParameters = _nni.getParameters();
		if (_nniControlLearn)
		{
			if (_nniLastSelected == "x")
			{
				_nniCCXY[0] = curParameter;
				_gNNI->getSlider("x")->setLabel("x:cc" + control);
			}
			if (_nniLastSelected == "y")
			{
				_nniCCXY[1] = curParameter;
				_gNNI->getSlider("y")->setLabel("y:cc" + control);
			}
		}
		if (_nniParameterLearn)
		{
			if (curParameters.find(curParameter) == curParameters.end())
			{
				_nni.addParameter(curParameter, value);
				_gNNI->addSlider(sliderLabel, 0., 1.);
				_gNNI->getSlider(sliderLabel)->setName(curParameter);
				_gNNI->getSlider(curParameter)->setTheme(new ofxDatGuiThemeWireframe());
				_gNNI->setOpacity(0.5);
				_gNNI->update();
			}
			else
			{
				_gNNI->getSlider(curParameter)->setValue(value, false);
				_nni.setParameter(curParameter, value);
				if (_selNNISite != -1) _nni.setParameter(_selNNISite, curParameter, value);
			}
		}
		else
		{
			if (curParameter == _nniCCXY[0]) _gNNI->getSlider("x")->setValue(value);
			if (curParameter == _nniCCXY[1]) _gNNI->getSlider("y")->setValue(value);
		}
	}
}

void ofApp::drawNNI()
{
	_nni.draw(_nniPosition.x, _nniPosition.y, _nniPosition.getWidth(), _nniPosition.getHeight());
	_gNNI->draw();
}

void ofApp::setupMIDI()
{
	_maxMidiMessages = 10;

	ofxMidiIn midiIn;
	_gMIDIIn = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
	_gMIDIIn->addLabel("MIDI In");
	for (auto port : midiIn.getInPortList()) _gMIDIIn->addToggle(port);
	_gMIDIIn->onToggleEvent(this, &ofApp::MIDIInToggle);
	_gMIDIIn->setAutoDraw(false);
	_gMIDIIn->setWidth(100, 0.3);
	_gMIDIIn->setPosition(20, 20);
	_gMIDIIn->setTheme(new ofxDatGuiThemeWireframe(), true);

	ofxMidiOut midiOut;
	_gMIDIOut = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
	_gMIDIOut->addLabel("MIDI out");
	for (auto port : midiOut.getOutPortList()) _gMIDIOut->addToggle(port);
	_gMIDIOut->onToggleEvent(this, &ofApp::MIDIOutToggle);
	_gMIDIOut->setAutoDraw(false);
	_gMIDIOut->setWidth(100, 0.3);
	_gMIDIOut->setPosition(320, 20);
	_gMIDIOut->setTheme(new ofxDatGuiThemeWireframe(), true);
}

void ofApp::drawMIDI()
{
	_gMIDIIn->draw();
	_gMIDIOut->draw();
}

void ofApp::MIDIInToggle(ofxDatGuiToggleEvent e)
{
	string port = e.target->getLabel();
	if (e.checked)
	{
		if (_MIDIInputs.find(port) == _MIDIInputs.end())
		{
			_MIDIInputs[port] = ofxMidiIn();
			_MIDIInputs[port].openPort(port);
			_MIDIInputs[port].addListener(this);
		}
	}
	else
	{
		if (_MIDIInputs.find(port) != _MIDIInputs.end())
		{
			_MIDIInputs[port].closePort();
			_MIDIInputs[port].removeListener(this);
			_MIDIInputs.erase(port);
		}
	}
}

void ofApp::MIDIOutToggle(ofxDatGuiToggleEvent e)
{
	string port = e.target->getLabel();
	if (e.checked)
	{
		if (_MIDIOutputs.find(port) == _MIDIOutputs.end())
		{
			_MIDIOutputs[port] = ofxMidiOut();
			_MIDIOutputs[port].openPort(port);
		}
	}
	else
	{
		if (_MIDIOutputs.find(port) != _MIDIOutputs.end())
		{
			_MIDIOutputs[port].closePort();
			_MIDIOutputs.erase(port);
		}
	}
}

void ofApp::newMidiMessage(ofxMidiMessage & msg)
{
	//NNI
	NNIMIDI(msg);
}

void ofApp::sendMIDICC(map<string, float> parameters, ofxMidiOut port)
{
	for (auto parameter : parameters)
	{
		int channel = ofToInt(ofSplitString(parameter.first, "/")[1]);
		int control = ofToInt(ofSplitString(parameter.first, "/")[2]);
		int value = parameter.second * 127;
		port.sendControlChange(channel, control, value);
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	switch (key)
	{
	case(OF_KEY_TAB):
		_page += 1;
		if (_page > _maxPages) _page = 0;
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	//NNI
	if (_page == 0)
	{
		_nniInside = _nniPosition.inside(x, y);
		if(_nniInside && _nniMouseControl) _nniCursor.set(normalizeMapCursor(x,y, _nniPosition));
	}
	else _nniInside = false;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	//NNI
	if (_page == 0)
	{
		ofRectangle guiPosition(_gNNI->getPosition(), _gNNI->getWidth(), _gNNI->getHeight());
		bool insideGui = guiPosition.inside(x, y);
		_nniInside = _nniPosition.inside(x, y);
		if (button < 2 && _nniInside && !insideGui) {
			_nni.move(_selNNISite, ofVec2f(normalizeMapCursor(x, y, _nniPosition)));
		}
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	//NNI
	if (_page == 0)
	{
		_nniInside = _nniPosition.inside(x, y);
		ofRectangle guiPosition(_gNNI->getPosition(), _gNNI->getWidth(), _gNNI->getHeight());
		bool insideGui = guiPosition.inside(x, y);
		if (_nniInside && !insideGui)
		{
			ofVec2f pos = normalizeMapCursor(x, y, _nniPosition);
			if (button == 0) addNNISite(pos.x, pos.y, _nni.getSites().size());
			if (button < 2) selectNNISite(pos.x, pos.y);
		}
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	//NNI
	if (_page == 0)
	{
		_nniInside = _nniPosition.inside(x, y);
		ofRectangle guiPosition(_gNNI->getPosition(), _gNNI->getWidth(), _gNNI->getHeight());
		bool insideGui = guiPosition.inside(x, y);
		if (button == 2 && _nniInside)
		{
			_nni.remove(normalizeMapCursor(x, y, _nniPosition));
		}
	}
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
	_nniPosition = centerMapPosition(w - _guiWidth, h);
	_gNNI->setPosition(ofGetWidth() - _guiWidth, 0);
	_gNNI->update();
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
