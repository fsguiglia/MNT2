#ifndef _RGB
#define _RGB
#include "ofMain.h"
#include "point.h"

class RGBPoint : public Point {
public:
	RGBPoint();
	RGBPoint(int width, int height);
	void setImage(ofImage img);
	void setImage(ofImage img, string path);
	ofImage getImage();
	string getImagePath();
	float getAverageColor(ofVec2f position, float radius);
	int getHeight();
	int getWidth();
	void setSize(int w, int h);
	void setTrigger(bool isTrigger);
	bool getTrigger();
	bool getState();
	void draw(int x, int y, int w, int h);
	void draw(int x, int y);

private:
	bool _isTrigger, _state; 
	int _width, _height;
	ofImage _img;
	string _imgPath;
};

#endif