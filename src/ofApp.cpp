#include "ofApp.h"

void ofApp::setup(){
	setWindowTitle("untitled");
	ofSetEscapeQuitsApp(false);
	_file = "";
	_settings = ofLoadJson("mnt.ini");
	ofJson colorPallete = ofLoadJson("color_pallete.json");
	for (auto colors : colorPallete)
	{
		for (auto color : colors)
		{
			vector<string> rgb = ofSplitString(color.get<string>(), ",");
			_colorPallete.push_back(ofColor(ofToInt(rgb[0]), ofToInt(rgb[1]), ofToInt(rgb[2])));
		}
	}
	/*
	//NNI
	_nni.setup(1024, 1024, _guiWidth);
	_nni.setMapColorPallete(_colorPallete);
	//Trigger
	_trigger.setup(1024, 1024, _guiWidth);
	_trigger.setMapColorPallete(_colorPallete);
	//
	_rgb.setup(1024, 1024, _guiWidth);
	_rgb.setMapColorPallete(_colorPallete);
	//MIDI
	setupMIDI();
	//GUI
	_page = 0;
	*/
	//setupMIDI();
	verdana.load("Verdana2.ttf", 8);

	Node<NNIPage> node;
	node.setup(50, 50, 50, 20, 1, 2);
	node.setupPage(1024, 1024, _guiWidth, _colorPallete);
	node.setTitle("NNI (" + ofToString(node.getId()) + ")");
	_nodes.push_back(node);

	Node<NNIPage> nodeDos;
	nodeDos.setup(50, 50, 50, 20, 1, 2);
	nodeDos.setupPage(1024, 1024, _guiWidth, _colorPallete);
	nodeDos.setTitle("NNI (" + ofToString(nodeDos.getId()) + ")");
	_nodes.push_back(nodeDos);


	_shift = false;
	_mode = false;
	_selected = -1;
	_shiftSelected[0] = -1;
	_shiftSelected[1] = -1;
}

void ofApp::update() {
	
	for (auto& node : _nodes) node.update();
	/*
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
	*/
}

void ofApp::draw(){
	/*
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
	*/

	ofClear(255);
	for (auto node : _nodes) node.draw(verdana);
	
	for (auto connection : _connections)
	{
		ofVec2f in, out;
		for (auto node : _nodes)
		{
			if (connection._fromId == node.getId())
			{
				ofRectangle rect = node.getOutput(connection._fromOutput);
				in = rect.getPosition();
				in.x += rect.getWidth() * 0.5;
				in.y += rect.getHeight() * 0.5;
			}
			if (connection._toId == node.getId())
			{
				ofRectangle rect = node.getInput(connection._toInput);
				out = rect.getPosition();
				out.x += rect.getWidth() * 0.5;
				out.y += rect.getHeight() * 0.5;
			}
		}
		ofSetColor(0);
		ofDrawLine(in, out);
	}
	if (_mode)
	{
		ofSetColor(255, 255, 255, 100);
		ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
		for (auto& node : _nodes)
		{
			if (node.getVisible())
			{
				ofSetColor(0);
				node._page.draw(verdana);
			}
		}
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
	/*
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
	*/
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
	/*
	string path;
	ofFileDialogResult loadFile = ofSystemLoadDialog("Load MNT set", false, _folder);
	if (loadFile.bSuccess)
	{
		path = loadFile.getPath();
		ofJson jLoad = ofLoadJson(path);
		//NNI
		ofJson jNNI = jLoad["NNI"];
		_nni.load(jNNI);
		//Trigger
		ofJson jTrigger = jLoad["Trigger"];
		_trigger.load(jTrigger);
		//RGB
		ofJson jRGB = jLoad["RGB"];
		_rgb.load(jRGB);
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
	*/
}

void ofApp::save()
{
	/*
	string path;
	ofFileDialogResult saveFile = ofSystemSaveDialog("untitled.json", "Save MNT set");
	if (saveFile.bSuccess)
	{
		ofJson jSave;
		path = saveFile.getPath();
		
		//NNI
		jSave["NNI"] = _nni.save();
		//Trigger
		jSave["Trigger"] = _trigger.save();
		//RGN
		jSave["RGB"] = _rgb.save();
		
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
	*/
}

void ofApp::setWindowTitle(string title)
{
	ofSetWindowTitle("El mapa no es el territorio - " + title);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == OF_KEY_SHIFT) _shift = true;
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
	case(OF_KEY_SHIFT):
		_shift = false;
		_shiftSelected[0] = -1;
		_shiftSelected[1] = -1;
		break;
	case(OF_KEY_ESC):
		_mode = false;
		for (auto& node : _nodes) node.setVisible(false);
		break;
	}
}

