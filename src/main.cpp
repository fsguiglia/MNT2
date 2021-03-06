#include "ofMain.h"
#include "ofApp.h"
#include "../resource.h"

//========================================================================

#if _DEBUG
int main() {
	ofGLWindowSettings settings;
	settings.setGLVersion(3, 2);
	settings.setSize(900, 600);
	ofCreateWindow(settings);

	ofRunApp(new ofApp());
}

#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	//float scale = ((ofAppGLFWWindow *)(ofGetWindowPtr()))->getWindowScale().x;
	ofGLFWWindowSettings settings;
	settings.setGLVersion(3, 2);
	settings.setSize(900, 600);
	ofCreateWindow(settings);
	
	HWND hwnd = ofGetWin32Window();
	HICON hMyIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hMyIcon);

	ofRunApp(new ofApp());
}
#endif