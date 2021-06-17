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
		isDump = false;
	}
	string fromId, toId;
	bool toOutputNode;
	bool fromInputNode;
	bool isDump;
	int fromOutput, toInput;
};