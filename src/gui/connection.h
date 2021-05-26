#include "ofMain.h"

struct Connection {
	Connection()
	{
		_fromId = NULL;
		_fromOutput = NULL;
		_toId = NULL;
		_toInput = NULL;
	}
	int _fromId, _fromOutput, _toId, _toInput;
};