#pragma once
#include "ofMain.h"
#include "moduleInterface.h"

template<typename T> class ModuleNode : public ModuleInterface {
public:
	ModuleNode();
	~ModuleNode();

	void setupPage(string name, int w, int h, int guiWidth, ofTrueTypeFont font, vector<ofColor> colorPalette);
	void setPageHeader(string label);
	void update();
	void drawPage();
	void drawTile(int x, int y, int w, int h, int margin);
	void resizePage(int w, int h);

	ofVec2f getPagePosition();
	int getPageHeight();
	int getPageWidth();

	bool getListeningMouse();
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button, bool doubleClick = false);
	void mouseReleased(int x, int y, int button);
	void mouseScrolled(int scroll);

	bool getMidiOutput();
	bool getOscOutput();
	bool getStringOutput();

	void MIDIIn(string port, int control, int channel, float value);
	void OSCIn(string address, float value);
	vector<pair<string, float>> getOSCOut();
	vector<pair<string, float>> getMidiOut();
	vector<pair<string, float>> getMidiDump();
	string getAddress();
	vector<string> getStringOut();
	void clearMessages();

	void setVisible(bool visible);
	bool getVisible();
	
	void load(ofJson& json);
	ofJson save();

	static int ID;

private:
	T* _page;
	bool _visible;
};


template<typename T>
int ModuleNode<T>::ID = 0;

template<typename T>
inline ModuleNode<T>::ModuleNode()
{
	_page = new T();
	_visible = false;
	setId(++ID);
}

template<typename T>
inline ModuleNode<T>::~ModuleNode()
{
	delete _page;
}

template<typename T>
inline void ModuleNode<T>::setupPage(string name, int w, int h, int guiWidth, ofTrueTypeFont font, vector<ofColor> colorPalette)
{
	_page->setup(name, w, h, guiWidth, font);
	_page->setColorPallete(colorPalette);
	_page->setVisible(false);
}

template<typename T>
inline void ModuleNode<T>::setPageHeader(string label)
{
	_page->setHeader(label);
}

template<typename T>
inline void ModuleNode<T>::update()
{
	_page->setVisible(_visible);
	_page->update();
}

template<typename T>
inline void ModuleNode<T>::drawPage()
{
	_page->draw();
}

template<typename T>
inline void ModuleNode<T>::drawTile(int x, int y, int w, int h, int margin)
{
	_page->drawTile(x, y, w, h, margin);
}

template<typename T>
inline void ModuleNode<T>::resizePage(int w, int h)
{
	_page->resize(w, h);
}

template<typename T>
inline ofVec2f ModuleNode<T>::getPagePosition()
{
	return _page->getPosition();
}

template<typename T>
inline int ModuleNode<T>::getPageHeight()
{
	return _page->getHeight();
}

template<typename T>
inline int ModuleNode<T>::getPageWidth()
{
	return _page->getWidth();
}

template<typename T>
inline bool ModuleNode<T>::getListeningMouse()
{
	return _page->getListeningMouse();
}

template<typename T>
inline void ModuleNode<T>::mouseMoved(int x, int y)
{
	_page->mouseMoved(x, y);
}

template<typename T>
inline void ModuleNode<T>::mouseDragged(int x, int y, int button)
{
	_page->mouseDragged(x, y, button);
}

template<typename T>
inline void ModuleNode<T>::mousePressed(int x, int y, int button, bool doubleClick)
{
	_page->mousePressed(x, y, button, doubleClick);
}

template<typename T>
inline void ModuleNode<T>::mouseReleased(int x, int y, int button)
{
	_page->mouseReleased(x, y, button);
}

template<typename T>
inline void ModuleNode<T>::mouseScrolled(int scroll)
{
	_page->mouseScrolled(scroll);
}

template<typename T>
inline bool ModuleNode<T>::getMidiOutput()
{
	return _page->getMidiOutput();
}

template<typename T>
inline bool ModuleNode<T>::getOscOutput()
{
	return _page->getOscOutput();
}

template<typename T>
inline bool ModuleNode<T>::getStringOutput()
{
	return _page->getStringOutput();
}

template<typename T>
inline void ModuleNode<T>::MIDIIn(string port, int control, int channel, float value)
{
	_page->MIDIIn(port, control, channel, value);
}

template<typename T>
inline void ModuleNode<T>::OSCIn(string address, float value)
{
	_page->OSCIn(address, value);
}

template<typename T>
inline vector<pair<string, float>> ModuleNode<T>::getOSCOut()
{
	return _page->getOscOut();
}

template<typename T>
inline vector<pair<string, float>> ModuleNode<T>::getMidiOut()
{
	return _page->getMidiOut();
}

template<typename T>
inline vector<pair<string, float>> ModuleNode<T>::getMidiDump()
{
	return _page->getMidiDump();
}

template<typename T>
inline string ModuleNode<T>::getAddress()
{
	return _page->getAddress();
}

template<typename T>
inline vector<string> ModuleNode<T>::getStringOut()
{
	return _page->getStringOut();
}

template<typename T>
inline void ModuleNode<T>::clearMessages()
{
	_page->clearMessages();
}

template<typename T>
inline void ModuleNode<T>::setVisible(bool visible)
{
	_visible = visible;
}

template<typename T>
inline bool ModuleNode<T>::getVisible()
{
	return _visible;
}

template<typename T>
inline void ModuleNode<T>::load(ofJson & json)
{
	_page->load(json);
}

template<typename T>
inline ofJson ModuleNode<T>::save()
{
	return _page->save();
}
