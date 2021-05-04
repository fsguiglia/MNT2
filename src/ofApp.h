#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "ofxXmlSettings.h"
#include "ofxNetwork.h"
#include "ofxOsc.h"
#include "ofxMidi.h"
#include "NNI.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void addSite(float x, float y, int id);

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		//NNI
		NNI _nni;
		int _selNNISite, _NNIx, _NNIy;
		bool _interpolate, _random, _inside;
		map<string, float> _weights;
		ofVec2f _cursor;

		//GUI
		vector<ofxDatGui*> _gSites;

		
};
