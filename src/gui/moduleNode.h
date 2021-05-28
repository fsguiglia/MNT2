#ifndef _MODULENODE
#define _MODULENODE
#include "ofMain.h"
#include "moduleInterface.h"

template<typename T> class ModuleNode : public ModuleInterface {
public:
	ModuleNode();
	~ModuleNode();

	void setupPage(int w, int h, int guiWidth, vector<ofColor> colorPalette);
	void update();
	void drawPage(ofTrueTypeFont& font);
	void resizePage(int w, int h);

	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseScrolled(int scroll);

	void MIDIIn(string port, int control, int channel, float value);
	map<string, float> getMidiout();
	void clearMIDIMessages();

	void setVisible(bool visible);
	bool getVisible();
	
	void load(ofJson& json);
	ofJson save();

private:
	T* _page;
	bool _visible;
	static int ID;
};
#endif

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
inline void ModuleNode<T>::setupPage(int w, int h, int guiWidth, vector<ofColor> colorPalette)
{
	_page->setup(w, h, guiWidth);
	_page->setMapColorPallete(colorPalette);
	_page->setVisible(false);
}

template<typename T>
inline void ModuleNode<T>::update()
{
	_page->setVisible(_visible);
	_page->update();
}

template<typename T>
inline void ModuleNode<T>::drawPage(ofTrueTypeFont & font)
{
	_page->draw(font);
}

template<typename T>
inline void ModuleNode<T>::resizePage(int w, int h)
{
	_page->resize(w, h);
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
inline void ModuleNode<T>::mousePressed(int x, int y, int button)
{
	_page->mousePressed(x, y, button);
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
inline void ModuleNode<T>::MIDIIn(string port, int control, int channel, float value)
{
	_page->MIDIIn(port, control, channel, value);
}

template<typename T>
inline map<string, float> ModuleNode<T>::getMidiout()
{
	return _page->getMidiOut();
}

template<typename T>
inline void ModuleNode<T>::clearMIDIMessages()
{
	_page->clearMIDIMessages();
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
