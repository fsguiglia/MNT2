#include "ofApp.h"

void ofApp::setup(){
	setWindowTitle("untitled");
	ofSetEscapeQuitsApp(false);
	ofSetFrameRate(60);
	/*
	of 0.11 does not support HiDPI Screens on Windows
	Add this to ofAppGLFWindow:
	glm::vec2 ofAppGLFWWindow::getWindowScale() {
		float xscale = 1;
		float yscale = 1;
		glfwGetWindowContentScale(windowP, &xscale, &yscale);
		return glm::vec2(xscale, yscale);
	}
	or replace the next line with _scale = 1
	*/
	_scale = ((ofAppGLFWWindow *)(ofGetWindowPtr()))->getWindowScale().x;
	_guiWidth *= _scale;
	_file = "";
	setupColor();
	//MIDI
	setupMIDI();
	
	//GUI
	_page = -1;
	setupGui();
	_verdana.load("Verdana2.ttf", 8 * _scale);
	_lastWidth = ofGetWidth();
	_lastHeight = ofGetHeight();

	_shift = false;
	_mode = false;
	_selected = "";
	_selectionOffset.set(0, 0);
	_shiftSelected = { "",-1,-1, ofVec2f(-1,-1) };
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
		//draw selected page
		ofSetColor(0);
		ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
		for (auto& node : _moduleNodes)
		{
			if (node->getVisible()) node->drawPage(_verdana);
		};
		//draw prev next areas
		if (mouseX < _pageMarginLeft) ofSetColor(35);
		else ofSetColor(0);
		ofDrawRectangle(0, 0, _pageMarginLeft, ofGetHeight());
		if (mouseX > _pageMarginRight) ofSetColor(35);
		else ofSetColor(0);
		ofDrawRectangle(_pageMarginRight, 0, ofGetWidth() - _pageMarginRight, ofGetHeight());
		//draw prev next icons
		ofSetColor(60);
		ofDrawTriangle(
			ofVec2f(3 * _pageMarginLeft * 0.25, ofGetHeight() * 0.5 - 30),
			ofVec2f(3 * _pageMarginLeft * 0.25, ofGetHeight() * 0.5 + 30),
			ofVec2f(2 * _pageMarginLeft * 0.25, ofGetHeight() * 0.5)
		);
		ofDrawTriangle(
			ofVec2f(_pageMarginRight + _pageMarginLeft * 0.25, ofGetHeight() * 0.5 - 30),
			ofVec2f(_pageMarginRight + _pageMarginLeft * 0.25, ofGetHeight() * 0.5 + 30),
			ofVec2f(_pageMarginRight + 2 * _pageMarginLeft * 0.25, ofGetHeight() * 0.5)
		);
	}
	else
	{
		//draw nodes and connections
		for (auto connection : _connections) drawConnection(connection);
		for (auto& node : _moduleNodes) node->draw();
		for (auto& node : _inputNodes) node.draw();
		for (auto& node : _outputNodes) node.draw();
		_gui->draw();
	}
}

void ofApp::drawConnection(Connection& connection)
{
	ofRectangle in_rect, out_rect;
	for (auto& node : _moduleNodes)
	{
		if (connection.fromId == node->getName()) in_rect = node->getOutputConnector(connection.fromOutput);
		if (connection.toId == node->getName()) out_rect = node->getInputConnector(connection.toInput);
	}
	for (auto& node : _inputNodes)
	{
		if (connection.fromId == node.getName()) in_rect = node.getOutputConnector(connection.fromOutput);
		if (connection.toId == node.getName()) out_rect = node.getInputConnector(connection.toInput);
	}
	for (auto& node : _outputNodes)
	{
		if (connection.fromId == node.getName()) in_rect = node.getOutputConnector(connection.fromOutput);
		if (connection.toId == node.getName()) out_rect = node.getInputConnector(connection.toInput);
	}

	ofVec2f in;
	in = in_rect.getPosition();
	in.x += in_rect.getWidth() * 0.5;
	in.y += in_rect.getHeight() * 0.5;
	ofVec2f out;
	out = out_rect.getPosition();
	out.x += out_rect.getWidth() * 0.5;
	out.y += out_rect.getHeight() * 0.5;
	if (connection.isDump) ofSetColor(255, 0, 0);
	else ofSetColor(0);
	ofDrawLine(in, out);
}

