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

#include "vstgui.h"

class Klein;
class KLFun;

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
enum MappingType {
	kMapCommand = 0,
	kMapControl = 1,
	kMapScript = 2
};

/**
 * values for tgt_id_t.
 * ???? xxxx fixme assuming for the moment that target will not be too large for a 6 bit unsigned quantity
 */
enum {
	kTargetGlobal = 0x3f,
	kTargetAllTracks = 0x3e
};

/***
* vstgui tag
*
*  @param mapIdx Mapping index
*  @param mapType (command, control or script)
*  @param targetType (global, all track, specific track)
*	@param track track number if from or for a specific track
*/
inline long makeTag(const int mapInd, const MappingType mapType, tgt_id_t target) {
	return (mapInd << 8) | ((mapType & 0x3) << 6) | (target & 0x3F);
}
inline void parseTag(const long tag, int &mapInd, MappingType &mapType, tgt_id_t &target) {
	target = (tag & 0x3F);
	mapType = (MappingType)((tag >> 6) & 0x3);
	mapInd = ((tag >> 8) & 0x00ffffff);
}
inline void setTagTargetId(long &t, tgt_id_t target) {
	t = (t & 0xffffffc0) | (target & 0x3f);
}

struct CommandMapping {
	CommandMapping() : CommandMapping(0, -1) {}
	CommandMapping(cmd_id_t i, tgt_id_t t) : command(i), target(t), attack(0), lastUpdate(0) {}

	cmd_id_t command;
	tgt_id_t target;

	time_t attack;
	time_t lastUpdate;
};

struct ControlMapping {
	ControlMapping() : ControlMapping(0, -1) {}
	ControlMapping(ctl_id_t i, tgt_id_t t) : control(i), target(t) {}

	ctl_id_t control;
	tgt_id_t target;
};

// every script mapping should have a separate context, so this should inherit from whatever holds the call stack for scripts
// todo 
struct ScriptMapping: public KLFBaseContext {
	ScriptMapping() : ScriptMapping(0, -1) {}
	ScriptMapping(script_id_t i, tgt_id_t t) : KLFBaseContext(t), script(nullptr), id(i) {}

	KLFun *script;
	script_id_t id;
};

/**
 * mappings for interface components. simple extensions of the prior with a few gui specific details
 */
struct GuiMapping {
	enum ComponentType {
		KNOB,
		BUTTON
	};

	GuiMapping(string &_label, ComponentType c=KNOB) : label(_label), type(c), tag(0) {}

	long tag;
	ComponentType type;
	string label;
};

struct CommandGuiMapping : CommandMapping, GuiMapping {
	CommandGuiMapping() : CommandGuiMapping("", 0, -1) {}
	CommandGuiMapping(string _label, cmd_id_t i, tgt_id_t t) : CommandMapping(i, t), GuiMapping(_label) {}
};

struct ControlGuiMapping : ControlMapping, GuiMapping {
	ControlGuiMapping() : ControlGuiMapping("", 0, -1) {}
	ControlGuiMapping(string _label, ctl_id_t i, tgt_id_t t) : ControlMapping(i, t), GuiMapping(_label) {}
};


struct ScriptGuiMapping : ScriptMapping, GuiMapping {
	ScriptGuiMapping() : ScriptGuiMapping("", 0, -1) {}
	ScriptGuiMapping(string _label, script_id_t i, tgt_id_t t) : ScriptMapping(i, t), GuiMapping(_label) {}
};



/**
 * handles mapping of control inputs (MIDI and eventually maybe other things). 
 * processes the inputs, and executs m-script commands
 */
class Controller: public IControlListener
{
public:
	Controller(Klein &k);
	virtual ~Controller();

	bool processEvent(VstMidiEvent *e);

	void addCommandMapping(CommandMapping & mapping, int mdicmd, int channel, int which);
	void addControlMapping(ControlMapping & mapping, int mdicmd, int channel, int which);
	void addScriptMapping(ScriptMapping & mapping, int mdicmd, int channel, int which);
	status_t addScript(script_id_t id, const char *src);
	status_t lockAndLoadScripts(vector<string> &errors);

	void addCommandGuiMapping(CommandGuiMapping & mapping);
	void addControlGuiMapping(ControlGuiMapping & mapping);
	void addScriptGuiMapping(ScriptGuiMapping & mapping);

	/* from IControlListener */
	virtual void valueChanged(VSTGUI::CControl* pControl) override;
	virtual int32_t controlModifierClicked(CControl* pControl, CButtonState button) override { return 0; }	///< return 1 if you want the control to not handle it, otherwise 0


protected:
	Klein &klein;
	unordered_map<int, vector<CommandMapping>> cmdMap;
	unordered_map<int, vector<ControlMapping>> ctlMap;
	unordered_map<int, vector<ScriptMapping>> klfMap;

	vector<CommandGuiMapping> guiCmds;
	vector<ControlGuiMapping> guiCtls;
	vector<ScriptGuiMapping> guiKlfs;

	vector<CommandMapping> * getCommandMap(int);
	vector<ControlMapping> * getControlMap(int);
	vector<ScriptMapping> * getScriptMap(int);

	vector<KLFun> scripts;

	static int makeMidiHash(int cmd, int chan, int which);

	bool processMapping(const CommandMapping &m);
	bool processMapping(const ControlMapping &m, const midi_val_t v);
	bool processMapping(const ScriptMapping &m, const midi_what_t wh, const midi_val_t v);

	bool processCommand(const cmd_id_t cmd, const tgt_id_t tgt) const;
	bool setControl(const ctl_id_t cmd, const tgt_id_t tgt, const KLFValue& v) const;
	KLFValue Controller::getControl(const ctl_id_t control, const tgt_id_t tgt) const;

	friend class KControl;
	friend class KCmdBlock;
	friend class KVarAssBlock;
	friend class KCtlAssBlock;
	friend class KleinEditor;
	friend class MasterStrip;
	friend class TrackStrip;
};

