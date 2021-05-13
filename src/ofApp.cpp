#include "ofApp.h"

void ofApp::setup(){
	setWindowTitle("untitled");
	_file = "";
	_settings = ofLoadJson("mnt.ini");
	_folder = ofToString(_settings["folder"]);
	//NNI
	setupNNI();
	//MIDI
	setupMIDI();
	//GUI
	_page = 0;
	verdana.load("Verdana2.ttf", 8);
}

void ofApp::update() {

	_nni.update();
	if (_nni.getActive()) NNIMIDIOut(_nni.getWeights());
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
}

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
	_settings["folder"] = _folder;
	ofSavePrettyJson("mnt.ini", _settings);
}

//--------------------------------------------------------------

void ofApp::setupNNI()
{
	_nni.setup(1024, 1024);
	_nniPosition = centerSquarePosition(ofGetWidth() - _guiWidth, ofGetHeight());
	_nni.setActive(false);
	_nni.setRandomize(0.);
	_nni.setDrawInterpolation(true);
	_nni.setDrawSelected(true);
	_nniMouseControl = false;
	_nniInside = false;

	setupNNIGui(_nni.getParameters() , false);
}

void ofApp::setupNNIGui(map<string, float> parameters, bool toggleState)
{
	_gNNI = new ScrollGui();
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
	_gNNI->setMaxHeight(ofGetHeight());
	_gNNI->update();
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
	if (e.target->getName() == "interpolate") _nni.setActive(e.checked);
	if (e.target->getName() == "randomize") _nni.setRandomize(float(e.checked));
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
			ofVec2f nniCursor = _nni.getCursor();
			if (name == "x") nniCursor.x = e.value;
			if (name == "y") nniCursor.y = e.value;
			_nni.setCursor(nniCursor);
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

void ofApp::updateNNISite(int selected, map<string, float> parameters)
{
	_gNNI->getLabel("Parameters")->setLabel("Parameters - Site: " + ofToString(selected));
	for (auto parameter : parameters) _gNNI->getSlider(parameter.first)->setValue(parameter.second);
	for (auto port : _MIDIOutputs) sendMIDICC(parameters, port.second);
}

void ofApp::drawNNI()
{
	ofPushStyle();
	_nni.draw(_nniPosition.x, _nniPosition.y, _nniPosition.getWidth(), _nniPosition.getHeight(), verdana);
	_gNNI->draw();
	ofPopStyle();
}

void ofApp::NNIMIDIIn(ofxMidiMessage& msg)
{
	//parametro puerto/canal/control/valor
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
				_gNNI->setRemovableSlider(curParameter);
				_gNNI->getSlider(curParameter)->setTheme(new ofxDatGuiThemeWireframe());
				_gNNI->setWidth(300, 0.3);
				_gNNI->setOpacity(0.5);
				_gNNI->update();
			}
			else
			{
				_gNNI->getSlider(curParameter)->setValue(value, false);
				_nni.setParameter(curParameter, value);
				if (_nni.getLastSelected() > 0) _nni.setParameter(_nni.getLastSelected(), curParameter, value);
			}
		}
		else
		{
			if (curParameter == _nniCCXY[0]) _gNNI->getSlider("x")->setValue(value);
			if (curParameter == _nniCCXY[1]) _gNNI->getSlider("y")->setValue(value);
		}
	}
}

void ofApp::NNIMIDIOut(map<string, float> weights)
{
	for (auto weight : weights)
	{
		for (auto port : _MIDIOutputs)
		{
			vector<string> msg = ofSplitString(weight.first, "/");
			if (port.second.isOpen())
			{
				port.second.sendControlChange(ofToInt(msg[1]), ofToInt(msg[2]), int(weight.second * 127));
			}
		}
	}
}

//--------------------------------------------------------------

void ofApp::setupMIDI()
{
	_maxMidiMessages = 10;
	ofxMidiIn midiIn;
	_MIDIInPorts = midiIn.getInPortList();
	ofxMidiOut midiOut;
	_MIDIOutPorts = midiOut.getOutPortList();
	setupMIDIGui();
}

