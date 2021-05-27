#include "ofMain.h"

struct Connection {
	Connection()
	{
		fromId = "";
		fromOutput = NULL;
		toId = "";
		toInput = NULL;
		fromOutputNode = false;
		toOutputNode = false;
		fromInputNode = false;
		toInputNode = false;
	}
	string fromId, toId;
	bool fromOutputNode, toOutputNode;
	bool fromInputNode, toInputNode;
	int fromOutput, toInput;
};