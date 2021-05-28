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
	
	//MIDI
	setupMIDI();
	//GUI
	_page = 0;
	setupGui();

	//setupMIDI();
	_verdana.load("Verdana2.ttf", 8);

	_shift = false;
	_mode = false;
	_selected = "";
	_shiftSelected = "";
	_id = 0;
}

void ofApp::update() {
	for (auto& node : _nodes) node->update();
	_gui->update();
	//_node[0].page->setVisible(_page == 0);
	//_node[0].page->update();
	//_node[1].page->setVisible(_page == 1);
	//_node[1].page->update();
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
	output.insert(nniOut->begin(), nniOut->end());
	output.insert(trOut->begin(), trOut->end());
	//output.insert(rgbOut->begin(), rgbOut->end());
	sendMIDICC(output, _MIDIOutputs);
	*/
	//updateMIDIGui(true);
}

void ofApp::draw(){
	/*
	ofClear(50);
	ofSetColor(0);
	switch (_page)
	{
	case 0:
		_node[0].page->draw(verdana);
		break;
	case 1:
		_node[1].page->draw(verdana);
		break;;
	}
	*/
	/*
	_gMIDIIn->setEnabled(true);
	_gMIDIIn->setVisible(true);
	_gMIDIIn->setEnabled(true);
	_gMIDIOut->setVisible(true);
	_gMIDIIn->draw();
	_gMIDIOut->draw();
	*/

	ofClear(255);

	if (_mode)
	{
		for (auto& node : _nodes)
		{
			if (node->getVisible())
			{
				ofSetColor(0);
				node->drawPage(_verdana);
			}
		}
	}
	else
	{
		for (auto connection : _connections) drawConnection(connection);
		for (auto& node : _nodes) node->draw(_verdana);
		_gui->draw();
	}
}

void ofApp::drawConnection(Connection& connection)
{
	ofVec2f in, out;
	for (auto& node : _nodes)
	{
		if (connection._fromId == node->getName())
		{
			ofRectangle rect = node->getOutput(connection._fromOutput);
			in = rect.getPosition();
			in.x += rect.getWidth() * 0.5;
			in.y += rect.getHeight() * 0.5;
		}
		if (connection._toId == node->getName())
		{
			ofRectangle rect = node->getInput(connection._toInput);
			out = rect.getPosition();
			out.x += rect.getWidth() * 0.5;
			out.y += rect.getHeight() * 0.5;
		}
	}
	ofSetColor(0);
	ofDrawLine(in, out);
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
void ofApp::setupGui()
{
	_gui = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
	_gui->addHeader("MNT");
	_gui->addButton("NNI");
	_gui->addButton("Trigger");
	_gui->addButton("Draw");
	_gui->addBreak();
	_gui->addButton("Midi Setup");
	_gui->addBreak();
	_gui->addButton("Load");
	_gui->addButton("Save");
	_gui->addFooter();
	_gui->collapse();
	_gui->onButtonEvent(this, &ofApp::buttonEvent);
	_gui->setAutoDraw(false);
	_gui->setWidth(_guiWidth, 0.3);
	_gui->setPosition(ofGetWidth() - _guiWidth, 20);
	_gui->setTheme(new ofxDatGuiThemeWireframe(), true);
	_gui->setOpacity(0.8);
}

void ofApp::buttonEvent(ofxDatGuiButtonEvent e)
{
	NodeInterface *x = new Node<NNIPage>();

	string label = e.target->getLabel();
	if (label == "NNI")
	{
		Node<NNIPage>* node = new Node<NNIPage>();
		node->setup(50, 50, 80, 30, 1, 1);
		node->setupPage(1024, 1024, _guiWidth, _colorPallete);
		node->setName("NNI (" + ofToString(_id) + ")");
		_nodes.push_back(unique_ptr<NodeInterface>(node));
		_id++;
	}
	if (label == "Trigger")
	{
		Node<TriggerPage>* node = new Node<TriggerPage>();
		node->setup(50, 50, 80, 30, 1, 1);
		node->setupPage(1024, 1024, _guiWidth, _colorPallete);
		node->setName("Trigger (" + ofToString(_id) + ")");
		_nodes.push_back(unique_ptr<NodeInterface>(node));
		_id++;
	}
	if (label == "Draw")
	{
		Node<RGBPage>* node = new Node<RGBPage>();
		node->setup(50, 50, 80, 30, 1, 1);
		node->setupPage(1024, 1024, _guiWidth, _colorPallete);
		node->setName("Draw (" + ofToString(_id) + ")");
		_nodes.push_back(unique_ptr<NodeInterface>(node));
		_id++;
	}
}

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
//	_nni.MIDIIn(msg.portName, msg.control, msg.channel, msg.value / 127.);
	//Trigger
//	_trigger.MIDIIn(msg.portName, msg.control, msg.channel, msg.value / 127.);
	//RGB
//	_rgb.MIDIIn(msg.portName, msg.control, msg.channel, msg.value / 127.);

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
		_shiftSelected = "";
		break;
	case(OF_KEY_ESC):
		_mode = false;
		for (auto& node : _nodes) node->setVisible(false);
		break;
	}
}

