#pragma once

#include <vector>
#include <unordered_map>

using namespace std;

#include "audioeffectx.h"

#include "mwdefs.h"

#include "MidiDefs.h"
#include "Command.h"
#include "ParserDriver.h"
#include "KLFContext.h"

class Klein;
class KLFun;

typedef int cmd_id_t;
typedef int ctl_id_t;
typedef int tgt_id_t;

/**
 * represents a single mapping between an input and an operation 
 *  - operation is a built in command (CommandMapping), a controller/variable mapping (ControlMapping), or a KLF Script (ScriptMapping)
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
struct CommandMapping {
	cmd_id_t command;
	tgt_id_t target;

	time_t attack;
	time_t lastUpdate;
};

struct ControlMapping {
	ctl_id_t control;
	tgt_id_t target;
};

// every script mapping should have a separate context, so this should inherit from whatever holds the call stack for scripts
// todo 
struct ScriptMapping: public KLFContext {
	KLFun *script;
	tgt_id_t targetId;

	time_t attack;
	time_t lastUpdate;
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

	void addCommandMapping(CommandMapping & mapping, int mdicmd, int channel, int which);
	void addControlMapping(ControlMapping & mapping, int mdicmd, int channel, int which);
	void addScriptMapping(ScriptMapping & mapping, int mdicmd, int channel, int which);
	status_t loadScript(const char *id, const char *src);

protected:
	Klein &klein;
	unordered_map<int, vector<CommandMapping>> cmdMap;
	unordered_map<int, vector<ControlMapping>> ctlMap;
	unordered_map<int, vector<ScriptMapping>> klfMap;

	vector<CommandMapping> * getCommandMap(int);
	vector<ControlMapping> * getControlMap(int);
	vector<ScriptMapping> * getScriptMap(int);

	int makeMidiHash(int cmd, int chan, int which);
	bool processMapping(CommandMapping &m);
	bool processMapping(ControlMapping &m, int v);
	bool processMapping(ScriptMapping &m, int wh, int v);

};

