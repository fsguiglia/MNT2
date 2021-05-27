#include "ofMain.h"

struct Connection {
	Connection()
	{
		fromId = "";
		fromOutput = NULL;
		toId = "";
		toInput = NULL;
		toOutputNode = false;
		fromInputNode = false;
	}
	string fromId, toId;
	bool toOutputNode;
	bool fromInputNode;
	int fromOutput, toInput;
};