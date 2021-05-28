#include "ofApp.h"

void ofApp::setup(){
	setWindowTitle("untitled");
	ofSetEscapeQuitsApp(false);
	_file = "";
	_settings = ofLoadJson("mnt.ini");
	setupColor();
	//MIDI
	setupMIDI();
	
	//GUI
	_page = 0;
	setupGui();
	_verdana.load("Verdana2.ttf", 8);
	_lastWidth = ofGetWidth();
	_lastHeight = ofGetHeight();

	_shift = false;
	_mode = false;
	_selected = "";
	_shiftSelected = { "",-1,-1 };
}

void ofApp::update() {
	for (auto& node : _moduleNodes) node->update();
	updateConnections();
	_gui->setVisible(!_mode);
	_gui->setEnabled(!_mode);
	_gui->update();
}

void ofApp::draw(){
	ofClear(255);

	if (_mode)
	{
		ofSetColor(50, 50);
		ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
		ofSetColor(0);
		for (auto& node : _moduleNodes)
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
		for (auto& node : _moduleNodes) node->draw(_verdana);
		for (auto& node : _inputNodes) node.draw(_verdana);
		for (auto& node : _outputNodes) node.draw(_verdana);
		_gui->draw();
	}
}

void ofApp::drawConnection(Connection& connection)
{
	ofVec2f in, out;
	for (auto& node : _moduleNodes)
	{
		if (connection.fromId == node->getName())
		{
			ofRectangle rect = node->getOutputConnector(connection.fromOutput);
			in = rect.getPosition();
			in.x += rect.getWidth() * 0.5;
			in.y += rect.getHeight() * 0.5;
		}
		if (connection.toId == node->getName())
		{
			ofRectangle rect = node->getInputConnector(connection.toInput);
			out = rect.getPosition();
			out.x += rect.getWidth() * 0.5;
			out.y += rect.getHeight() * 0.5;
		}
	}
	for (auto& node : _inputNodes)
	{
		if (connection.fromId == node.getName())
		{
			ofRectangle rect = node.getOutputConnector(connection.fromOutput);
			in = rect.getPosition();
			in.x += rect.getWidth() * 0.5;
			in.y += rect.getHeight() * 0.5;
		}
		if (connection.toId == node.getName())
		{
			ofRectangle rect = node.getInputConnector(connection.toInput);
			out = rect.getPosition();
			out.x += rect.getWidth() * 0.5;
			out.y += rect.getHeight() * 0.5;
		}
	}
	for (auto& node : _outputNodes)
	{
		if (connection.fromId == node.getName())
		{
			ofRectangle rect = node.getOutputConnector(connection.fromOutput);
			in = rect.getPosition();
			in.x += rect.getWidth() * 0.5;
			in.y += rect.getHeight() * 0.5;
		}
		if (connection.toId == node.getName())
		{
			ofRectangle rect = node.getInputConnector(connection.toInput);
			out = rect.getPosition();
			out.x += rect.getWidth() * 0.5;
			out.y += rect.getHeight() * 0.5;
		}
	}
	if (connection.isDump) ofSetColor(255, 0, 0);
	else ofSetColor(0);
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

void ofApp::setupColor()
{
	ofJson colorPallete = ofLoadJson("color_pallete.json");
	for (auto colors : colorPallete)
	{
		for (auto color : colors)
		{
			vector<string> rgb = ofSplitString(color.get<string>(), ",");
			_colorPallete.push_back(ofColor(ofToInt(rgb[0]), ofToInt(rgb[1]), ofToInt(rgb[2])));
		}
	}
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
	_midiInFolder = _gui->addFolder("Midi In");
	for (auto port : _MIDIInPorts) _midiInFolder->addToggle(port);
	_midiInFolder->onToggleEvent(this, &ofApp::MIDIInToggle);
	_midiInFolder->collapse();
	_midiOutFolder = _gui->addFolder("Midi Out");
	for (auto port : _MIDIOutPorts) _midiOutFolder->addToggle(port);
	_midiOutFolder->onToggleEvent(this, &ofApp::MIDIOutToggle);
	_midiOutFolder->collapse();
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
	string label = e.target->getLabel();
	if (label == "NNI")
	{
		ModuleNode<NNIPage>* node = new ModuleNode<NNIPage>();
		node->setup(ofGetWidth() * 0.5, ofGetHeight() * 0.5, 80, 30);
		node->setInputs(1);
		node->setOutputs(1);
		node->setupPage(1024, 1024, _guiWidth, _colorPallete);
		node->setName("NNI", true);
		_moduleNodes.push_back(unique_ptr<ModuleInterface>(node));
	}
	if (label == "Trigger")
	{
		ModuleNode<TriggerPage>* node = new ModuleNode<TriggerPage>();
		node->setup(ofGetWidth() * 0.5, ofGetHeight() * 0.5, 80, 30);
		node->setInputs(1);
		node->setOutputs(1);
		node->setupPage(1024, 1024, _guiWidth, _colorPallete);
		node->setName("Trigger", true);
		_moduleNodes.push_back(unique_ptr<ModuleInterface>(node));
	}
	if (label == "Draw")
	{
		ModuleNode<RGBPage>* node = new ModuleNode<RGBPage>();
		node->setup(ofGetWidth() * 0.5, ofGetHeight() * 0.5, 80, 30);
		node->setInputs(1);
		node->setOutputs(1);
		node->setupPage(1024, 1024, _guiWidth, _colorPallete);
		node->setName("Draw", true);
		_moduleNodes.push_back(unique_ptr<ModuleInterface>(node));
	}
	if (label == "Load")
	{

	}
	if (label == "Save") save();
}

void ofApp::setupMIDI()
{
	_maxMidiMessages = 10;
	ofxMidiIn midiIn;
	_MIDIInPorts = midiIn.getInPortList();
	ofxMidiOut midiOut;
	_MIDIOutPorts = midiOut.getOutPortList();
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
			
			Node node;
			node.setup(50, ofGetHeight() * 0.5, 80, 30);
			node.setName(port);
			node.setAsInput(true);
			node.setColor(ofColor(80, 200, 80));
			_inputNodes.push_back(node);
		}
	}
	else
	{
		if (_MIDIInputs.find(port) != _MIDIInputs.end())
		{
			_MIDIInputs[port].closePort();
			_MIDIInputs[port].removeListener(this);
			_MIDIInputs.erase(port);
			int curIndex = -1;
			for (int i = 0; i < _inputNodes.size(); i++)
			{
				if (_inputNodes[i].getName() == port)
				{
					curIndex = i;
					break;
				}
			}
			if (curIndex != -1)
			{
				vector<int> deleteConnection;
				_inputNodes.erase(_inputNodes.begin() + curIndex);
				for (int i = 0; i < _connections.size(); i++)
				{
					if (_connections[i].fromId == port) deleteConnection.push_back(i);
				}
				for (int i = deleteConnection.size() - 1; i >= 0; i--)
				{
					_connections.erase(_connections.begin() + deleteConnection[i]);
				}
			}
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
			
			Node node;
			node.setup(ofGetWidth() - 250, ofGetHeight() * 0.5, 80, 30);
			node.setName(port);
			node.setAsOutput(true);
			node.setColor(ofColor(80, 200, 80));
			_outputNodes.push_back(node);
		}
	}
	else
	{
		if (_MIDIOutputs.find(port) != _MIDIOutputs.end())
		{
			_MIDIOutputs[port].closePort();
			_MIDIOutputs.erase(port);
			int curIndex = -1;
			for (int i = 0; i < _outputNodes.size(); i++)
			{
				if (_outputNodes[i].getName() == port)
				{
					curIndex = i;
					break;
				}
			}
			if (curIndex != -1)
			{
				vector<int> deleteConnection;
				_outputNodes.erase(_outputNodes.begin() + curIndex);
				for (int i = 0; i < _connections.size(); i++)
				{
					if (_connections[i].toId == port) deleteConnection.push_back(i);
				}
				for (int i = deleteConnection.size() - 1; i >= 0; i--)
				{
					_connections.erase(_connections.begin() + deleteConnection[i]);
				}
			}
		}
	}
}

