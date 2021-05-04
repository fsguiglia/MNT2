#ifndef _TRIGGERMAP
#define _TRIGGERMAP
#include "ofMain.h"
#include "point.h"
#include "trigger.h"
#include "map.h"

class TriggerMap : private Map{
public:
	TriggerMap();
	void addTrigger(map<string, float> parameters, ofVec2f position);
	void removeTrigger(int index);
	void setColor(ofColor color);
	void addParameter(string parameter, float value);
	void removeParameter(string parameter);
	void updateParameters(int index, map<string, float> values);
private:
	vector<Trigger> _triggers;
	ofColor _color;
	set<string> _parameters;
};
#endif