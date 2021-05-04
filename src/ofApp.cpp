#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	//NNI
	_nni.setup(400, 400);
	_nni.addParameter("id", 0.);
	_selNNISite = -1;
	_interpolate = false;
	_random = false;
	_inside = false;
	_NNIx = 50;
	_NNIy = 50;
}

//--------------------------------------------------------------
void ofApp::update(){
	if (_random) _nni.randomize(1);
	if (_interpolate) _weights = _nni.interpolate(_cursor, true);
	for (int i = 0; i < _gSites.size(); i++) {
		_gSites[i]->setVisible(true);
		_gSites[i]->setEnabled(true);
		_gSites[i]->update();
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofClear(50);
	ofSetColor(0);
	ofDrawRectangle(_NNIx - 5, _NNIy - 5, 410, 410);
	_nni.draw(_NNIx, _NNIy);
	if (_interpolate)
	{
		ofDrawBitmapString(ofToString("id") + ": " + ofToString(_weights["id"]), 500, 200);
		//_nni.drawInterpolation(_NNIx + _nni.getWidth(), _NNIy);
	}
	if (_selNNISite >= 0) _gSites[_selNNISite]->draw();
}

void ofApp::addSite(float x, float y, int id)
{
	map<string, float> adHocValues;
	adHocValues["id"] = id;
	_nni.add(adHocValues, ofVec2f(x, y));

	ofxDatGui* gui = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
	gui->addLabel("site: " + ofToString(id));
	gui->addToggle("learn");
	gui->setAutoDraw(false);
	gui->setWidth(100, 0.3);
	gui->setPosition(_NNIx + _nni.getWidth() + 100, _NNIy);
	gui->setTheme(new ofxDatGuiThemeWireframe(), true);
	_gSites.push_back(gui);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == ' ') _interpolate = !_interpolate;
	if (key == 'r') _random = !_random;
	if (key == 'p')
	{
		set<string> parameters = _nni.getParameters();
		cout << parameters.size() << endl;
		for (auto parameter : parameters) cout << parameter << endl;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	_inside = x > _NNIx && x < _NNIx + _nni.getWidth() && y > _NNIy && y < _NNIy + _nni.getHeight();
	float normX = (float)(x - _NNIx) / _nni.getWidth();
	float normY = (float)(y - _NNIy) / _nni.getHeight();
	if (_inside) _cursor.set(normX, normY);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	if (button < 2 && _inside) {
		float normX = (float)(x - _NNIx) / _nni.getWidth();
		float normY = (float)(y - _NNIy) / _nni.getHeight();
		_nni.move(_selNNISite, ofVec2f(normX, normY));
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	if (_inside)
	{
		float normX = (float)(x - _NNIx) / _nni.getWidth();
		float normY = (float)(y - _NNIy) / _nni.getHeight();
		if (button == 0) addSite(normX, normY, _nni.getSites().size());
		if (button < 2) _selNNISite = _nni.getClosest(ofVec2f(normX, normY))[0];
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	float normX = (float)(x - _NNIx) / _nni.getWidth();
	float normY = (float)(y - _NNIy) / _nni.getHeight();
	if (button == 2) _nni.remove(ofVec2f(normX, normY));
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