void ofApp::newMidiMessage(ofxMidiMessage & msg)
{
	_MIDIMessages.push_back(msg);
	while (_MIDIMessages.size() > _maxMidiMessages) {
		_MIDIMessages.erase(_MIDIMessages.begin());
	}
}


tuple<string, int, int> ofApp::selectNode(int x, int y)
{
	tuple<string, int, int> parameters = { "", -1, -1 };
	for (auto &node : _moduleNodes)
	{
		if (node->inside(x, y))
		{
			get<0>(parameters) = node->getName();
			get<1>(parameters) = node->getInputs();
			get<2>(parameters) = node->getOutputs();
			break;
		}
	}
	for (auto &node : _inputNodes)
	{
		if (node.inside(x, y))
		{
			get<0>(parameters) = node.getName();
			get<1>(parameters) = node.getInputs();
			get<2>(parameters) = node.getOutputs();
			break;
		}
	}
	for (auto &node : _outputNodes)
	{
		if (node.inside(x, y))
		{
			get<0>(parameters) = node.getName();
			get<1>(parameters) = node.getInputs();
			get<2>(parameters) = node.getOutputs();
			break;
		}
	}
	return parameters;
}

void ofApp::createDeleteConnection(tuple<string, int, int> out, tuple<string, int, int> in, bool dump)
{
	bool connectionExists = false;
	for (int i = 0; i < _connections.size(); i++)
	{
		if (_connections[i].fromId == get<0>(out) && _connections[i].toId == get<0>(in))
		{
			connectionExists = true;
			_connections.erase(_connections.begin() + i);
			break;
		}
	}
	if (!connectionExists)
	{
		Connection connection;
		connection.fromId = get<0>(out);
		connection.toId = get<0>(in);
		connection.fromInputNode = get<1>(out) == 0;
		connection.toOutputNode = get<2>(in) == 0;
		if(connection.toOutputNode) connection.isDump = dump;
		_connections.push_back(connection);
	}
}