void ofApp::mouseMoved(int x, int y ){
	
	if (_mode)
	{
		for (auto& node : _nodes)
		{
			if (node->getVisible()) node->mouseMoved(x, y);
		}
	}
	
	/*
	if (_page == 0) _nni.mouseMoved(x, y);
	if (_page == 1) _trigger.mouseMoved(x, y);
	if (_page == 2) _rgb.mouseMoved(x, y);
	*/
	/*
	if (_page == 0) _node[0].page->mouseMoved(x, y);
	if (_page == 1) _node[1].page->mouseMoved(x, y);
	*/
}

void ofApp::mouseDragged(int x, int y, int button){
	
	if (_mode)
	{
		for (auto& node : _nodes)
		{
			if (node->getVisible()) node->mouseDragged(x, y, button);
		}
	}
	else
	{
		for (auto& node : _nodes)
		{
			if (node->getName() == _selected) node->setPosition(x, y);
		}
	}
	/*
	if (_page == 0) _node[0].page->mouseDragged(x, y, button);
	if (_page == 1) _node[1].page->mouseDragged(x, y, button);
	*/
}

void ofApp::mousePressed(int x, int y, int button){
	/*
	if (_page == 0) _node[0].page->mousePressed(x, y, button);
	if (_page == 1) _node[1].page->mousePressed(x, y, button);
	*/

	if (!_mode)
	{
		if (button == 0)
		{
			if (_shift)
			{
				if (_shiftSelected == "")
				{
					for (auto &node : _nodes)
					{
						if (node->inside(x, y))
						{
							if (node->getOutputs() > 0) _shiftSelected = node->getName();
							break;
						}
					}
				}
				else
				{
					string curSelected = _shiftSelected;
					for (auto &node : _nodes)
					{
						if (node->inside(x, y))
						{
							curSelected = node->getName();
							break;
						}
					}
					if (_shiftSelected != "" && curSelected != "" && curSelected != _shiftSelected)
					{
						bool connectionExists = false;
						for (int i = 0; i < _connections.size(); i++)
						{
							if (_connections[i]._fromId == _shiftSelected && _connections[i]._toId == curSelected)
							{
								connectionExists = true;
								_connections.erase(_connections.begin() + i);
								break;
							}
						}
						if (!connectionExists)
						{
							Connection connection;
							connection._fromId = _shiftSelected;
							connection._toId = curSelected;
							connection._fromOutput = 0;
							connection._toInput = 0;
							_connections.push_back(connection);
						}
					}
					_shiftSelected = curSelected;
				}
			}
			else
			{
				string lastSelected = _selected;
				_selected = "";
				for (auto& node : _nodes)
				{
					if (node->inside(x, y))
					{
						_selected = node->getName();
						break;
					}
				}
				cout << _selected << endl;
				cout << lastSelected << endl;
				if (_selected != "" && lastSelected == _selected && ofGetElapsedTimeMillis() - _lastClick < 500)
				{
					for (auto& node : _nodes)
					{
						if (node->getName() == _selected)
						{
							_mode = true;
							node->setVisible(true);
						}
						else node->setVisible(false);
					}
				}
				_lastClick = ofGetElapsedTimeMillis();
			}
		}
	}
	else
	{
		for (auto& node : _nodes)
		{
			if (node->getVisible()) node->mousePressed(x, y, button);
		}
	}
}

void ofApp::mouseReleased(int x, int y, int button){
	//if (_page == 0) _node[0].page->mouseReleased(x, y, button);
	//if (_page == 1) _node[1].page->mouseReleased(x, y, button);
	
	if (_mode)
	{
		for (auto& node : _nodes)
		{
			if (node->getVisible()) node->mouseReleased(x, y, button);
		}
	}
	else
	{
		if (button == 2)
		{
			string curSelected = "";
			int curIndex = -1;
			for (int i = 0; i < _nodes.size(); i++)
			{
				if (_nodes[i]->inside(x, y))
				{
					curSelected = _nodes[i]->getName();
					curIndex = i;
					break;
				}
			}
			if (curIndex != -1)
			{
				vector<int> deleteConnection;
				_nodes.erase(_nodes.begin() + curIndex);
				for (int i = 0; i < _connections.size(); i++)
				{
					if (_connections[i]._fromId == curSelected || _connections[i]._toId == curSelected)
					{
						deleteConnection.push_back(i);
					}
				}
				for (int i = deleteConnection.size() - 1; i >= 0; i--)
				{
					_connections.erase(_connections.begin() + deleteConnection[i]);
				}
			}
			_selected = "";
		}
	}
}

void ofApp::mouseScrolled(ofMouseEventArgs& mouse)
{
	/*
	if (_page == 0) _node[0].page->mouseScrolled(mouse.scrollY * 5);
	if (_page == 1) _node[1].page->mouseScrolled(mouse.scrollY * 5);
	*/
	if (_mode)
	{
		for (auto& node : _nodes)
		{
			if (node->getVisible()) node->mouseScrolled(mouse.scrollY * 5);
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