void ofApp::mouseMoved(int x, int y ){
	if (_mode)
	{
		for (auto& node : _nodes)
		{
			if (node.getVisible() == _selected) node._page.mouseMoved(x, y);
		}
	}
	/*
	if (_page == 0) _nni.mouseMoved(x, y);
	if (_page == 1) _trigger.mouseMoved(x, y);
	if (_page == 2) _rgb.mouseMoved(x, y);
	*/
}

void ofApp::mouseDragged(int x, int y, int button){
	if (_mode)
	{
		for (auto& node : _nodes)
		{
			if (node.getVisible()) node._page.mouseDragged(x, y, button);
		}
	}
	else
	{
		for (auto& node : _nodes)
		{
			if (node.getId() == _selected) node.setPosition(x, y);
		}
	}
	/*
	if (_page == 0) _nni.mouseDragged(x, y, button);
	if (_page == 1) _trigger.mouseDragged(x, y, button);
	if (_page == 2) _rgb.mouseDragged(x, y, button);
	*/
}

void ofApp::mousePressed(int x, int y, int button){
	/*
	if (_page == 0) _nni.mousePressed(x, y, button);
	if (_page == 1) _trigger.mousePressed(x, y, button);
	if (_page == 2) _rgb.mousePressed(x, y, button);
	*/
	if (!_mode)
	{
		if (_shift)
		{
			if (_shiftSelected[0] == -1)
			{
				for (auto node : _nodes)
				{
					if (node.inside(x, y))
					{
						if (node.getOutputs() > 0) _shiftSelected[0] = node.getId();
						break;
					}
				}
			}
			else
			{
				int curSelected = _shiftSelected[0];
				for (auto node : _nodes)
				{
					if (node.inside(x, y))
					{
						curSelected = node.getId();
						break;
					}
				}
				if (_shiftSelected[0] != -1 && curSelected != _shiftSelected[0])
				{
					bool connectionExists = false;
					for (int i = 0; i < _connections.size(); i++)
					{
						if (_connections[i]._fromId == _shiftSelected[0] && _connections[i]._toId == curSelected)
						{
							connectionExists = true;
							_connections.erase(_connections.begin() + i);
							break;
						}
					}
					if (!connectionExists)
					{
						Connection connection;
						connection._fromId = _shiftSelected[0];
						connection._toId = curSelected;
						connection._fromOutput = 0;
						connection._toInput = 0;
						_connections.push_back(connection);
					}
				}
				_shiftSelected[0] = curSelected;
			}
		}
		else
		{
			int lastSelected = _selected;
			_selected = -1;
			for (auto node : _nodes)
			{
				if (node.inside(x, y))
				{
					_selected = node.getId();
					break;
				}
			}
			if (_selected != -1 && lastSelected == _selected && ofGetElapsedTimeMillis() - _lastClick < 1000)
			{
				for (auto& node : _nodes)
				{
					if (node.getId() == _selected)
					{
						_mode = true;
						node.setVisible(true);
					}
					else node.setVisible(false);
				}
			}
			_lastClick = ofGetElapsedTimeMillis();
		}
	}
	else
	{
		for (auto& node : _nodes)
		{
			if (node.getVisible()) node._page.mousePressed(x, y, button);
		}
	}
}

void ofApp::mouseReleased(int x, int y, int button){
	/*
	if (_page == 0) _nni.mouseReleased(x, y, button);
	if (_page == 1) _trigger.mouseReleased(x, y, button);
	if (_page == 2) _rgb.mouseReleased(x, y, button);
	*/
	if (_mode)
	{
		for (auto& node : _nodes)
		{
			if (node.getVisible()) node._page.mouseReleased(x, y, button);
		}
	}
}

void ofApp::mouseScrolled(ofMouseEventArgs& mouse)
{
	/*
	if (_page == 0) _nni.mouseScrolled(mouse.scrollY * 5);
	if (_page == 1) _trigger.mouseScrolled(mouse.scrollY * 5);
	if (_page == 2) _rgb.mouseScrolled(mouse.scrollY * 5);
	*/
	if (_mode)
	{
		for (auto& node : _nodes)
		{
			if (node.getVisible()) node._page.mouseScrolled(mouse.scrollY * 5);
		}
	}
}

void ofApp::mouseEntered(int x, int y){

}

void ofApp::mouseExited(int x, int y){

}

void ofApp::windowResized(int w, int h){
	/*
	_nni.resize(w, h);
	_trigger.resize(w, h);
	*/
}

void ofApp::gotMessage(ofMessage msg){

}

void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
