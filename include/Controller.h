#pragma once

#include <vector>
#include <unordered_map>

using namespace std;

#include "audioeffectx.h"

#include "MidiDefs.h"
#include "Command.h"
#include "ParserDriver.h"

class Klein;

/**
 * represents a single mapping between an input and an operation 
 *  - operation is either a built in command or a script
 * inputs could be
 *  - midi program. simplest. no time or wierd value. direct map onto operation
 *  - midi control. immediate, but with a variable control value
 *  - midi note. over time, we have a duration (note on-note off), also double taps and more. context of operation would need to be kept
 *  - midi pitch bend. wide range of control values
 *  - ??? key stroke (would have duration, and possibility of double tap ... like midi note)
 *  - ??? interface operations or mouse gestures
 *  - ??? wii controller
 *
 * other things needed here
 *  - link to compiled for scripts
 *  - context ... ie variable storage for scripts
 *  - possible execution position of a script
 *
 * for things like notes, with a duration and multiple attacks this will need to track
 *  - the initial input time, note down. history of attacks for multitap?
 *  - implicit variables that are automatically defined
 *
 * target would either be
 *  - global
 *  - KleinChannel
 *  - something else??
 * 
 */
struct ControlMapping {
	uchar command;
	int target;
};

/**
 * handles mapping of control inputs (MIDI and eventually maybe other things). 
 * processes the inputs, and executs m-script commands
 */
class Controller
{
public:
	Controller(Klein &k);
	virtual ~Controller();

	bool processEvent(VstMidiEvent *e);

protected:
	Klein &klein;
	unordered_map<int, vector<ControlMapping>> midiMap;

	vector<ControlMapping> * getMidiMap(int);
	int makeMidiHash(char *midiData);
	bool processMapping(ControlMapping &m);
};