void ofApp::exit()
{
	clear();
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
	/*
	use ofAppGLFWWindow::getWindowScale() -see ofApp::setup()- in ofxDatGuiTheme::ofxDatGuiIsHighResolution()
	if you want gui to resize following windows' scale and layout
	*/
	_moduleColor = ofColor(30, 30, 70, 190);
	_generatorColor = ofColor(125, 30, 190);
	_ioColor = ofColor(30, 125, 30, 190);
	_gui = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
	_gui->addHeader("MNT");
	_moduleFolder = _gui->addFolder("Modules");
	_moduleFolder->addButton("Interpolate");
	_moduleFolder->addButton("Concatenate");
	_moduleFolder->addButton("Trigger");
	_moduleFolder->addButton("Draw");
	_moduleFolder->addButton("Gesture");
	_moduleFolder->addButton("Noise");
	_moduleFolder->onButtonEvent(this, &ofApp::moduleButtonEvent);
	_moduleFolder->collapse();
	_gui->onButtonEvent(this, &ofApp::buttonEvent);
	_midiInFolder = _gui->addFolder("Midi In");
	for (auto port : _MIDIInPorts) _midiInFolder->addToggle(port.first);
	_midiInFolder->onToggleEvent(this, &ofApp::MIDIInToggle);
	_midiInFolder->collapse();
	_midiOutFolder = _gui->addFolder("Midi Out");
	for (auto port : _MIDIOutPorts) _midiOutFolder->addToggle(port.first);
	_midiOutFolder->onToggleEvent(this, &ofApp::MIDIOutToggle);
	_midiOutFolder->collapse();
	_oscFolder = _gui->addFolder("OSC");
	_oscFolder->addTextInput("in", "")->setName("oscIn");
	_oscFolder->addTextInput("out", "")->setName("oscOut");
	_oscFolder->onTextInputEvent(this, &ofApp::OSCTextInput);
	_oscFolder->collapse();
	_gui->addBreak();
	_gui->addButton("New");
	_gui->addButton("Load");
	_gui->addButton("Save");
	_gui->addFooter();
	_gui->collapse();
	_gui->onButtonEvent(this, &ofApp::buttonEvent);
	_gui->setAutoDraw(false);
	_gui->setTheme(new ofxDatGuiThemeWireframe(), true);
	_gui->setWidth(_guiWidth, 0.3);
	_gui->setPosition(ofGetWidth() - _guiWidth, 20);
	_moduleFolder->setLabelColor(_moduleColor);
	/*
	_gui->getButton("Interpolate")->setLabelColor(_moduleColor);
	_gui->getButton("Concatenate")->setLabelColor(_moduleColor);
	_gui->getButton("Trigger")->setLabelColor(_moduleColor);
	_gui->getButton("Draw")->setLabelColor(_moduleColor);
	_gui->getButton("Gesture")->setLabelColor(_generatorColor);
	_gui->getButton("Noise")->setLabelColor(_generatorColor);
	*/
	_moduleFolder->setLabelColor(_moduleColor);
	_midiInFolder->setLabelColor(_ioColor);
	_midiOutFolder->setLabelColor(_ioColor);
	_oscFolder->setLabelColor(_ioColor);
	_gui->setOpacity(0.8);
}

void ofApp::moduleButtonEvent(ofxDatGuiButtonEvent e)
{
	string label = e.target->getLabel();
	if (label == "Interpolate") _moduleNodes.push_back(make_unique<ModuleNode<NNIPage>>());
	if (label == "Concatenate") _moduleNodes.push_back(make_unique<ModuleNode<CBCSPage>>());
	if (label == "Trigger") _moduleNodes.push_back(make_unique<ModuleNode<TriggerPage>>());
	if (label == "Draw") _moduleNodes.push_back(make_unique<ModuleNode<RGBPage>>());
	if (label == "Gesture") _moduleNodes.push_back(make_unique<ModuleNode<GesturePage>>());
	if (label == "Noise") _moduleNodes.push_back(make_unique<ModuleNode<NoiseGenerator>>());

	label = ofToLower(label);
	_moduleNodes[_moduleNodes.size() - 1]->setup(0.5, 0.5, 30, 1, 1, _verdana, _moduleColor);
	_moduleNodes[_moduleNodes.size() - 1]->setName(label, true);
	_moduleNodes[_moduleNodes.size() - 1]->setPosition(
		(ofGetWidth() - _moduleNodes[_moduleNodes.size() - 1]->getWidth()) * 0.5 / (float)ofGetWidth(),
		(ofGetHeight() - _moduleNodes[_moduleNodes.size() - 1]->getHeight()) * 0.5 / (float)ofGetHeight()
	);
	_moduleNodes[_moduleNodes.size() - 1]->setupPage(1024, 1024, _guiWidth, _colorPallete);
	_moduleNodes[_moduleNodes.size() - 1]->setPageHeader(_moduleNodes[_moduleNodes.size() - 1]->getName());
}

void ofApp::buttonEvent(ofxDatGuiButtonEvent e)
{
	string label = e.target->getLabel();
	if (label == "New") clear();
	if (label == "Load") load();
	if (label == "Save") save();
}

void ofApp::changePage(int page)
{
	if (page >= 0 && page <= _moduleNodes.size())
	{
		for (auto& node : _moduleNodes) node->setVisible(false);
		_moduleNodes[page]->setVisible(true);
	}
}

