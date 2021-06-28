#include "ofMain.h"
#include "ofApp.h"
#include "../resource.h"

//========================================================================


int main( ){
	ofGLWindowSettings settings;
	settings.setGLVersion(3, 2);
	settings.setSize(900, 600);
	ofCreateWindow(settings);
	ofRunApp(new ofApp());
}

/*
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	ofGLFWWindowSettings settings;
	settings.setGLVersion(3, 2);
	settings.setSize(900, 600);
	settings.resizable = false;
	ofCreateWindow(settings);

	HWND hwnd = ofGetWin32Window();
	HICON hMyIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hMyIcon);

	ofRunApp(new ofApp());
}
*/