void ofApp::updateConnections()
{
	for (auto& connection : _connections)
	{
		map<string, float> messages;
		if (connection.fromInputNode)
		{
			for (auto msg : _MIDIMessages)
			{
				if (msg.portName == connection.fromId)
				{
					string sPort = msg.portName;
					string sChannel = ofToString(msg.channel);
					string sControl = ofToString(msg.control);
					string key = sPort + "/" + sChannel + "/" + sControl;
					float value = msg.value / 127.;
					messages[key] = value;
				}
			}
		}
		else
		{
			for (auto& node : _moduleNodes)
			{
				if (node->getName() == connection.fromId)
				{
					
					if (connection.isDump) messages = node->getMidiDump();
					else messages = node->getMidiOut();
					break;
				}
			}
		}
		if (connection.toOutputNode)
		{
			for (auto& element : messages)
			{
				int channel = ofToInt(ofSplitString(element.first, "/")[1]);
				int control = ofToInt(ofSplitString(element.first, "/")[2]);
				int value = element.second * 127;
				if (_MIDIOutputs[connection.toId].isOpen())
				{
					_MIDIOutputs[connection.toId].sendControlChange(channel, control, value);
				}
			}
		}
		else
		{
			for (auto& node : _moduleNodes)
			{
				if (node->getName() == connection.toId)
				{
					for (auto element : messages)
					{
 						string port = ofSplitString(element.first, "/")[0];
						int channel = ofToInt(ofSplitString(element.first, "/")[1]);
						int control = ofToInt(ofSplitString(element.first, "/")[2]);
						float value = element.second;
						node->MIDIIn(port, channel, control, value);
					}
				}
			}
		}
	}
	for (auto& node : _moduleNodes) node->clearMIDIMessages();
	if(_MIDIMessages.size() > 0) _MIDIMessages.clear();
}