void ofApp::setupMIDI()
{
	_maxMidiMessages = 20;
	ofxMidiIn midiIn;
	for (auto& port : midiIn.getInPortList()) _MIDIInPorts[removePortNumber(port)] = port;
	ofxMidiOut midiOut;
	for (auto& port : midiOut.getOutPortList()) _MIDIOutPorts[removePortNumber(port)] = port;
}

string ofApp::removePortNumber(string name)
{
	vector<string> split = ofSplitString(name, " ");
	string newName = "";
	for (int i = 0; i < split.size() - 1; i++) newName += split[i];
	return newName;
}

void ofApp::MIDIInToggle(ofxDatGuiToggleEvent e)
{
	string port = e.target->getLabel();
	string name = "in:" + port;
	if (e.checked)
	{
		if (_MIDIInputs.find(name) == _MIDIInputs.end())
		{
			createMIDIInput(port, 0.15, 0.5);
			_inputNodes[_inputNodes.size() - 1].setPosition(
				_inputNodes[_inputNodes.size() - 1].getPosition().x,
				(ofGetHeight() - _inputNodes[_inputNodes.size() - 1].getHeight()) * 0.5 / (float)ofGetHeight()
			);
		}
	}
	else
	{
		if (_MIDIInputs.find(name) != _MIDIInputs.end()) deleteMIDIInput(port);
	}
}

void ofApp::MIDIOutToggle(ofxDatGuiToggleEvent e)
{
	string port = e.target->getLabel();
	string name = "out:" + port;
	if (e.checked)
	{
		if (_MIDIOutputs.find(name) == _MIDIOutputs.end())
		{
			createMIDIOutput(port, 0.85, 0.5);
			_outputNodes[_outputNodes.size() - 1].setPosition(
				0.85 - (_outputNodes[_outputNodes.size() - 1].getWidth() / (float)ofGetWidth()),
				(ofGetHeight() - _outputNodes[_outputNodes.size() - 1].getHeight()) * 0.5 / (float)ofGetHeight()
			);
		}
	}
	else
	{
		if (_MIDIOutputs.find(name) != _MIDIOutputs.end()) deleteMIDIOutput(port);
	}
}

string ofApp::createMIDIInput(string port, float x, float y)
{ 
	string name = "in:" + port;
	_MIDIInputs[name] = ofxMidiIn();
	_MIDIInputs[name].openPort(_MIDIInPorts[port]);
	_MIDIInputs[name].addListener(this);

	Node node;
	node.setup(x, y, 30, 1, 0, _verdana, _ioColor);
	node.setName(name);
	node.setAsInput(true);
	_inputNodes.push_back(node);
	return name;
}

void ofApp::deleteMIDIInput(string port)
{
	string name = "in:" + port;
	_MIDIInputs[name].closePort();
	_MIDIInputs[name].removeListener(this);
	_MIDIInputs.erase(name);
	int curIndex = -1;
	for (int i = 0; i < _inputNodes.size(); i++)
	{
		if (_inputNodes[i].getName() == name)
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
			if (_connections[i].fromId == name) deleteConnection.push_back(i);
		}
		for (int i = deleteConnection.size() - 1; i >= 0; i--)
		{
			_connections.erase(_connections.begin() + deleteConnection[i]);
		}
	}
	_gui->getToggle(port, "Midi In")->setChecked(false);
}

string ofApp::createMIDIOutput(string port, float x, float y)
{
	string name = "out:" + port;
	_MIDIOutputs[name] = ofxMidiOut();
	_MIDIOutputs[name].openPort(_MIDIOutPorts[port]);

	Node node;
	node.setup(x, y, 30, 1, 0, _verdana, _ioColor);
	node.setName(name);
	node.setAsOutput(true);
	_outputNodes.push_back(node);
	return name;
}

void ofApp::deleteMIDIOutput(string port)
{
	string name = "out:" + port;
	_MIDIOutputs[name].closePort();
	_MIDIOutputs.erase(name);
	int curIndex = -1;
	for (int i = 0; i < _outputNodes.size(); i++)
	{
		if (_outputNodes[i].getName() == name)
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
			if (_connections[i].toId == name) deleteConnection.push_back(i);
		}
		for (int i = deleteConnection.size() - 1; i >= 0; i--)
		{
			_connections.erase(_connections.begin() + deleteConnection[i]);
		}
	}
	_gui->getToggle(port, "Midi Out")->setChecked(false);
}

void ofApp::newMidiMessage(ofxMidiMessage& msg)
{
	midiMutex.lock();
	_MIDIMessages.push_back(msg);
	while (_MIDIMessages.size() >= _maxMidiMessages) {
		_MIDIMessages.erase(_MIDIMessages.begin());
	}
	midiMutex.unlock();
}

void ofApp::setupOSC()
{
	_maxOscMessages = 20;
}

