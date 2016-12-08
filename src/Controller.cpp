#include <iostream>

#include "Controller.h"
#include "Klein.h"


Controller::Controller(Klein &k)
	: klein(k)
{
}


Controller::~Controller()
{
}

bool
Controller::processEvent(VstMidiEvent * e)
{
	char* midiData = e->midiData;
	int cmd = midiCmd(midiData);	
	int chan = midiChan(midiData);
//	int hash = makeIdxHash(midiData);
	cerr << "command " << cmd << " in klein" << endl;
	switch (cmd) {
	case MIDI_NOTE_OFF:
	case MIDI_NOTE_ON: {
		vector<ControlMapping> *cmv = getMap(makeMidiHash(midiData));
		if (cmv != nullptr) {
			for (auto cmi : *cmv) {

			}
		}
		break;
	}
	case MIDI_CTRL: {
		vector<ControlMapping> *cmv = getMap(makeMidiHash(midiData));
		if (cmv != nullptr) {
			for (auto cmi : *cmv) {
				
			}
		}
		break;
	}
	case MIDI_PROG: {
		vector<ControlMapping> *cmv = getMap(makeMidiHash(midiData));
		if (cmv != nullptr) {
			for (auto cmi : *cmv) {
				processMapping(cmi);
			}
		}
		break;
	}
	case MIDI_BEND:
		break;
	}
	return true;
}

bool Controller::processMapping(ControlMapping & m)
{
	switch (m.command) {

	}
	return false;
}

/**
 *
 */
vector<ControlMapping> * Controller::getMap(int hash)
{
	auto ci = maps.find(hash);
	if (ci == maps.end()) {
		return nullptr;
	}

	return &ci->second;
}

/**
 * hash function into our unordered map. no type checking atm, assume it is a midi control with at least 2 bytes
 */
int Controller::makeMidiHash(char *midiData) {
	return midiData[0] | (midiData[1] << 8);
}
