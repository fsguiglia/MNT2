#include "ofApp.h"

void ofApp::setup(){
	setWindowTitle("untitled");
	_file = "";
	_settings = ofLoadJson("mnt.ini");
	//NNI
	_nni.setup(1024, 1024, _guiWidth);
	//Trigger
	_trigger.setup(1024, 1024, _guiWidth);
	//
	_rgb.setup(1024, 1024, _guiWidth);
	//MIDI
	setupMIDI();
	//GUI
	_page = 0;
	verdana.load("Verdana2.ttf", 8);
}

void ofApp::update() {

	
	map<string, float> nniOut, trOut, rgbOut, output;
	//NNI
	_nni.setVisible(_page == 0);
	_nni.update();
	nniOut = removePortFromMessages(_nni.getMidiOut());
	//Trigger
	_trigger.setVisible(_page == 1);
	_trigger.update();
	trOut = removePortFromMessages(_trigger.getMidiOut());
	//RGB
	_rgb.setVisible(_page == 2);
	_rgb.update();
	rgbOut = removePortFromMessages(_rgb.getMidiOut());
	//MIDI
	output.insert(nniOut.begin(), nniOut.end());
	output.insert(trOut.begin(), trOut.end());
	//output.insert(rgbOut.begin(), rgbOut.end());
	sendMIDICC(output, _MIDIOutputs);
	updateMIDIGui(_page == 3);
}

void ofApp::draw(){
	ofClear(50);
	ofSetColor(0);
	switch (_page)
	{
	case 0:
		_nni.draw(verdana);
		break;
	case 1:
		_trigger.draw(verdana);
		break;
	case 2:
		_rgb.draw(verdana);
		break;
	case 3:
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
	ofSavePrettyJson("mnt.ini", _settings);
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

void ofApp::updateMIDIGui(bool visible)
{
	_gMIDIIn->setVisible(visible);
	_gMIDIIn->setEnabled(visible);
	_gMIDIIn->update();

	_gMIDIOut->setVisible(visible);
	_gMIDIOut->setEnabled(visible);
	_gMIDIOut->update();
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
	_nni.MIDIIn(msg.portName, msg.control, msg.channel, msg.value / 127.);
	//Trigger
	_trigger.MIDIIn(msg.portName, msg.control, msg.channel, msg.value / 127.);
	//RGB
	_rgb.MIDIIn(msg.portName, msg.control, msg.channel, msg.value / 127.);

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

map<string, float> ofApp::removePortFromMessages(map<string, float> messages)
{
	map<string, float> newMessages;
	for (auto message : messages)
	{
		vector<string> split = ofSplitString(message.first, "/");
		string curMessage;
		for (int i = 1; i < split.size(); i++) curMessage += split[i];
		newMessages[curMessage] = message.second;
	}
	return newMessages;
}

void ofApp::sendMIDICC(map<string, float> parameters, map<string, ofxMidiOut> ports)
{
	for (auto port : ports)
	{
		if (port.second.isOpen())
		{
			for (auto parameter : parameters)
			{
				int channel = ofToInt(ofSplitString(parameter.first, "/")[1]);
				int control = ofToInt(ofSplitString(parameter.first, "/")[2]);
				int value = parameter.second * 127;
				port.second.sendControlChange(channel, control, value);
			}
		}
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
		_nni.load(jNNI);
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
		_settings["folder"] = _folder;
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
		jSave["NNI"] = _nni.save();
		
		//MIDI
		ofJson jMIDI;
		for (auto port : _MIDIInPorts) if (_gMIDIIn->getToggle(port)->getChecked()) jMIDI["in"].push_back(port);
		for (auto port : _MIDIOutPorts) if (_gMIDIOut->getToggle(port)->getChecked()) jMIDI["out"].push_back(port);
		jSave["MIDI"] = jMIDI;

		//Save
		ofSavePrettyJson(path, jSave);
		_file = saveFile.getName();
		_folder = ofSplitString(path, _file)[0];
		_settings["folder"] = _folder;
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
	if (_page == 0) _nni.mouseMoved(x, y);
	if (_page == 1) _trigger.mouseMoved(x, y);
	if (_page == 2) _rgb.mouseMoved(x, y);
}

void ofApp::mouseDragged(int x, int y, int button){
	if (_page == 0) _nni.mouseDragged(x, y, button);
	if (_page == 1) _trigger.mouseDragged(x, y, button);
	if (_page == 2) _rgb.mouseDragged(x, y, button);
}

void ofApp::mousePressed(int x, int y, int button){
	if (_page == 0) _nni.mousePressed(x, y, button);
	if (_page == 1) _trigger.mousePressed(x, y, button);
	if (_page == 2) _rgb.mousePressed(x, y, button);
}

void ofApp::mouseReleased(int x, int y, int button){
	if (_page == 0) _nni.mouseReleased(x, y, button);
	if (_page == 1) _trigger.mouseReleased(x, y, button);
	if (_page == 2) _rgb.mouseReleased(x, y, button);
}

void ofApp::mouseScrolled(ofMouseEventArgs& mouse)
{
	if (_page == 0) _nni.mouseScrolled(mouse.scrollY * 5);
	if (_page == 1) _trigger.mouseScrolled(mouse.scrollY * 5);
	if (_page == 2) _rgb.mouseScrolled(mouse.scrollY * 5);
}

void ofApp::mouseEntered(int x, int y){

}

void ofApp::mouseExited(int x, int y){

}

void ofApp::windowResized(int w, int h){
	_nni.resize(w, h);
	_trigger.resize(w, h);
}

void ofApp::gotMessage(ofMessage msg){

}

void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