void ofApp::OSCTextInput(ofxDatGuiTextInputEvent e)
{
	if (e.target->getName() == "oscIn")
	{
		bool isNumber = (e.text.find_first_not_of("0123456789") == std::string::npos);
		if (_oscReceivers.find(e.text) == _oscReceivers.end() && isNumber)
		{
			createOscInput(e.text, 0.15, 0.5);
			_inputNodes[_inputNodes.size() - 1].setPosition(
				_inputNodes[_inputNodes.size() - 1].getPosition().x,
				(ofGetHeight() - _inputNodes[_inputNodes.size() - 1].getHeight()) * 0.5 / (float)ofGetHeight()
			);
		}
	}
	else if (e.target->getName() == "oscOut")
	{
		vector<string> split = ofSplitString(e.text, ":");
		if (split.size() == 2) {
			bool isNumber = (split[1].find_first_not_of("0123456789") == std::string::npos);
			if (isNumber)
			{
				createOscOutput(split[0], split[1], 0.85, 0.5);
				_outputNodes[_outputNodes.size() - 1].setPosition(
					0.85 - (_outputNodes[_outputNodes.size() - 1].getWidth() / (float)ofGetWidth()),
					(ofGetHeight() - _outputNodes[_outputNodes.size() - 1].getHeight()) * 0.5 / (float)ofGetHeight()
				);
			}
		}
	}
	e.target->setText("");
}

void ofApp::createOscInput(string port, float x, float y)
{
	_oscReceivers[port] = ofxOscReceiver();
	_oscReceivers[port].setup(ofToInt(port));

	Node node;
	node.setup(x, y, 30, 0, 1, _verdana, _ioColor);
	node.setName("osc:" + port);
	node.setAsInput(true);
	_inputNodes.push_back(node);
}

void ofApp::deleteOscInput(string port)
{
	_oscReceivers.erase(port);
	int curIndex = -1;
	for (int i = 0; i < _inputNodes.size(); i++)
	{
		if (_inputNodes[i].getName() == "osc:" + port)
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
			if (_connections[i].fromId == "osc:" + port) deleteConnection.push_back(i);
		}
		for (int i = deleteConnection.size() - 1; i >= 0; i--)
		{
			_connections.erase(_connections.begin() + deleteConnection[i]);
		}
	}
}

void ofApp::createOscOutput(string ip, string port, float x, float y)
{
	ofxOscSender sender;
	string name = ip + ":" + port;
	sender.setup(ip, ofToInt(port));
	_oscSenders[name] = sender;

	Node node;
	node.setup(x, y, 30, 1, 0, _verdana, _ioColor);
	node.setName("osc:" + name);
	node.setAsOutput(true);
	_outputNodes.push_back(node);
}

void ofApp::deleteOscOutput(string ip, string port)
{
	string name = ip + ":" + port;
	_oscSenders.erase(name);
	int curIndex = -1;
	for (int i = 0; i < _outputNodes.size(); i++)
	{
		if (_outputNodes[i].getName() == "osc:" + name)
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
			if (_connections[i].toId == "osc:" + name) deleteConnection.push_back(i);
		}
		for (int i = deleteConnection.size() - 1; i >= 0; i--)
		{
			_connections.erase(_connections.begin() + deleteConnection[i]);
		}
	}
}


tuple<string, int, int, ofVec2f> ofApp::selectNode(int x, int y)
{
	tuple<string, int, int, ofVec2f> parameters = { "", -1, -1, ofVec2f(-1, -1)};
	for (int i = _moduleNodes.size() - 1; i >= 0; i--)
	{
		if (_moduleNodes[i]->inside(x, y))
		{
			get<0>(parameters) = _moduleNodes[i]->getName();
			get<1>(parameters) = _moduleNodes[i]->getInputs();
			get<2>(parameters) = _moduleNodes[i]->getOutputs();
			ofVec2f offset;
			offset.x = x - _moduleNodes[i]->getPosition().x * ofGetWidth();
			offset.y = y - _moduleNodes[i]->getPosition().y * ofGetHeight();
			get<3>(parameters) = offset;
			break;
		}
	}
	for (int i = _inputNodes.size() - 1; i >= 0; i--)
	{
		if (_inputNodes[i].inside(x, y))
		{
			get<0>(parameters) = _inputNodes[i].getName();
			get<1>(parameters) = _inputNodes[i].getInputs();
			get<2>(parameters) = _inputNodes[i].getOutputs();
			ofVec2f offset;
			offset.x = x - _inputNodes[i].getPosition().x * ofGetWidth();
			offset.y = y - _inputNodes[i].getPosition().y * ofGetHeight();
			get<3>(parameters) = offset;
			break;
		}
	}
	for (int i = _outputNodes.size() - 1; i >= 0; i--)
	{
		if (_outputNodes[i].inside(x, y))
		{
			get<0>(parameters) = _outputNodes[i].getName();
			get<1>(parameters) = _outputNodes[i].getInputs();
			get<2>(parameters) = _outputNodes[i].getOutputs();
			ofVec2f offset;
			offset.x = x - _outputNodes[i].getPosition().x * ofGetWidth();
			offset.y = y - _outputNodes[i].getPosition().y * ofGetHeight();
			get<3>(parameters) = offset;
			break;
		}
	}
	return parameters;
}

