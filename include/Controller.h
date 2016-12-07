#pragma once

#include "audioeffectx.h"

class Klein;

class Controller
{
public:
	Controller(Klein &k);
	virtual ~Controller();

	Klein &klein;

	bool processEvent(VstMidiEvent *e);
};

