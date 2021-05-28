#include "ofMain.h"

struct Connection {
	Connection()
	{
		_fromId = "";
		_fromOutput = NULL;
		_toId = "";
		_toInput = NULL;
	}
	string _fromId, _toId;
	int _fromOutput, _toInput;
};