void ofApp::createDeleteConnection(tuple<string, int, int, ofVec2f> out, tuple<string, int, int, ofVec2f> in, bool dump)
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
	//vector of all current midi messages
	midiMutex.lock();
	vector<ofxMidiMessage> curMIDIMessages;
	curMIDIMessages = _MIDIMessages;
	_MIDIMessages.clear();
	midiMutex.unlock();

	/*
	create a copy of all osc messages so they can be used in multiple connection 
	they get deleted from receiver on getNextMessage()
	*/
	map<string, vector<ofxOscMessage>> curOSCMessages;
	for (auto& receiver : _oscReceivers)
	{
		vector<ofxOscMessage> messages;
		while (receiver.second.hasWaitingMessages()) {
			ofxOscMessage m;
			receiver.second.getNextMessage(m);
			messages.push_back(m);
		}
		curOSCMessages[receiver.first] = messages;
	}

	for (auto& connection : _connections)
	{
		map<string, float> MIDIMessages, OSCMessages;
		vector <pair<string, vector<string>>> stringMessages;

		//input
		//from input
		if (connection.fromInputNode)
		{
			string input = connection.fromId;
			vector<string> split = ofSplitString(input, ":");
			bool osc = false;
			if (split.size() > 0)
			{
				//osc input
				if (split[0] == "osc")
				{
					for (auto& receiver : curOSCMessages)
					{
						if (receiver.first == split[1])
						{
							for(auto &message : receiver.second)
							{
								OSCMessages[message.getAddress()] = message.getArgAsFloat(0);
							}
						}
					}
				}
				//midi input
				else
				{
					for (auto msg : curMIDIMessages)
					{
						if (msg.portName == _MIDIInPorts[split[1]])
						{
							string sPort = msg.portName;
							string sChannel = ofToString(msg.channel);
							string sControl = ofToString(msg.control);
							string key = sPort + "/" + sChannel + "/" + sControl;
							float value = msg.value / 127.;
							MIDIMessages[key] = value;
						}
					}
				}
			}
		}
		//from module
		else
		{
			for (auto& node : _moduleNodes)
			{
				if (node->getName() == connection.fromId)
				{
					//midi dump
					if (connection.isDump)
					{
						if (node->getMidiOutput())
						{
							for (auto message : node->getMidiDump())
							{
								//add node name as port name
								string newName = node->getName() + "/" + message.first;
								MIDIMessages[newName] = message.second;
							}
						}
					}
					else
					{
						//midi
						if (node->getMidiOutput())
						{
							for (auto message : node->getMidiOut())
							{
								//add node name as port name
								string newName = node->getName() + "/" + message.first;
								MIDIMessages[newName] = message.second;
							}
						}
						if(node->getOscOutput()) OSCMessages = node->getOSCOut();
						if (node->getStringOutput())
						{
							vector<string> vOutput = node->getStringOut();
							if (vOutput.size() > 0)
							{
								pair<string, vector<string>> curOutput;
								curOutput.first = node->getAddress();
								curOutput.second = vOutput;
								stringMessages.push_back(curOutput);
							}
						}
					}
					break;
				}
			}
		}
		//output
		//output node
		if (connection.toOutputNode)
		{
			string output = connection.toId;
			vector<string> split = ofSplitString(output, ":");
			bool osc = false;
			if (split.size() == 3)
			{
				if (split[0] == "osc") osc = true;
			}
			//osc output
			if (osc)
			{
				for (auto& element : OSCMessages)
				{
					string name = split[1] + ":" + split[2];
					ofxOscMessage m;
					m.setAddress(element.first);
					m.addFloatArg(element.second);
					if (_oscSenders.find(name) != _oscSenders.end())
					{
						_oscSenders[name].sendMessage(m);
					}
				}
				//midi to osc
				for (auto& element : MIDIMessages)
				{
					string name = split[1] + ":" + split[2];
					string channel = "channel" + ofSplitString(element.first, "/")[1];
					string control = "control" + ofSplitString(element.first, "/")[2];
					float value = element.second;
					ofxOscMessage m;
					m.setAddress(channel + "/" + control);
					m.addFloatArg(element.second);
					if (_oscSenders.find(name) != _oscSenders.end())
					{
						_oscSenders[name].sendMessage(m);
					}
				}
				//string to osc
				for (auto& element : stringMessages)
				{
					string name = split[1] + ":" + split[2];
					ofxOscMessage m;
					m.setAddress(element.first);
					for (auto message : element.second) m.addStringArg(message);
					if (_oscSenders.find(name) != _oscSenders.end())
					{
						_oscSenders[name].sendMessage(m);
					}
				}
			}
			//midi
			else
			{
				for (auto& element : MIDIMessages)
				{
					int channel = ofToInt(ofSplitString(element.first, "/")[1]);
					int control = ofToInt(ofSplitString(element.first, "/")[2]);
					int value = element.second * 127;
					if (_MIDIOutputs[output].isOpen())
					{
						_MIDIOutputs[output].sendControlChange(channel, control, value);
					}
				}
			}
		}
		//to module
		else
		{
			for (auto& node : _moduleNodes)
			{
				if (node->getName() == connection.toId)
				{
					for (auto element : MIDIMessages)
					{
 						string port = ofSplitString(element.first, "/")[0];
						int channel = ofToInt(ofSplitString(element.first, "/")[1]);
						int control = ofToInt(ofSplitString(element.first, "/")[2]);
						float value = element.second;
						node->MIDIIn(port, channel, control, value);
					}
					for (auto element : OSCMessages)
					{
						vector<string> vAddress = ofSplitString(element.first, "/");
						bool addressMatch = false;
						string curAddress = element.first;
						if (vAddress.size() > 0)
						{
							if (vAddress[0] == "global") {
								addressMatch = true;
								curAddress = curAddress.substr(7, string::npos);
							}
						}
						if (vAddress.size() > 1)
						{
							string nodeAddress = vAddress[0] + "/" + vAddress[1];
							if (node->getName() == nodeAddress)
							{
								addressMatch = true;
								curAddress = curAddress.substr(nodeAddress.length() + 1, string::npos);
							}
						}
						if (addressMatch)
						{
							node->OSCIn(curAddress, element.second);
						}
					}
				}
			}
		}
	}
	for (auto& node : _moduleNodes) node->clearMessages();
}

