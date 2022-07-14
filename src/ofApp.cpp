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
	_tileIcon.load("tile_icon.png");
	_tileIcon.resize(30, 30);
	_shift = false;
	_connectorSelected = false;
	_mode = false;
	_selected = "";
	_selectionOffset.set(0, 0);
	_shiftSelected = { "",-1,-1, ofVec2f(-1,-1), -1 };
	if (_scale > 1) ofSetWindowShape(ofGetWidth() * _scale, ofGetHeight() * _scale);
	_prevNextButton = false;
	

	//intro
	ofDirectory dir;
	int n = dir.listDir("animated_logo");
	_introSequence.resize(n);
	for (int i = 0; i < n; i++) _introSequence[i].load(dir.getPath(i));
	_intro = true;
}

void ofApp::update() {
	if(!_intro)
	{
		for (auto& node : _moduleNodes) node->update();
		updateConnections();

		_gui->setVisible(_mode != EDIT_MODE);
		_gui->setEnabled(_mode == CONNECT_MODE);
		_gui->update();
	}
}

void ofApp::draw(){
	ofBackground(255);
	if (_intro) drawIntro();
	else
	{
		switch (_mode)
		{
		case EDIT_MODE:
			drawEditMode();
			break;
		case CONNECT_MODE:
			drawConnectionMode();
			break;
		case TILE_MODE:
			if (_moduleNodes.size() > 0)
			{
				drawConnectionMode();
				ofSetColor(0, 185);
				ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
				ofSetColor(0);
				drawTileMode();
			}
			else _mode = CONNECT_MODE;
			break;
		}
	}
}

void ofApp::drawIntro()
{
	static float i = 0;
	if (i < _introSequence.size())
	{
		int x = ((float)ofGetWidth() - _introSequence[int(i)].getWidth()) * 0.5;
		int y = ((float)ofGetHeight() - _introSequence[int(i)].getHeight()) * 0.5;
		_introSequence[int(i)].draw(x, y);
		i+=0.6;
	}
	else
	{
		_intro = false;
		_introSequence.clear();
	}
}

void ofApp::drawConnectionMode()
{
	//draw nodes and connections
	ofPushStyle();
	if(_connectorSelected) drawCurConnection();
	for (auto connection : _connections) drawConnection(connection);
	for (auto& node : _moduleNodes) node->draw();
	for (auto& node : _inputNodes) node.draw();
	for (auto& node : _outputNodes) node.draw();
	if (!_tileIconHovered) ofSetColor(255, 150);
	else ofSetColor(255);
	_tileIcon.draw(ofGetWidth() - (_tileIcon.getWidth() + _tileIconMargin), ofGetHeight() - (_tileIcon.getHeight() + _tileIconMargin));
	ofSetColor(0);
	_gui->draw();
	ofPopStyle();
}

void ofApp::drawEditMode()
{
	//draw selected page
	ofPushStyle();
	ofSetColor(0);
	ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
	for (auto& node : _moduleNodes) if (node->getVisible()) node->drawPage();
	//draw prev next areas
	if (_pageMarginLeft > 80)
	{
		_prevNextButton = true;
		if (mouseX < 40) ofSetColor(35);
		else ofSetColor(0);
		ofDrawRectangle(0, 0, _pageMarginLeft, ofGetHeight());
		if (mouseX > ofGetWidth() - 40) ofSetColor(35);
		else ofSetColor(0);
		ofDrawRectangle(_pageMarginRight, 0, ofGetWidth() - _pageMarginRight, ofGetHeight());
		//draw prev next icons
		ofSetColor(60);
		ofDrawTriangle(
			ofVec2f(40, ofGetHeight() * 0.5 - 30),
			ofVec2f(40, ofGetHeight() * 0.5 + 30),
			ofVec2f(10, ofGetHeight() * 0.5)
		);
		ofDrawTriangle(
			ofVec2f(ofGetWidth() - 40, ofGetHeight() * 0.5 - 30),
			ofVec2f(ofGetWidth() - 40, ofGetHeight() * 0.5 + 30),
			ofVec2f(ofGetWidth() - 10, ofGetHeight() * 0.5)
		);
		ofPopStyle();
	}
	else _prevNextButton = false;
}

