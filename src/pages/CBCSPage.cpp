#include "CBCSPage.h"

CBCSPage::CBCSPage()
{
	setUseGlobalParameters(false);
	setMidiOutput(false);
	setOscOutput(false);
	setStringOutput(true);
	setAddress("cbcs/");
}

void CBCSPage::setup(int width, int height, int guiWidth, int maxMessages)
{
	_map.setup(width, height);
	_guiWidth = guiWidth;

	_position = centerSquarePosition(ofGetWidth() - _guiWidth, ofGetHeight());
	_map.setRadius(0.05);
	_map.setActive(false);
	_map.setRandomize(0.);
	_map.setDrawSelected(true);
	_map.setCursor(ofVec2f(-1, -1));

	_mouseControl = false;
	_controlLearn = false;
	_parameterLearn = false;
	_inside = false;
	_visible = false;
	setupTsne();
	setupGui();
	_maxMessages = maxMessages;
}

void CBCSPage::setupGui()
{
	_gui = new ScrollGui();
	_gui->addHeader("Concatenate", false);
	_gui->addToggle("active");
	_arrangeFolder = _gui->addFolder("Analize");
	_arrangeFolder->addToggle("Analize complete files", false);
	_arrangeFolder->addButton("PCA")->setName("pca");
	_arrangeFolder->addButton("t-SNE")->setName("tsne");
	_arrangeFolder->addSlider("perplexity", 5, 50, _dr.getParameter("--perplexity"))->setName("--perplexity");
	_arrangeFolder->addSlider("learning rate", 10, 1000, _dr.getParameter("--learning_rate"))->setName("--learning_rate");
	_arrangeFolder->addSlider("iterations", 250, 2500, _dr.getParameter("--iterations"))->setName("--iterations");
	_arrangeFolder->addButton("Normalize");
	_arrangeFolder->collapse();
	_controlFolder = _gui->addFolder("Control");
	_controlFolder->addToggle("learn")->setName("controlLearn");
	_controlFolder->addToggle("Mouse Control");
	_controlFolder->addSlider("x", 0., 1.)->setName("x");
	_controlFolder->addSlider("y", 0., 1.)->setName("y");
	_controlFolder->collapse();
	_gui->addSlider("radius", 0, 1, _map.getRadius() * 2);
	_gui->addTextInput("address", getAddress());
	_gui->onButtonEvent(this, &CBCSPage::buttonEvent);
	_gui->onToggleEvent(this, &CBCSPage::toggleEvent);
	_gui->onSliderEvent(this, &CBCSPage::sliderEvent);
	_gui->onTextInputEvent(this, &CBCSPage::textInputEvent);
	_gui->setAutoDraw(false);
	_gui->setOpacity(0.5);
	_gui->setTheme(new ofxDatGuiThemeWireframe(), true);
	_gui->setWidth(_guiWidth, 0.3);
	_gui->setPosition(ofGetWidth() - _guiWidth, 0);
	_gui->setMaxHeight(ofGetHeight());
	_gui->setEnabled(false);
	_gui->setVisible(false);
	_gui->update();
}

void CBCSPage::setupTsne()
{
	_dr.setup("../../analysis/analysis_cbcs.py", "cbcs"); //ver valores por defecto
	map<string, float> drParameters;
	drParameters["--perplexity"] = 30;
	drParameters["--learning_rate"] = 200;
	drParameters["--iterations"] = 1000;
	drParameters["--mode"] = 0;
	_dr.setParameters(drParameters);
}

void CBCSPage::update()
{
	if (_dr.getRunning())
	{
		if (_dr.getCompleted())
		{
			_map.clearPoints();
			load(_dr.getData());
		}
		else _dr.check();
	}

	BasePage::update();
}

void CBCSPage::buttonEvent(ofxDatGuiButtonEvent e)
{
	if (e.target->getName() == "tsne")
	{
		_dr.setParameter("--technique", 0);
		if (!_dr.getRunning()) _dr.start();
	}
	if (e.target->getName() == "pca")
	{
		_dr.setParameter("--technique", 1);
		if (!_dr.getRunning()) _dr.start();
	}
	if (e.target->getName() == "Normalize")
	{
		_map.normalize();
	}
}

void CBCSPage::sliderEvent(ofxDatGuiSliderEvent e)
{
	string name = e.target->getName();
	if (name == "perplexity" || name == "learning rate" || name == "iterations")
	{
		_dr.setParameter(name, e.value);
	}
	else if (name == "x" || name == "y")
	{
		_lastSelectedControl = name;
		if (!_controlLearn)
		{
			ofVec2f nniCursor = _map.getCursor();
			if (name == "x") nniCursor.x = e.value;
			if (name == "y") nniCursor.y = e.value;
			_map.setCursor(nniCursor);
		}
	}
	else if (name == "radius")
	{
		_map.setRadius(e.value * 0.5);
	}
}

void CBCSPage::toggleEvent(ofxDatGuiToggleEvent e)
{
	if (e.target->getName() == "controlLearn") _controlLearn = e.checked;
	if (e.target->getName() == "active") _map.setActive(e.checked);
	if (e.target->getName() == "Mouse Control") _mouseControl = e.checked;
	if (e.target->getName() == "Analize complete files") {
		if (e.checked) _dr.setParameter("mode", 1);
		else _dr.setParameter("mode", 0);
	}
}

void CBCSPage::textInputEvent(ofxDatGuiTextInputEvent e)
{
	_address = e.text;
}

void CBCSPage::mouseMoved(int x, int y)
{
	if (_visible)
	{
		_inside = _position.inside(x, y);
		if (_inside && _mouseControl) _map.setCursor(normalize(ofVec2f(x, y), _position));
	}
}

void CBCSPage::mouseDragged(int x, int y, int button)
{
	_inside = _position.inside(x, y);
	if (_inside)
	{
		if (button == 1)
		{
			//pan
		}
	}
}

void CBCSPage::mousePressed(int x, int y, int button, bool doubleClick)
{
	
}

void CBCSPage::mouseReleased(int x, int y, int button)
{
	_inside = _position.inside(x, y);
	if (_inside)
	{
		if (button == 2)
		{
			_map.removeSelection();
		}
	}
}

void CBCSPage::mouseScrolled(int scroll)
{
	//zoom
}

void CBCSPage::load(ofJson & json)
{
	_map.clearPoints();
	for (ofJson point : json["points"])
	{
		Point curPoint;
		curPoint.setPosition(point["x"], point["y"]);
		curPoint.setName(point["name"].get<string>());
		curPoint.setValue("position", point["pos"]);
		_map.addPoint(curPoint);
	}
	_map.build();
}

ofJson CBCSPage::save()
{
	ofJson jSave;
	vector<Point> points = _map.getPoints();
	for (int i = 0; i < points.size(); i++)
	{
		ofJson curPoint;
		curPoint["x"] = points[i].getPosition().x;
		curPoint["y"] = points[i].getPosition().y;
		curPoint["name"] = points[i].getName();
		curPoint["pos"] = points[i].getValue("position");
		for (auto parameter : points[i].getValues())
		jSave["points"].push_back(curPoint);
	}

	jSave["MIDIMap"]["x"] = _CCXY[0];
	jSave["MIDIMap"]["y"] = _CCXY[1];
	return jSave;
}