void ofApp::clear()
{
	_connections.clear();
	_moduleNodes.clear();
	_outputNodes.clear();
	_inputNodes.clear();

	for (auto port : _MIDIInputs)
	{
		if (port.second.isOpen())
		{
			port.second.closePort();
			port.second.removeListener(this);
			vector<string> split = ofSplitString(port.first, ":");
			_gui->getToggle(split[1], "Midi In")->setChecked(false);
		}
	}
	for (auto port : _MIDIOutputs)
	{
		if (port.second.isOpen())
		{
			port.second.closePort();
			vector<string> split = ofSplitString(port.first, ":");
			_gui->getToggle(split[1], "Midi Out")->setChecked(false);
		}
	}

	//for (auto port : _oscReceivers) port.second.stop();
	//for (auto port : _oscSenders) port.second.clear();
	
	_file = "";
	setWindowTitle("untitled");
}

void ofApp::load()
{
	string path;
	ofFileDialogResult loadFile = ofSystemLoadDialog("Load MNT set", false, _folder);
	if (loadFile.bSuccess)
	{
		_mode = false;
		clear();
		path = loadFile.getPath();
		ofJson jLoad = ofLoadJson(path);
		auto obj = jLoad.get<ofJson::object_t>();
		bool mntFile = false;
		for (auto element : obj)
		{
			if (element.first == "MNT_Version")
			{
				mntFile = true;
				break;
			}
		}
		if (mntFile)
		{
			map<string, string> names;

			//MIDI/OSC
			ofJson jIn = jLoad["in"];
			for (auto& element : jIn)
			{
				string curPort = element["port"].get<string>();
				bool osc = false;
				vector<string> split = ofSplitString(curPort, ":");
				if (split.size() > 0)
				{
					if (split[0] == "osc") osc = true;
				}
				if (osc)
				{
					bool isNumber = (split[1].find_first_not_of("0123456789") == std::string::npos);
					if (isNumber)
					{
						createOscInput(split[1], element["x"], element["y"]);
						names[curPort] = curPort;
					}
				}
				else
				{
					bool portAvailable = false;
					for (auto port : _MIDIInPorts) {
						if (port.first == split[1])
						{
							portAvailable = true;
							break;
						}
					}
					if (portAvailable)
					{
						string name = createMIDIInput(split[1], element["x"], element["y"]);
						names[name] = name;
					}
				}
			}
			//Modules
			ofJson jModules = jLoad["Modules"];
			for (auto& element : jModules)
			{
				if (element["type"].get<string>() == "interpolate") _moduleNodes.push_back(make_unique<ModuleNode<NNIPage>>());
				if (element["type"].get<string>() == "concatenate") _moduleNodes.push_back(make_unique<ModuleNode<CBCSPage>>());
				if (element["type"].get<string>() == "trigger") _moduleNodes.push_back(make_unique<ModuleNode<TriggerPage>>());
				if (element["type"].get<string>() == "draw") _moduleNodes.push_back(make_unique<ModuleNode<RGBPage>>());
				if (element["type"].get<string>() == "gesture") _moduleNodes.push_back(make_unique<ModuleNode<GesturePage>>());
				if (element["type"].get<string>() == "noise") _moduleNodes.push_back(make_unique<ModuleNode<NoiseGenerator>>());
				
				_moduleNodes[_moduleNodes.size() - 1]->setup(
					element["x"],
					element["y"],
					30,
					element["inputs"],
					element["outputs"],
					_verdana,
					_moduleColor);

				_moduleNodes[_moduleNodes.size() - 1]->setName(element["type"].get<string>(), true);

				_moduleNodes[_moduleNodes.size() - 1]->setupPage(1024, 1024, _guiWidth, _colorPallete);
				ofJson data = element["data"];
				_moduleNodes[_moduleNodes.size() - 1]->load(data);
				string oldName = element["type"].get<string>() + "/" + ofToString(element["id"]);
				string newName = _moduleNodes[_moduleNodes.size() - 1]->getName();
				names[oldName] = newName;
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
				connection.toOutputNode = element["toOutputNode"];
				connection.isDump = element["isDump"];

				bool validConnection = true;
				if (connection.fromInputNode)
				{
					bool inputExists = false;
					for (auto port : _MIDIInPorts) if ("in:" + port.first == connection.fromId) inputExists = true;
					if (ofSplitString(connection.fromId, ":")[0] == "osc") inputExists = true;
					//this line was causing crashes randomly, maybe the receiver wasnt created yet?
					//for (auto port : _oscReceivers) if ("osc:" + port.first == connection.fromId) inputExists = true;
					validConnection = validConnection && inputExists;
				}

				if (connection.toOutputNode)
				{
					bool outputExists = false;
					for (auto port : _MIDIOutPorts) if ("out:" + port.first == connection.toId) outputExists = true;
					if (ofSplitString(connection.toId, ":")[0] == "osc") outputExists = true;
					//for (auto port : _oscSenders) if ("osc:" + port.first == connection.toId) outputExists = true;
					validConnection = validConnection && outputExists;
				}
				if (validConnection) _connections.push_back(connection);
			}
			
			//LOAD
			_file = loadFile.getName();
			_folder = ofSplitString(path, _file)[0];
			setWindowTitle(_file);
		}
	}
}

