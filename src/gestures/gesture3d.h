#ifndef _GESTURE3D
#define _GESTURE3D
#include "ofMain.h"
#include "gesture.h"

class Gesture3d : public Gesture {
public:
	void update();
	void draw();

	void start();
	void end();
	void clear();
private:
	ofPolyline _poly;
};

#endif