void ofApp::load()
{
	string path;
	ofFileDialogResult loadFile = ofSystemLoadDialog("Load MNT set", false, _folder);
	if (loadFile.bSuccess)
	{
		path = loadFile.getPath();
		ofJson jLoad = ofLoadJson(path);
		map<string, string> names;

		//MIDI
		ofJson jMIDIIn = jLoad["MIDIIn"];
		cout << "----------" << endl;
		for (auto key : _MIDIInputs) cout << key.first << endl;
		for (auto& element : jMIDIIn)
		{
			string curPort = element["port"].get<string>();
			bool portAvailable = false;
			for (auto port : _MIDIInPorts) {
				if (port == curPort)
				{
					portAvailable = true;
					break;
				}
			}
			if (portAvailable)
			{
				_MIDIInputs[curPort] = ofxMidiIn();
				_MIDIInputs[curPort].openPort(curPort);
				_MIDIInputs[curPort].addListener(this);

				Node node;
				node.setup(element["x"].get<int>(), element["y"].get<int>(), 80, 30);
				node.setName(curPort);
				node.setAsInput(true);
				node.setColor(ofColor(80, 200, 80));
				_inputNodes.push_back(node);
				names[curPort] = curPort;

				_gui->getToggle(curPort)->setChecked(true);
			}
		}
		ofJson jMIDIOut = jLoad["MIDIOut"];
		for (auto& element : jMIDIOut)
		{
			string curPort = element["port"].get<string>();
			bool portAvailable = false;
			for (auto port : _MIDIOutPorts) {
				if (port == curPort)
				{
					portAvailable = true;
					break;
				}
			}
			if (portAvailable)
			{
				_MIDIOutputs[curPort] = ofxMidiOut();
				_MIDIOutputs[curPort].openPort(curPort);
				
				Node node;
				node.setup(element["x"], element["y"], 80, 30);
				node.setName(curPort);
				node.setAsOutput(true);
				node.setColor(ofColor(80, 200, 80));
				_outputNodes.push_back(node);
				names[curPort] = curPort;

				_gui->getToggle(curPort)->setChecked(true);
			}
		}
		
		//Modules
		ofJson jModules = jLoad["Modules"];
		for (auto& element : jModules)
		{
			
			if (element["type"].get<string>() == "NNI")
			{
				
				ModuleNode<NNIPage>* node = new ModuleNode<NNIPage>();
				node->setup(ofGetWidth() * 0.5, ofGetHeight() * 0.5, 80, 30);
				node->setInputs(element["inputs"]);
				node->setOutputs(element["outputs"]);
				node->setupPage(1024, 1024, _guiWidth, _colorPallete);
				node->setName(element["type"].get<string>(), true);
				node->setPosition(element["x"], element["y"]);
				ofJson data = element["data"];
				node->load(data);
				_moduleNodes.push_back(unique_ptr<ModuleInterface>(node));
				string oldName = element["type"].get<string>() + "(" + ofToString(element["id"]) + ")";
				string newName = node->getName();
				names[oldName] = newName;
			}
			else if (element["type"].get<string>() == "Trigger")
			{
				ModuleNode<TriggerPage>* node = new ModuleNode<TriggerPage>();
				node->setup(ofGetWidth() * 0.5, ofGetHeight() * 0.5, 80, 30);
				node->setInputs(element["inputs"]);
				node->setOutputs(element["outputs"]);
				node->setupPage(1024, 1024, _guiWidth, _colorPallete);
				node->setName(element["type"].get<string>(), true);
				node->setPosition(element["x"], element["y"]);
				ofJson data = element["data"];
				node->load(data);
				_moduleNodes.push_back(unique_ptr<ModuleInterface>(node));
				string oldName = element["type"].get<string>() + "(" + ofToString(element["id"]) + ")";
				string newName = node->getName();
				names[oldName] = newName;
			}
			else if (element["type"].get<string>() == "Draw")
			{
				ModuleNode<RGBPage>* node = new ModuleNode<RGBPage>();
				node->setup(ofGetWidth() * 0.5, ofGetHeight() * 0.5, 80, 30);
				node->setInputs(element["inputs"]);
				node->setOutputs(element["outputs"]);
				node->setupPage(1024, 1024, _guiWidth, _colorPallete);
				node->setName(element["type"].get<string>(), true);
				node->setPosition(element["x"], element["y"]);
				ofJson data = element["data"];
				node->load(data);
				_moduleNodes.push_back(unique_ptr<ModuleInterface>(node));
				string oldName = element["type"].get<string>() + "(" + ofToString(element["id"]) + ")";
				string newName = node->getName();
				names[oldName] = newName;
			}
		}
		//CONNECTIONS
		ofJson jConnections = jLoad["Connections"];
		for (auto& element : jConnections)
		{
			Connection connection;
			connection.fromId = names[element["fromId"].get<string>()];
			connection.toId = names[element["toId"].get<string>()];
			connection.fromOutput = element["fromOutput"];
			connection.toInput = element["toInput"];
			connection.fromInputNode = element["fromInputNode"];
			connection.toOutputNode= element["toOutputNode"];
			connection.isDump = element["isDump"];
			_connections.push_back(connection);
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
		//MIDI
		path = saveFile.getPath();
		ofJson jSave;
		ofJson jMIDIIn, jMIDIOut;
		for (auto& node : _inputNodes)
		{
			ofJson input;
			input["port"] = node.getName();
			input["x"] = node.getBox().x;
			input["y"] = node.getBox().y;
			jMIDIIn.push_back(input);
		}
		for (auto& node : _outputNodes) {
			ofJson output;
			output["port"] = node.getName();
			output["x"] = node.getBox().x;
			output["y"] = node.getBox().y;
			jMIDIOut.push_back(output);
		}
		jSave["MIDIIn"] = jMIDIIn;
		jSave["MIDIOut"] = jMIDIOut;

		//Modules
		ofJson jModules;
		for (auto& node : _moduleNodes)
		{
			ofJson jModule;
			jModule["type"] = ofSplitString(node->getName(), "(")[0];
			jModule["id"] = node->getId();
			jModule["x"] = node->getBox().x;
			jModule["y"] = node->getBox().y;
			jModule["inputs"] = node->getInputs();
			jModule["outputs"] = node->getOutputs();
			jModule["data"] = node->save();
			jModules.push_back(jModule);
		}
		jSave["Modules"] = jModules;

		//Connections
		ofJson jConnections;
		for (auto& connection : _connections)
		{
			ofJson jConnection;
			jConnection["fromId"] = connection.fromId;
			jConnection["toId"] = connection.toId;
			jConnection["fromOutput"] = connection.fromOutput;
			jConnection["toInput"] = connection.toInput;
			jConnection["fromInputNode"] = connection.fromInputNode;
			jConnection["toOutputNode"] = connection.toOutputNode;
			jConnection["isDump"] = connection.isDump;
			jConnections.push_back(jConnection);
		}
		jSave["Connections"] = jConnections;
		//Save
		cout << path << endl;
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
	switch (key)
	{
	case(OF_KEY_SHIFT):
		_shift = true;
		break;
	case(OF_KEY_CONTROL):
		_control = true;
		break;
	}
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
		_shiftSelected = { "",-1,-1 };
		break;
	case(OF_KEY_CONTROL):
		_control = false;
		break;
	case(OF_KEY_ESC):
		_mode = false;
		for (auto& node : _moduleNodes) node->setVisible(false);
		break;
	}
}

void ofApp::mouseMoved(int x, int y ){
	if (_mode)
	{
		for (auto& node : _moduleNodes)
		{
			if (node->getVisible()) node->mouseMoved(x, y);
		}
	}
}

void ofApp::mouseDragged(int x, int y, int button){
	
	if (_mode)
	{
		for (auto& node : _moduleNodes)
		{
			if (node->getVisible()) node->mouseDragged(x, y, button);
		}
	}
	else
	{
		for (auto& node : _moduleNodes)
		{
			if (node->getName() == _selected) node->setPosition(x, y);
		}
		for (auto& node : _inputNodes)
		{
			if (node.getName() == _selected) node.setPosition(x, y);
		}
		for (auto& node : _outputNodes)
		{
			if (node.getName() == _selected) node.setPosition(x, y);
		}
	}
}

void ofApp::mousePressed(int x, int y, int button){
	if (!_mode)
	{
		if (button == 0)
		{
			if (_shift)
			{
				_selected = "";
				if (get<0>(_shiftSelected) == "")
				{
					tuple<string, int, int> curSelected = selectNode(x, y);
					if (get<2>(curSelected) > 0) _shiftSelected = curSelected;
					else _shiftSelected = { "", -1 , -1 };
				}
				else
				{
					tuple<string, int, int> curSelected = selectNode(x, y);
					bool keep = get<0>(curSelected) != get<0>(_shiftSelected) && get<1>(curSelected) > 0;
					if (keep) createDeleteConnection(_shiftSelected, curSelected, _control);
					if (get<2>(curSelected) > 0) _shiftSelected = curSelected;
					else _shiftSelected = { "", -1 , -1 };
				}
			}
			else
			{
				string lastSelected = _selected;
				_selected = get<0>(selectNode(x,y));
				bool doubleClick = _selected != "" && lastSelected == _selected;
				doubleClick = doubleClick && ofGetElapsedTimeMillis() - _lastClick < 500;
				if (doubleClick)
				{
					for (auto& node : _moduleNodes)
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
		for (auto& node : _moduleNodes)
		{
			if (node->getVisible()) node->mousePressed(x, y, button);
		}
	}
}

void ofApp::mouseReleased(int x, int y, int button){
	if (_mode)
	{
		for (auto& node : _moduleNodes)
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
			for (int i = 0; i < _moduleNodes.size(); i++)
			{
				if (_moduleNodes[i]->inside(x, y))
				{
					curSelected = _moduleNodes[i]->getName();
					curIndex = i;
					break;
				}
			}
			if (curIndex != -1)
			{
				vector<int> deleteConnection;
				_moduleNodes.erase(_moduleNodes.begin() + curIndex);
				for (int i = 0; i < _connections.size(); i++)
				{
					if (_connections[i].fromId == curSelected || _connections[i].toId == curSelected)
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
	if (_mode)
	{
		for (auto& node : _moduleNodes)
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
	_gui->setPosition(ofGetWidth() - _guiWidth, 20);
	for (auto& node : _moduleNodes)
	{
		node->resizePage(w, h);
		int x = w * float(node->getBox().x) / _lastWidth;
		int y = h * float(node->getBox().y) / _lastHeight;
		node->setPosition(x, y);
	}
	_lastWidth = w;
	_lastHeight = h;
}

void ofApp::gotMessage(ofMessage msg){

}

void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