void ofApp::save()
{
	/*
	openframeworks has an old bug that makes default name do nothing in ofSystemSaveDialog on Windows
	*/
	string path;
	ofFileDialogResult saveFile = ofSystemSaveDialog("untitled.mnt", "Save MNT set");
	if (saveFile.bSuccess)
	{
		//MIDI/OSC
		path = saveFile.getPath();
		path = ofSplitString(path, ".")[0];
		path += ".mnt";
		ofJson jSave;
		jSave["MNT_Version"] = 2;
		ofJson jIn, jOut;
		for (auto& node : _inputNodes)
		{
			ofJson input;
			input["port"] = node.getName();
			input["x"] = node.getBox().x;
			input["y"] = node.getBox().y;
			jIn.push_back(input);
		}
		for (auto& node : _outputNodes) {
			ofJson output;
			output["port"] = node.getName();
			output["x"] = node.getBox().x;
			output["y"] = node.getBox().y;
			jOut.push_back(output);
		}
		jSave["in"] = jIn;
		jSave["out"] = jOut;

		//Modules
		ofJson jModules;
		for (auto& node : _moduleNodes)
		{
			ofJson jModule;
			jModule["type"] = ofSplitString(node->getName(), "/")[0];
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
		if (_mode)
		{
			_page += 1;
			if (_page >= _moduleNodes.size()) _page = 0;
			changePage(_page);
		}
		break;
	case('S'):
	case('s'):
	case(19):
		if(_control) save();
		break;
	case('L'):
	case('l'):
	case(12):
		if(_control) load();
		break;
	case(OF_KEY_SHIFT):
		_shift = false;
		_shiftSelected = { "",-1,-1, ofVec2f(-1, -1) };
		break;
	case(OF_KEY_CONTROL):
		_control = false;
		break;
	case(OF_KEY_ESC):
		_mode = false;
		for (auto& node : _moduleNodes)
		{
			_page = -1;
			node->setVisible(false);
		}
		break;
	}
}

void ofApp::mouseMoved(int x, int y){
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
		ofVec2f curPos;
		curPos.x = (x - _selectionOffset.x) / (float)ofGetWidth();
		curPos.y = float(y - _selectionOffset.y) / (float)ofGetHeight();

		for (auto& node : _moduleNodes)
		{
			if (node->getName() == _selected) node->setPosition(curPos.x, curPos.y);
		}
		for (auto& node : _inputNodes)
		{
			if (node.getName() == _selected) node.setPosition(curPos.x, curPos.y);
		}
		for (auto& node : _outputNodes)
		{
			if (node.getName() == _selected) node.setPosition(curPos.x, curPos.y);
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
					tuple<string, int, int, ofVec2f> curSelected = selectNode(x, y);
					if (get<2>(curSelected) > 0) _shiftSelected = curSelected;
					else _shiftSelected = { "", -1 , -1, ofVec2f(-1, -1) };
				}
				else
				{
					tuple<string, int, int, ofVec2f> curSelected = selectNode(x, y);
					bool keep = get<0>(curSelected) != get<0>(_shiftSelected) && get<1>(curSelected) > 0;
					if (keep) createDeleteConnection(_shiftSelected, curSelected, _control);
					if (get<2>(curSelected) > 0) _shiftSelected = curSelected;
					else _shiftSelected = { "", -1 , -1, ofVec2f(-1, -1) };
				}
			}
			else
			{
				string lastSelected = _selected;
				auto curSelected = selectNode(x, y);
				_selected = get<0>(curSelected);
				_selectionOffset = get<3>(curSelected);
				bool doubleClick = _selected != "" && lastSelected == _selected;
				doubleClick = doubleClick && ofGetElapsedTimeMillis() - _lastClick < 500;
				if (doubleClick)
				{
					for (int i = 0; i < _moduleNodes.size(); i++)
					{
						if (_moduleNodes[i]->getName() == _selected)
						{
							_mode = true;
							_moduleNodes[i]->setVisible(true);
							_page = i;
							_pageMarginLeft = _moduleNodes[i]->getPagePosition().x;
							_pageMarginRight = _pageMarginLeft + _moduleNodes[i]->getPageWidth();
						}
						else
						{
							_page = -1;
							_moduleNodes[i]->setVisible(false);
						}
					}
				}
			}
		}
	}
	else
	{
		for (auto& node : _moduleNodes)
		{
			bool doubleClick = false;
			if (button == 0)
			{
				doubleClick = ofGetElapsedTimeMillis() - _lastClick < 300;
			}
			if (node->getVisible()) node->mousePressed(x, y, button, doubleClick);
		}
	}
	if(button == 0) _lastClick = ofGetElapsedTimeMillis();
}