void ofApp::drawTileMode()
{
	//only active pages?
	ofPushStyle();
	ofSetColor(0);
	int columns = COLUMNS;
	if (_moduleNodes.size() < COLUMNS) columns = _moduleNodes.size();
	int rows = int(ceil((float)_moduleNodes.size() / columns));
	int cell_width = ofGetWidth() / columns;
	int cell_height = ofGetHeight() / rows;
	int tile_size = cell_width;
	if (cell_height < cell_width) tile_size = cell_height;
	tile_size -= 10;
	int spacing_x = int(floor((float)(ofGetWidth() - columns * tile_size) / (columns + 1)));
	int spacing_y = int(floor((float)(ofGetHeight() - rows * tile_size) / (rows + 1)));
	int index = 0;

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			if (index < _moduleNodes.size())
			{
				_moduleNodes[index]->drawTile(
					j * tile_size + spacing_x * (j + 1),
					i * tile_size + spacing_y * (i + 1),
					tile_size,
					tile_size,
					4
				);
			}
			index++;
		}
	}
	ofPopStyle();
}

void ofApp::drawCurConnection()
{
	ofRectangle output;
	bool valid = false;
	
	for (auto& node : _moduleNodes)
	{
		if (get<0>(_shiftSelected) == node->getName()) {
			output = node->getOutputConnector(0);
			valid = true;
			break;
		}
	}
	for (auto& node : _inputNodes)
	{
		if (get<0>(_shiftSelected) == node.getName()) {
			output = node.getOutputConnector(0);
			valid = true;
			break;
		}
	}
	if (valid)
	{
		ofSetColor(0);
		ofDrawLine(output.x + output.width, output.y + output.height * 0.5, mouseX, mouseY);
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
	//out.x += out_rect.getWidth() * 0.5;
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
	createModule(label, 0.5, 0.5);
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

int ofApp::createModule(string type, float x, float y)
{
	int index = setupModule(type, x, y);
	setupModulePage(index);
	return index;
}

int ofApp::createModule(string type, float x, float y, int id)
{
	int index = setupModule(type, x, y);

	bool updateId = _moduleNodes[index]->getId() <= id;
	_moduleNodes[index]->setId(id);
	_moduleNodes[index]->setName(type, true);
	//There are two ids one in node accesible trough set and get and a static one in ModuleNode...
	if (updateId)
	{
		if (type == "interpolate") ModuleNode<NNIPage>::ID = id;
		if (type == "concatenate") ModuleNode<CBCSPage>::ID = id;
		if (type == "trigger") ModuleNode<TriggerPage>::ID = id;
		if (type == "draw") ModuleNode<RGBPage>::ID = id;
		if (type == "gesture") ModuleNode<GesturePage>::ID = id;
		if (type == "noise") ModuleNode<NoiseGenerator>::ID = id;
	}

	setupModulePage(index);
	return index;
}

int ofApp::setupModule(string type, float x, float y)
{
	type = ofToLower(type);
	if (type == "interpolate") _moduleNodes.push_back(make_unique<ModuleNode<NNIPage>>());
	if (type == "concatenate") _moduleNodes.push_back(make_unique<ModuleNode<CBCSPage>>());
	if (type == "trigger") _moduleNodes.push_back(make_unique<ModuleNode<TriggerPage>>());
	if (type == "draw") _moduleNodes.push_back(make_unique<ModuleNode<RGBPage>>());
	if (type == "gesture") _moduleNodes.push_back(make_unique<ModuleNode<GesturePage>>());
	if (type == "noise") _moduleNodes.push_back(make_unique<ModuleNode<NoiseGenerator>>());

	int index = _moduleNodes.size() - 1;
	_moduleNodes[index]->setup(x, y, _moduleNodeHeight, 1, 1, _verdana, _moduleColor);
	_moduleNodes[index]->setName(type, true);
	_moduleNodes[index]->setPosition(
		(ofGetWidth() - _moduleNodes[index]->getWidth()) * 0.5 / (float)ofGetWidth(),
		(ofGetHeight() - _moduleNodes[index]->getHeight()) * 0.5 / (float)ofGetHeight()
	);
	return index;
}



void ofApp::setupModulePage(int index)
{
	_moduleNodes[index]->setupPage(
		_moduleNodes[index]->getName(),
		_moduleSize,
		_moduleSize,
		_guiWidth,
		_verdana,
		_colorPallete
	);
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
	node.setup(x, y, 30, 0, 1, _verdana, _ioColor);
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
		bool isNumber = (e.text.find_first_not_of("0123456789") == std::string::npos) && e.text.size() > 0;
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
			bool isNumber = (split[1].find_first_not_of("0123456789") == std::string::npos) && split[1].size() > 0;
			if (isNumber)
			{
				createOscOutput(split[0], split[1], 0.85, 0.5);
				_outputNodes[_outputNodes.size() - 1].setPosition(
					0.85 - (_outputNodes[_outputNodes.size() - 1].getWidth() / (float)ofGetWidth()),
					(ofGetHeight() - _outputNodes[_outputNodes.size() - 1].getHeight()) * 0.5 / (float)ofGetHeight()
				);
			}
		}
		else if (split.size() == 1)
		{
			bool isNumber = (split[0].find_first_not_of("0123456789") == std::string::npos) && split[0].size() > 0;
			if (isNumber)
			{
				createOscOutput("127.0.0.1", split[0], 0.85, 0.5);
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


tuple<string, int, int, ofVec2f, int> ofApp::selectNode(int x, int y)
{
	tuple<string, int, int, ofVec2f, int> parameters = { "", -1, -1, ofVec2f(-1, -1), -1};
	for (int i = _moduleNodes.size() - 1; i >= 0; i--)
	{
		int inside = _moduleNodes[i]->inside(x, y, true);
		if (inside >= 0)
		{
			get<0>(parameters) = _moduleNodes[i]->getName();
			get<1>(parameters) = _moduleNodes[i]->getInputs();
			get<2>(parameters) = _moduleNodes[i]->getOutputs();
			get<4>(parameters) = inside;
			ofVec2f offset;
			offset.x = x - _moduleNodes[i]->getPosition().x * ofGetWidth();
			offset.y = y - _moduleNodes[i]->getPosition().y * ofGetHeight();
			get<3>(parameters) = offset;
			break;
		}
	}
	for (int i = _inputNodes.size() - 1; i >= 0; i--)
	{
		int inside = _inputNodes[i].inside(x, y, true);
		if (inside >= 0)
		{
			get<0>(parameters) = _inputNodes[i].getName();
			get<1>(parameters) = _inputNodes[i].getInputs();
			get<2>(parameters) = _inputNodes[i].getOutputs();
			get<4>(parameters) = inside;
			ofVec2f offset;
			offset.x = x - _inputNodes[i].getPosition().x * ofGetWidth();
			offset.y = y - _inputNodes[i].getPosition().y * ofGetHeight();
			get<3>(parameters) = offset;
			break;
		}
	}
	for (int i = _outputNodes.size() - 1; i >= 0; i--)
	{
		int inside = _outputNodes[i].inside(x, y, true);
		if (inside >= 0)
		{
			get<0>(parameters) = _outputNodes[i].getName();
			get<1>(parameters) = _outputNodes[i].getInputs();
			get<2>(parameters) = _outputNodes[i].getOutputs();
			get<4>(parameters) = inside;
			ofVec2f offset;
			offset.x = x - _outputNodes[i].getPosition().x * ofGetWidth();
			offset.y = y - _outputNodes[i].getPosition().y * ofGetHeight();
			get<3>(parameters) = offset;
			break;
		}
	}
	return parameters;
}

void ofApp::createDeleteConnection(tuple<string, int, int, ofVec2f, int> out, tuple<string, int, int, ofVec2f, int> in, bool dump)
{
	bool connectionExists = false;
	for (int i = 0; i < _connections.size(); i++)
	{
		if (_connections[i].fromId == get<0>(out) && _connections[i].toId == get<0>(in))
		{
			connectionExists = true;
			//_connections.erase(_connections.begin() + i);
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
		map<string, float> MIDIMessages;
		map<string, vector<float>> OSCMessages;
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
								OSCMessages[message.getAddress()].push_back(message.getArgAsFloat(0));
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
						if (node->getOscOutput()) {
							vector<pair<string, float>>& curOutput = node->getOSCOut();
							for (auto& message : curOutput) OSCMessages[message.first].push_back(message.second);
						}
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
					if (element.second.size() > 0)
					{
						for (auto& value : element.second) m.addFloatArg(value);
						if (_oscSenders.find(name) != _oscSenders.end()) _oscSenders[name].sendMessage(m);
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
					int channel = ofToInt(ofSplitString(element.first, "/")[2]);
					int control = ofToInt(ofSplitString(element.first, "/")[3]);
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
								if(curAddress.length() >= 7) curAddress = curAddress.substr(7, string::npos);
							}
						}
						if (vAddress.size() > 1)
						{
							string nodeAddress = vAddress[0] + "/" + vAddress[1];
							if (node->getName() == nodeAddress)
							{
								addressMatch = true;
								if (curAddress.length() >= nodeAddress.length() + 1)
								{
									curAddress = curAddress.substr(nodeAddress.length() + 1, string::npos);
								}
							}
						}
						if (addressMatch)
						{
							node->OSCIn(curAddress, element.second[0]);
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

	ModuleNode<NNIPage>::ID = 0;
	ModuleNode<CBCSPage>::ID = 0;
	ModuleNode<TriggerPage>::ID = 0;
	ModuleNode<RGBPage>::ID = 0;
	ModuleNode<GesturePage>::ID = 0;
	ModuleNode<NoiseGenerator>::ID = 0;

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

	_MIDIInputs.clear();
	_MIDIOutputs.clear();
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
		_mode = CONNECT_MODE;
		clear();
		path = loadFile.getPath();
		ofJson jLoad = ofLoadJson(path);
		bool mntFile = false;

		//is it a mnt file
		if (path.substr(path.size() - 3, string::npos) == "mnt")
		{
			auto obj = jLoad.get<ofJson::object_t>();
			for (auto element : obj)
			{
				if (element.first == "MNT_Version")
				{
					mntFile = true;
					break;
				}
			}
		}
		//load
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
					}
				}
			}
			ofJson jOut = jLoad["out"];
			for (auto& element : jOut)
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
					bool isNumber = (split[2].find_first_not_of("0123456789") == std::string::npos);
					if (isNumber)
					{
						createOscOutput(split[1], split[2], element["x"], element["y"]);
					}
				}
				else
				{
					bool portAvailable = false;
					for (auto port : _MIDIOutPorts) {
						if (port.first == split[1])
						{
							portAvailable = true;
							break;
						}
					}
					if (portAvailable)
					{
						string name = createMIDIOutput(split[1], element["x"], element["y"]);
					}
				}
			}
			//Modules
			ofJson jModules = jLoad["Modules"];
			for (auto& element : jModules)
			{
				int index = createModule(
					element["type"],
					element["x"],
					element["y"],
					element["id"]);
				
				ofJson data = element["data"];
				_moduleNodes[index]->load(data);
				//call draw so position is updated, maybe node should have an update method
				_moduleNodes[index]->draw();
			}
			//CONNECTIONS
			ofJson jConnections = jLoad["Connections"];
			for (auto& element : jConnections)
			{

				Connection connection;
				connection.fromId = element["fromId"].get<string>();
				connection.toId = element["toId"].get<string>();
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
		vector<string> split = ofSplitString(path, ".");
		if(split[split.size() - 1] != "mnt") path += ".mnt";
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
	case('L'):
	case('l'):
	case(12):
		if (_control) load();
		break;
	case('S'):
	case('s'):
	case(19):
		if(_control) save();
		break;
	case(OF_KEY_TAB):
		if (_mode == EDIT_MODE)
		{
			_page += 1;
			if (_page >= _moduleNodes.size()) _page = 0;
			changePage(_page);
		}
		break;
	case(OF_KEY_SHIFT):
		_shift = false;
		break;
	case(OF_KEY_CONTROL):
		_control = false;
		break;
	case(OF_KEY_ESC):
		_mode = CONNECT_MODE;
		for (auto& node : _moduleNodes)
		{
			_page = -1;
			node->setVisible(false);
		}
		_tileIconHovered = false;
		break;
	}
}

void ofApp::mouseMoved(int x, int y){
	switch (_mode)
	{
	case CONNECT_MODE:
		//module
		selectNode(x, y);
		//tile
		_tileIconHovered = x > ofGetWidth() - (_tileIcon.getWidth() + _tileIconMargin);
		_tileIconHovered = _tileIconHovered && x < ofGetWidth() - _tileIconMargin;
		_tileIconHovered = _tileIconHovered && y > ofGetHeight() - (_tileIcon.getHeight() + _tileIconMargin);
		_tileIconHovered = _tileIconHovered && y < ofGetHeight() - _tileIconMargin;
		break;
	case EDIT_MODE:
		for (auto& node : _moduleNodes)
		{
			if (node->getVisible()) node->mouseMoved(x, y);
		}
		break;
	case TILE_MODE:
		break;
	}
}

void ofApp::mouseDragged(int x, int y, int button){
	switch (_mode)
	{
	case CONNECT_MODE:
	{
		ofVec2f curPos;
		curPos.x = (x - _selectionOffset.x) / (float)ofGetWidth();
		curPos.y = float(y - _selectionOffset.y) / (float)ofGetHeight();
		bool valid;
		//node width is unknown, use height instead
		valid = x - _selectionOffset.x < ofGetWidth() - _moduleNodeHeight && y - _selectionOffset.y < ofGetHeight() - _moduleNodeHeight;
		valid = valid && curPos.x > 0 && curPos.y > 0;

		if (valid)
		{
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
		break;

	case EDIT_MODE:
		for (auto& node : _moduleNodes)
		{
			if (node->getVisible()) node->mouseDragged(x, y, button);
		}
		break;

	case TILE_MODE:
		break;
	}
}

void ofApp::mousePressed(int x, int y, int button){
	switch (_mode)
	{
	case CONNECT_MODE:
		if (button == 0)
		{
			if (_connectorSelected)
			{
				_selected = "";
				if (get<0>(_shiftSelected) == "")
				{
					tuple<string, int, int, ofVec2f, int> curSelected = selectNode(x, y);
					if (get<4>(curSelected) > 0) _shiftSelected = curSelected;
				}
				else
				{
					tuple<string, int, int, ofVec2f, int> curSelected = selectNode(x, y);
					bool keep = get<0>(curSelected) != get<0>(_shiftSelected) && get<1>(curSelected) > 0;
					if (keep) createDeleteConnection(_shiftSelected, curSelected, _control);
				}
				_connectorSelected = false;
				_shiftSelected = { "", -1 , -1, ofVec2f(-1, -1), -1 };
			}
			else
			{
				string lastSelected = _selected;
				auto curSelected = selectNode(x, y);
				_connectorSelected = get<4>(curSelected) > 0;
				if (_connectorSelected) _shiftSelected = curSelected;
				else _selected = get<0>(curSelected);
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
		break;

	case EDIT_MODE:
		for (auto& node : _moduleNodes)
		{
			bool doubleClick = false;
			if (button == 0)
			{
				doubleClick = ofGetElapsedTimeMillis() - _lastClick < 300;
			}
			if (node->getVisible()) node->mousePressed(x, y, button, doubleClick);
		}
		break;

	case TILE_MODE:
		break;
	}

	if(button == 0) _lastClick = ofGetElapsedTimeMillis();
}

void ofApp::mouseReleased(int x, int y, int button){
	if (_intro)
	{
		_intro = false;
		_introSequence.clear();
	}
	switch (_mode)
	{
	case CONNECT_MODE:
		if (button == 2)
		{
			auto node = selectNode(x, y);
			string curSelected = get<0>(node);
			int mode = get<4>(node);
		
			if (mode != -1)
			{
				vector<int> deleteConnection;
				for (int i = 0; i < _connections.size(); i++)
				{
					if (mode == 0)
					{
						if (_connections[i].fromId == curSelected || _connections[i].toId == curSelected)
						{
							deleteConnection.push_back(i);
						}
					}
					else
					{
						if (_connections[i].fromId == curSelected)
						{
							deleteConnection.push_back(i);
						}
					}
				}
				if (deleteConnection.size() > 0)
				{
					for (int i = deleteConnection.size() - 1; i >= 0; i--)
					{
						_connections.erase(_connections.begin() + deleteConnection[i]);
					}
				}
				else
				{
					int index = -1;
					for (int i = 0; i < _moduleNodes.size(); i++)
					{
						if (_moduleNodes[i]->getName() == curSelected)
						{
							index = i;
							break;
						}
					}
					if(index != -1) _moduleNodes.erase(_moduleNodes.begin() + index);
					else
					{
						for (int i = 0; i < _inputNodes.size(); i++)
						{
							if (_inputNodes[i].getName() == curSelected)
							{
								index = i;
								break;
							}
						}
						if (index != -1)
						{
							vector<string> split = ofSplitString(curSelected, ":");
							bool osc = false;
							if (split.size() > 1)
							{
								if (split[0] == "osc") osc = true;
							}
							if (osc) deleteOscInput(split[1]);
							else deleteMIDIInput(split[1]);
						}
						else
						{
							for (int i = 0; i < _outputNodes.size(); i++)
							{
								if (_outputNodes[i].getName() == curSelected)
								{
									index = i;
									break;
								}
							}
							if (index != -1)
							{
								vector<string> split = ofSplitString(curSelected, ":");
								bool osc = false;
								if (split.size() > 1)
								{
									if (split[0] == "osc") osc = true;
								}
								if (osc) deleteOscOutput(split[1],split[2]);
								else deleteMIDIOutput(split[1]);
							}
						}
					}
				}
			}
			_selected = "";
			_selectionOffset.set(0, 0);
		}
		//tile icon
		if (button == 0 && _tileIconHovered && _moduleNodes.size() > 0)
		{
			if (_mode == TILE_MODE) _mode = CONNECT_MODE;
			else _mode = TILE_MODE;
			for (auto& node : _moduleNodes) node->setVisible(false);
		}
		break;

	case EDIT_MODE:
		for (auto& node : _moduleNodes)
		{
			if (node->getVisible()) node->mouseReleased(x, y, button);
		}
		if (_prevNextButton)
		{
			if (x < 40 || x > ofGetWidth() - 40)
			{
				if (x < 40)
				{
					_page--;
					if (_page < 0) _page = _moduleNodes.size() - 1;
				}
				if (x > ofGetWidth() - 40)
				{
					_page++;
					if (_page >= _moduleNodes.size()) _page = 0;
				}
				changePage(_page);
			}
		}
		break;

	case TILE_MODE:
		break;
	}
}

void ofApp::mouseScrolled(ofMouseEventArgs& mouse)
{
	switch (_mode)
	{
	case CONNECT_MODE:
		break;
	case EDIT_MODE:
		for (auto& node : _moduleNodes)
		{
			if (node->getVisible()) node->mouseScrolled(mouse.scrollY * 5);
		}
		break;
	case TILE_MODE:
		break;
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