void ofApp::setupMIDIGui()
{
	_gMIDIIn = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
	_gMIDIIn->addLabel("MIDI In");
	for (auto port : _MIDIInPorts) _gMIDIIn->addToggle(port);
	_gMIDIIn->onToggleEvent(this, &ofApp::MIDIInToggle);
	_gMIDIIn->setAutoDraw(false);
	_gMIDIIn->setWidth(100, 0.3);
	_gMIDIIn->setPosition(20, 20);
	_gMIDIIn->setTheme(new ofxDatGuiThemeWireframe(), true);

	_gMIDIOut = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
	_gMIDIOut->addLabel("MIDI out");
	for (auto port : _MIDIOutPorts) _gMIDIOut->addToggle(port);
	_gMIDIOut->onToggleEvent(this, &ofApp::MIDIOutToggle);
	_gMIDIOut->setAutoDraw(false);
	_gMIDIOut->setWidth(100, 0.3);
	_gMIDIOut->setPosition(320, 20);
	_gMIDIOut->setTheme(new ofxDatGuiThemeWireframe(), true);
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
	NNIMIDIIn(msg);
	for (auto port : _MIDIOutputs)
	{
		string in, out;
		for (int i = 0; i < ofSplitString(msg.portName, " ").size() - 1; i++)
		{
			in += ofSplitString(msg.portName, " ")[i];
		}
		for (int i = 0; i < ofSplitString(port.first, " ").size() - 1; i++)
		{
			out += ofSplitString(port.first, " ")[i];
		}
		if (port.second.isOpen() && in != out)
		{
			port.second.sendMidiBytes(msg.bytes);
		}
	}
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

void ofApp::drawMIDI()
{
	_gMIDIIn->draw();
	_gMIDIOut->draw();
}

void ofApp::load()
{
	string path;
	ofFileDialogResult loadFile = ofSystemLoadDialog("Load MNT set", false, _folder);
	if (loadFile.bSuccess)
	{
		path = loadFile.getPath();
		ofJson jLoad = ofLoadJson(path);
		//NNI
		ofJson jNNI = jLoad["NNI"];
		///clear current NNI
		_nni.clearPoints();
		_gNNI->clearRemovableSliders();
		//load parameters to NNI and GUI
		for (auto parameter : jNNI["parameters"])
		{
			_nni.addParameter(parameter, 0);
			//GUI
			string sliderLabel = ofSplitString(parameter, "/").back();
			_gNNI->addSlider(sliderLabel, 0., 1.);
			_gNNI->getSlider(sliderLabel)->setName(parameter);
			_gNNI->setRemovableSlider(parameter);
			_gNNI->getSlider(parameter)->setTheme(new ofxDatGuiThemeWireframe());
			_gNNI->setWidth(300, 0.3);
			_gNNI->setOpacity(0.5);
		}
		for (ofJson site : jNNI["sites"])
		{
			for (auto parameter : _nni.getParameters())
			{
				_nni.setParameter(parameter.first, site["parameters"][parameter.first]);
			}
			_nni.addPoint(ofVec2f(site["pos"]["x"], site["pos"]["y"]));
		}
		//update gui
		_gNNI->update();
		if (_nni.getPoints().size() != 0)
		{
			updateNNISite(0, _nni.getPoint(0).getValues());
		}

		//MIDI
		ofJson jMIDI = jLoad["MIDI"];
		for (string port : jMIDI["in"])
		{
			_gMIDIIn->getToggle(port)->setChecked(true);
			if (_MIDIInputs.find(port) == _MIDIInputs.end())
			{
				_MIDIInputs[port] = ofxMidiIn();
				_MIDIInputs[port].openPort(port);
				_MIDIInputs[port].addListener(this);
			}
		}
		for (string port : jMIDI["out"])
		{
			_gMIDIOut->getToggle(port)->setChecked(true);
			if (_MIDIOutputs.find(port) == _MIDIOutputs.end())
			{
				_MIDIOutputs[port] = ofxMidiOut();
				_MIDIOutputs[port].openPort(port);
			}
		}

		//LOAD
		_file = loadFile.getName();
		_folder = ofSplitString(path, _file)[0];
		setWindowTitle(_file);
	}
}

void ofApp::save()
{
	string path;
	ofFileDialogResult saveFile = ofSystemSaveDialog("untitled.json", "Save MNT set");
	if (saveFile.bSuccess)
	{
		ofJson jSave;
		path = saveFile.getPath();
		
		//NNI
		ofJson jNNI;
		map<string, float> nniParameters = _nni.getParameters();
		for (auto element : nniParameters) jNNI["parameters"].push_back(element.first);
		vector<Point> nniSites = _nni.getPoints();
		for (int i = 0; i < nniSites.size(); i++)
		{
			ofJson curSite;
			curSite["id"] = i;
			curSite["pos"]["x"] = nniSites[i].getPosition().x;
			curSite["pos"]["y"] = nniSites[i].getPosition().y;
			for (auto parameter : nniSites[i].getValues())
			{
				curSite["parameters"][parameter.first] = parameter.second;
			}
			jNNI["sites"].push_back(curSite);
		}
		jSave["NNI"] = jNNI;
		
		//MIDI
		ofJson jMIDI;
		for (auto port : _MIDIInPorts) if (_gMIDIIn->getToggle(port)->getChecked()) jMIDI["in"].push_back(port);
		for (auto port : _MIDIOutPorts) if (_gMIDIOut->getToggle(port)->getChecked()) jMIDI["out"].push_back(port);
		jSave["MIDI"] = jMIDI;

		//Save
		ofSavePrettyJson(path, jSave);
		_file = saveFile.getName();
		_folder = ofSplitString(path, _file)[0];
		setWindowTitle(_file);
	}
}

void ofApp::setWindowTitle(string title)
{
	ofSetWindowTitle("El mapa no es el territorio - " + title);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	
}

void ofApp::keyReleased(int key){
	switch (key)
	{
	case(OF_KEY_TAB):
		_page += 1;
		if (_page > _maxPages) _page = 0;
		break;
	case('S'):
	case('s'):
		save();
		break;
	case('L'):
	case('l'):
		load();
		break;
	}
}

void ofApp::mouseMoved(int x, int y ){
	//NNI
	if (_page == 0)
	{
		_nniInside = _nniPosition.inside(x, y);
		if(_nniInside && _nniMouseControl) _nni.setCursor(normalize(ofVec2f(x, y), _nniPosition));
	}
	else _nniInside = false;
}

void ofApp::mouseDragged(int x, int y, int button){
	//NNI
	if (_page == 0)
	{
		ofRectangle guiPosition(_gNNI->getPosition(), _gNNI->getWidth(), _gNNI->getHeight());
		bool insideGui = guiPosition.inside(x, y);
		_nniInside = _nniPosition.inside(x, y);
		if (button < 2 && _nniInside && !insideGui) {
			ofVec2f normPosition = normalize(ofVec2f(x, y), _nniPosition);
			_nni.movePoint(_nni.getLastSelected(), normPosition);
		}
	}
}

void ofApp::mousePressed(int x, int y, int button){
	//NNI
	if (_page == 0)
	{
		_nniInside = _nniPosition.inside(x, y);
		ofRectangle guiPosition(_gNNI->getPosition(), _gNNI->getWidth(), _gNNI->getHeight());
		bool insideGui = guiPosition.inside(x, y);
		if (_nniInside && !insideGui)
		{
			ofVec2f pos = normalize(ofVec2f(x, y), _nniPosition);
			if (button == 0) _nni.addPoint(pos);
			if (button < 2)
			{
				int lastSelected = _nni.getLastSelected();
				int curSelected = _nni.getClosest(pos, true)[0];
				if (curSelected != lastSelected)
				{
					updateNNISite(curSelected, _nni.getPoint(curSelected).getValues());
				}
			}
		}
	}
}

void ofApp::mouseReleased(int x, int y, int button){
	//NNI
	if (_page == 0)
	{
		ofVec2f normalized = normalize(ofVec2f(x, y), _nniPosition);
		_nniInside = _nniPosition.inside(x, y);
		if (button == 2)
		{
			if (_nniInside)
			{
				_nni.removePoint(normalized);
				if (_nni.getPoints().size() > 0)
				{
					int curPoint = _nni.getPoints().size() - 1;
					if (curPoint > 0)
					{
						_nni.setLastSelected(curPoint);
						updateNNISite(curPoint, _nni.getPoint(curPoint).getValues());
					}
					else _nni.setLastSelected(-1);
				}
			}
			else
			{
				_nni.getClosest(normalized, true);
				_gNNI->update();
				_gNNI->updatePositions();
				string removableSlider = _gNNI->inside(x, y);
				if (removableSlider != "")
				{
					_nni.removeParameter(removableSlider);
					_gNNI->removeSlider(removableSlider);
					_gNNI->setPosition(_gNNI->getPosition().x, _gNNI->getPosition().y);
					_gNNI->update();
				}
			}
		}
	}
}

void ofApp::mouseScrolled(ofMouseEventArgs& mouse)
{
	if (_page == 0) _gNNI->scroll(mouse.scrollY * 5);
}

void ofApp::mouseEntered(int x, int y){

}

void ofApp::mouseExited(int x, int y){

}

void ofApp::windowResized(int w, int h){
	_nniPosition = centerSquarePosition(w - _guiWidth, h);
	_gNNI->setPosition(ofGetWidth() - _guiWidth, 0);
	_gNNI->setMaxHeight(ofGetHeight());
	_gNNI->update();
}

void ofApp::gotMessage(ofMessage msg){

}

void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