void ofApp::mouseReleased(int x, int y, int button){
	if (_mode)
	{
		for (auto& node : _moduleNodes)
		{
			if (node->getVisible()) node->mouseReleased(x, y, button);
		}
		if (x < _pageMarginLeft - 15 || x > _pageMarginRight + 15)
		{
			if (x < _pageMarginLeft)
			{
				_page--;
				if (_page < 0) _page = _moduleNodes.size() - 1;
			}
			if (x > _pageMarginRight)
			{
				_page++;
				if (_page >= _moduleNodes.size()) _page = 0;
			}
			changePage(_page);
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
			else {
				for (int i = 0; i < _inputNodes.size(); i++)
				{
					if (_inputNodes[i].inside(x, y))
					{
						curSelected = _inputNodes[i].getName();
						vector<string> split = ofSplitString(curSelected, ":");
						bool osc = false;
						curIndex = i;
						if (split.size() > 1)
						{
							if (split[0] == "osc") osc = true;
						}
						if (osc) deleteOscInput(split[1]);
						else deleteMIDIInput(split[1]);
						break;
					}
				}
				if (curIndex == -1)
				{
					for (int i = 0; i < _outputNodes.size(); i++)
					{
						if (_outputNodes[i].inside(x, y))
						{
							curSelected = _outputNodes[i].getName();
							vector<string> split = ofSplitString(curSelected, ":");
							bool osc = false;
							curIndex = i;
							if (split.size() > 1)
							{
								if (split[0] == "osc") osc = true;
							}
							if (osc) deleteOscOutput(split[1], split[2]);
							else deleteMIDIOutput(split[1]);
							break;
						}
					}
				}
			}
			_selected = "";
			_selectionOffset.set(0, 0);
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
	if (w != 0 && h != 0)
	{
		_gui->setPosition(ofGetWidth() - _guiWidth, 20);
		for (auto& node : _moduleNodes)
		{
			node->resizePage(w, h);
			int x = w * float(node->getBox().x) / _lastWidth;
			int y = h * float(node->getBox().y) / _lastHeight;
			//node->setPosition(x, y);
		}
		for (auto& node : _inputNodes)
		{
			int x = w * float(node.getBox().x) / _lastWidth;
			int y = h * float(node.getBox().y) / _lastHeight;
			//node.setPosition(x, y);
		}
		for (auto& node : _outputNodes)
		{
			int x = w * float(node.getBox().x) / _lastWidth;
			int y = h * float(node.getBox().y) / _lastHeight;
			//node.setPosition(x, y);
		}
		if (_moduleNodes.size() > 0)
		{
			_pageMarginLeft = _moduleNodes[0]->getPagePosition().x;
			_pageMarginRight = _pageMarginLeft + _moduleNodes[0]->getPageWidth();
		}
		_lastWidth = w;
		_lastHeight = h;
	}
}

void ofApp::gotMessage(ofMessage msg){

}

void ofApp::dragEvent(ofDragInfo dragInfo){ 

}