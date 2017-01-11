#include <iostream>
#include <algorithm>
#include <boost/filesystem.hpp>

#include "Controller.h"
#include "Klein.h"

/**
 * central controller, handles midi processing, perhaps eventually keyboard and OSC processing
 * maps these various inputs onto global and track specific functions
 */

/**
 * sorted list of commands. we can search in here via binary search
 */
vector<Command> commands {
	{ Command::AUTO_RECORD,	"AutoRecord", "Auto Record" },
	{ Command::BACKWARD,	"Backward", "Backward" },
	{ Command::BOUNCE,		"Bounce", "Bounce" },
	{ Command::CHECKPOINT,	"Checkpoint", "Checkpoint" },
	{ Command::CLEAR,		"Clear", "Clear" },
	{ Command::CONFIRM,		"Confirm", "Confirm" },
	{ Command::FOCUS_LOCK,	"FocusLock", "Focus Lock" },
	{ Command::FORWARD,		"Forward", "Forward" },
	{ Command::GLOBAL_MUTE,	"GlobalMute", "Global Mute" },
	{ Command::GLOBAL_PAUSE, "GlobalPause", "Global Pause" },
	{ Command::GLOBAL_RESET, "GlobalReset", "Global Reset" },
	{ Command::HALF_SPEED,	"HalfSpeed", "Half Speed" },
	{ Command::INSERT,		"Insert", "Insert" },
	{ Command::INSERT_DIVIDE, "InsertDivide", "Insert Divide" },
	{ Command::INSERT_DIVIDE_3, "InsertDivide3", "Insert Divide 3" },
	{ Command::INSERT_DIVIDE_4, "InsertDivide4", "Insert Divide 4" },
	{ Command::INSERT_MULTIPLY, "InsertMultiply", "Insert Multiply" },
	{ Command::INSERT_MULTIPLY_3, "InsertMultiply3", "Insert Multiply 3" },
	{ Command::INSERT_MULTIPLY_4, "InsertMultiply4", "Insert Multiply 4" },
	{ Command::LOOP1,	"Loop1", "Loop 1" },
	{ Command::LOOP2,	"Loop2", "Loop 2" },
	{ Command::LOOP3,	"Loop3", "Loop 3" },
	{ Command::LOOP4,	"Loop4", "Loop 4" },
	{ Command::LOOP5,	"Loop5", "Loop 5" },
	{ Command::LOOP6,	"Loop6", "Loop 6" },
	{ Command::LOOP7,	"Loop7", "Loop 7" },
	{ Command::LOOP8,	"Loop8", "Loop 8" },
	{ Command::MIDI_START,	"MidiStart", "Midi Start" },
	{ Command::MIDI_STOP,	"MidiStop", "Midi Stop" },
	{ Command::MULTIPLY,	"Multiply", "Multiply" },
	{ Command::MUTE,		"Mute", "Mute" },
	{ Command::MUTE_MIDI_START, "MuteMidiStart", "Mute Midi Start" },
	{ Command::MUTE_REALIGN, "MuteRealign", "Mute Realign" },
	{ Command::NEXT_LOOP,	"NextLoop", "Next Loop" },
	{ Command::NEXT_TRACK,	"NextTrack", "Next Track" },
	{ Command::OVERDUB,		"Overdub", "Overdub" },
	{ Command::PAUSE,		"Pause", "Pause" },
	{ Command::PITCH_CANCEL, "PitchCancel", "Pitch Cancel" },
	{ Command::PITCH_DOWN,	"PitchDown", "Pitch Down" },
	{ Command::PITCH_NEXT,	"Pitch Next", "Pitch Next" },
	{ Command::PITCH_PREVIOUS, "PitchPrevious", "Pitch Previous" },
	{ Command::PITCH_STEP,	"PitchStep", "Pitch Step" },
	{ Command::PITCH_UP,	"PitchUp", "Pitch Up" },
	{ Command::PLAY,		"Play", "Play" },
	{ Command::PREVIOUS_LOOP, "PreviousLoop", "Previous Loop" },
	{ Command::PREVIOUS_TRACK, "PreviousTrack", "Previous Track" },
	{ Command::REALIGN,		"Realign", "Realign" },
	{ Command::RECORD,		"Record", "Record" },
	{ Command::REDO,		"Redo", "Redo" },
	{ Command::REHEARSE,	"Rehearse", "Rehearse" },
	{ Command::RELOAD_SCRIPTS, "ReloadScripts", "Reload Scripts" },
	{ Command::REPLACE,		"Replace", "Replace" },
	{ Command::RESET,		"Reset", "Reset" },
	{ Command::RESTART,		"Restart", "Restart" },
	{ Command::RESTART_ONCE, "RestartOnce", "Restart Once" },
	{ Command::REVERSE,		"Reverse", "Reverse" },
	{ Command::SAMPLE_2,	"Sample2", "Sample 2" },
	{ Command::SAMPLE_3,	"Sample3", "Sample 3" },
	{ Command::SAMPLE_4,	"Sample4", "Sample 4" },
	{ Command::SAMPLE_5,	"Sample5", "Sample 5" },
	{ Command::SAMPLE_6,	"Sample6", "Sample 6" },
	{ Command::SAMPLE_7,	"Sample7", "Sample 7" },
	{ Command::SAMPLE_8,	"Sample8", "Sample 8" },
	{ Command::SAMPLE_9,	"Sample9", "Sample 9" },
	{ Command::SAVE_AUDIO_RECORDING, "SaveAudioRecording", "Save Audio Recording" },
	{ Command::SAVE_LOOP,	"SaveLoop", "Save Loop" },
	{ Command::SHUFFLE,		"Shuffle", "Shuffle" },
	{ Command::SLIP_BACKWARD, "SlipBackward", "Slip Backward" },
	{ Command::SLIP_FORWARD, "SlipForward", "Slip Forward" },
	{ Command::SOLO,		 "Solo", "Solo" },
	{ Command::SPEED_CANCEL, "SpeedCancel", "Speed Cancel" },
	{ Command::SPEED_DOWN,	"SpeedDown", "Speed Down" },
	{ Command::SPEED_NEXT,	"SpeedNext", "Speed Next" },
	{ Command::SPEED_PREV,	"SpeedPrev", "Speed Prev" },
	{ Command::SPEED_STEP,	"SpeedStep", "Speed Step" },
	{ Command::SPEED_TOGGLE, "SpeedToggle", "Speed Toggle" },
	{ Command::SPEED_UP,	"SpeedUp", "Speed Up" },
	{ Command::START_AUDIO_RECORDING,	"StartAudioRecording", "Start Audio Recording" },
	{ Command::START_POINT, "StartPoint", "Start Point" },
	{ Command::STATUS,		"Status", "Status" },
	{ Command::STOP_AUDIO_RECORDING,	"StopAudioRecording", "Stop Audio Recording" },
	{ Command::STUTTER,		"Stutter", "Stutter" },
	{ Command::SUBSTITUTE,	"Substitute", "Substitute" },
	{ Command::SUSTAIN_,	"Sustain", "Sustain" },
	{ Command::SYNC_MASTER_TRACK,	"SyncMasterTrack", "Sync Master Track" },
	{ Command::SYNC_START_POINT,	"SyncStartPoint", "Sync Start Point" },
	{ Command::TRACK_1,		"Track1", "Track 1" },
	{ Command::TRACK_2,		"Track2", "Track 2" },
	{ Command::TRACK_3,		"Track3", "Track 3" },
	{ Command::TRACK_4,		"Track4", "Track 4" },
	{ Command::TRACK_5,		"Track5", "Track 5" },
	{ Command::TRACK_6,		"Track6", "Track 6" },
	{ Command::TRACK_7,		"Track7", "Track 7" },
	{ Command::TRACK_8,		"Track8", "Track 8" },
	{ Command::TRACK_COPY,	"TrackCopy", "Track Copy" },
	{ Command::TRACK_COPY_TIMING,	"TrackCopyTiming", "Track Copy Timing" },
	{ Command::TRACK_GROUP, "TrackGroup", "Track Group" },
	{ Command::TRACK_RESET, "TrackReset", "Track Reset" },
	{ Command::TRIM_END,	"TrimEnd", "Trim End" },
	{ Command::TRIM_START,	"TrimStart", "Trim Start" },
	{ Command::UI_,				"Ui ", "Ui " },
	{ Command::UNDO,			"Undo", "Undo" },
	{ Command::WINDOW_BACKWARD, "WindowBackward", "Window Backward" },
	{ Command::WINDOW_FORWARD,	"WindowForward", "Window Forward" },
	{ Command::WINDOW_START_BACKWARD,	"WindowStartBackward", "Window Start Backward" },
	{ Command::WINDOW_START_FORWARD,	"WindowStartForward", "Window Start Forward" },
	{ Command::WINDOW_END_BACKWARD,		"WindowEndBackward", "Window End Backward" },
	{ Command::WINDOW_END_FORWARD,		"WindowEndForward", "Window End Forward" },
};

vector<Control> controls {
	{ Control::FEEDBACK,		"Feedback", "Feedback" },
	{ Control::INPUT_LEVEL,		"InputLevel", "Input Level"  },
	{ Control::OUTPUT_LEVEL,	"OutputLevel", "Output Level" },
	{ Control::PAN,				"Pan", "Pan" },
	{ Control::PITCH_BEND,		"PitchBend", "Pitch Bend" },
	{ Control::PITCH_OCT,		"PitchOct", "Pitch Oct" },
	{ Control::PITCH_STEP,		"PitchStep", "Pitch Step" },
	{ Control::SECONDARY_FEEDBACK,	"SecondaryFeedback", "Secondary Feedback" },
	{ Control::SPEED_BEND,		"SpeedBend", "Speed Bend" },
	{ Control::SPEED_OCT,		"SpeedOct", "Speed Oct" },
	{ Control::SPEED_STEP,		"SpeedStep", "Speed Step" },
	{ Control::TIME_STRETCH,	"TimeStretch", "Time Stretch" }
};

Command doNothing(Command::NOTHING, "", "");

Control *
Control::find(string nm) {
	auto f = std::lower_bound(controls.begin(), controls.end(), nm,
		[](Control jt, string jnm) { return jt.scriptName < jnm; });
	if (f != controls.end() && f->scriptName == nm) {
		return std::addressof(*f);
	}
	return nullptr;
}

Command *
Command::find(string nm) {
	auto f = std::lower_bound(commands.begin(), commands.end(), nm,
		[](Command jt, string jnm) { return jt.scriptName < jnm; });
	if (f != commands.end() && f->scriptName == nm) {
		return std::addressof(*f);
	}
	return nullptr;
}


Controller::Controller(Klein &k)
	: klein(k)
{
	cmdMap.max_load_factor(0.75);
	ctlMap.max_load_factor(0.75);
	klfMap.max_load_factor(0.75);
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
	vector<CommandMapping> *cmv;
	vector<ControlMapping> *dmv;
	vector<ScriptMapping> *smv;

	switch (cmd) {
	case MIDI_NOTE_OFF:
	case MIDI_NOTE_ON: { // has duration, secondary value
		int note = midiNote(midiData);
		int vel = midiNoteVel(midiData);
		int rcmd = (cmd == MIDI_NOTE_OFF ? MIDI_NOTE_ON : cmd);
		int hash = makeMidiHash(rcmd, chan, note);
		if ((cmv = getCommandMap(hash)) != nullptr) {
			for (auto cmi : *cmv) {
				processMapping(cmi);
			}
		}
		if ((dmv = getControlMap(hash)) != nullptr) {
			for (auto dmi : *dmv) {
				processMapping(dmi, vel);
			}
		}
		if ((smv = getScriptMap(hash)) != nullptr) {
			for (auto smi : *smv) {
				processMapping(smi, note, vel);
			}
		}
		break;
	}
	case MIDI_CTRL: { // instantaneous, secondary value
		int ctl = midiCtrl(midiData);
		int val = midiCtrlVal(midiData);
		int hash = makeMidiHash(cmd, chan, ctl);
		if ((cmv = getCommandMap(hash)) != nullptr) {
			for (auto cmi : *cmv) {
				processMapping(cmi);
			}
		}
		if ((dmv = getControlMap(hash)) != nullptr) {
			for (auto dmi : *dmv) {
				processMapping(dmi, val);
			}
		}
		if ((smv = getScriptMap(hash)) != nullptr) {
			for (auto smi : *smv) {
				processMapping(smi, ctl, val);
			}
		}
		break;
	}
	case MIDI_PROG: { // instantaneous, single value
		int prog = midiProg(midiData);
		int hash = makeMidiHash(cmd, chan, prog);
		if ((cmv = getCommandMap(hash)) != nullptr) {
			for (auto cmi : *cmv) {
				processMapping(cmi);
			}
		}
		if ((smv = getScriptMap(hash)) != nullptr) {
			for (auto smi : *smv) {
				processMapping(smi, prog, 0);
			}
		}
		break;
	}
	case MIDI_BEND:
		break;
	}
	return true;
}

bool Controller::processMapping(const CommandMapping & m) {
	return processCommand(m.command, m.target);
}

bool Controller::processCommand(const cmd_id_t cmd, const tgt_id_t tgt) const
{
	KleinTrack &t = klein.getTrack(tgt);
	switch (cmd) {
		case Command::AUTO_RECORD: return true;
		case Command::BACKWARD: return true;
		case Command::BOUNCE: return true;
		case Command::CHECKPOINT: return true;
		case Command::CLEAR: return true;
		case Command::CONFIRM: return true;
		case Command::FOCUS_LOCK: return true;
		case Command::FORWARD: return true;
		case Command::GLOBAL_MUTE: return klein.globalMute();
		case Command::GLOBAL_PAUSE: return klein.globalPause();
		case Command::GLOBAL_RESET: return klein.globalReset();
		case Command::HALF_SPEED: return true;
		case Command::INSERT: return true;
		case Command::INSERT_DIVIDE: return true;
		case Command::INSERT_DIVIDE_3: return true;
		case Command::INSERT_DIVIDE_4: return true;
		case Command::INSERT_MULTIPLY: return true;
		case Command::INSERT_MULTIPLY_3: return true;
		case Command::INSERT_MULTIPLY_4: return true;
		case Command::LOOP1: return t.doLoop(1);
		case Command::LOOP2: return t.doLoop(2);
		case Command::LOOP3: return t.doLoop(3);
		case Command::LOOP4: return t.doLoop(4);
		case Command::LOOP5: return t.doLoop(5);
		case Command::LOOP6: return t.doLoop(6);
		case Command::LOOP7: return t.doLoop(7);
		case Command::LOOP8: return t.doLoop(8);
		case Command::MIDI_START: return false;
		case Command::MIDI_STOP: return false;
		case Command::MULTIPLY: return true;
		case Command::MUTE: return t.doMute();
		case Command::MUTE_MIDI_START: return true;
		case Command::MUTE_REALIGN: return true;
		case Command::NEXT_LOOP: return true;
		case Command::NEXT_TRACK: return true;
		case Command::OVERDUB: return t.overdub();
		case Command::PAUSE: return t.pause();
		case Command::PITCH_CANCEL: return true;
		case Command::PITCH_DOWN: return true;
		case Command::PITCH_NEXT: return true;
		case Command::PITCH_PREVIOUS: return true;
		case Command::PITCH_STEP: return true;
		case Command::PITCH_UP: return true;
		case Command::PLAY: return t.play();
		case Command::PREVIOUS_LOOP: return true;
		case Command::PREVIOUS_TRACK: return true;
		case Command::REALIGN: return true;
		case Command::RECORD: return t.record();
		case Command::REDO: return true;
		case Command::REHEARSE: return true;
		case Command::RELOAD_SCRIPTS: return true;
		case Command::REPLACE: return true;
		case Command::RESET: return true;
		case Command::RESTART: return true;
		case Command::RESTART_ONCE: return true;
		case Command::REVERSE: return true;
		case Command::SAMPLE_2: return true;
		case Command::SAMPLE_3: return true;
		case Command::SAMPLE_4: return true;
		case Command::SAMPLE_5: return true;
		case Command::SAMPLE_6: return true;
		case Command::SAMPLE_7: return true;
		case Command::SAMPLE_8: return true;
		case Command::SAMPLE_9: return true;
		case Command::SAVE_AUDIO_RECORDING: return true;
		case Command::SAVE_LOOP: return true;
		case Command::SHUFFLE: return true;
		case Command::SLIP_BACKWARD: return true;
		case Command::SLIP_FORWARD: return true;
		case Command::SOLO: return true;
		case Command::SPEED_CANCEL: return true;
		case Command::SPEED_DOWN: return true;
		case Command::SPEED_NEXT: return true;
		case Command::SPEED_PREV: return true;
		case Command::SPEED_STEP: return true;
		case Command::SPEED_TOGGLE: return true;
		case Command::SPEED_UP: return true;
		case Command::START_AUDIO_RECORDING: return true;
		case Command::STOP_AUDIO_RECORDING: return true;
		case Command::START_POINT: return true;
		case Command::STATUS: return true;
		case Command::STUTTER: return true;
		case Command::SUBSTITUTE: return true;
		case Command::SUSTAIN_: return true;
		case Command::SYNC_MASTER_TRACK: return true;
		case Command::SYNC_START_POINT: return true;
		case Command::TRACK_1: return klein.selectTrack(1);
		case Command::TRACK_2: return klein.selectTrack(2);
		case Command::TRACK_3: return klein.selectTrack(3);
		case Command::TRACK_4: return klein.selectTrack(4);
		case Command::TRACK_5: return klein.selectTrack(5);
		case Command::TRACK_6: return klein.selectTrack(6);
		case Command::TRACK_7: return klein.selectTrack(7);
		case Command::TRACK_8: return klein.selectTrack(8);
		case Command::TRACK_COPY: return true;
		case Command::TRACK_COPY_TIMING: return true;
		case Command::TRACK_GROUP: return true;
		case Command::TRACK_RESET: return true;
		case Command::TRIM_END: return true;
		case Command::TRIM_START: return true;
		case Command::UI_: return true;
		case Command::UNDO: return true;
		case Command::WINDOW_BACKWARD: return true;
		case Command::WINDOW_FORWARD: return true;
		case Command::WINDOW_START_BACKWARD: return true;
		case Command::WINDOW_START_FORWARD: return true;
		case Command::WINDOW_END_BACKWARD: return true;
		case Command::WINDOW_END_FORWARD: return true;

	}
	return false;
}

bool Controller::processMapping(const ControlMapping & m, int v)
{
	return setControl(m.control, m.target, v);
}

bool Controller::setControl(const ctl_id_t control, const tgt_id_t tgt, const int v) const
{
	KleinTrack &t = klein.getTrack(tgt);
	switch (control) {
		case Control::FEEDBACK:		t.setFeedback(v);  return true;
		case Control::INPUT_LEVEL:	t.setInputGain(v);  return true;
		case Control::OUTPUT_LEVEL:	t.setOutputGain(v);   return true;
		case Control::PAN:			t.setPan(v);  return true;
		case Control::PITCH_BEND:	return false;
		case Control::PITCH_OCT:	return false;
		case Control::PITCH_STEP:	return false;
		case Control::SECONDARY_FEEDBACK: t.setSecondaryFeedback(v);  return true;
		case Control::SPEED_BEND:	return false;
		case Control::SPEED_OCT:	return false;
		case Control::SPEED_STEP:	return false;
		case Control::TIME_STRETCH:	return false;
	}
	return false;
}

KLFValue Controller::getControl(const ctl_id_t control, const tgt_id_t tgt) const
{
	KleinTrack &t = klein.getTrack(tgt);
	switch (control) {
	case Control::FEEDBACK:		return KLFValue(t.getFeedback());
	case Control::INPUT_LEVEL:	return KLFValue(t.getInputGain());
	case Control::OUTPUT_LEVEL:	return KLFValue(t.getOutputGain());
	case Control::PAN:			return KLFValue(t.getPan());
	case Control::PITCH_BEND:	return KLFValue();
	case Control::PITCH_OCT:	return KLFValue();
	case Control::PITCH_STEP:	return KLFValue();
	case Control::SECONDARY_FEEDBACK: return KLFValue(t.getSecondaryFeedback()); 
	case Control::SPEED_BEND:	return KLFValue();
	case Control::SPEED_OCT:	return KLFValue();
	case Control::SPEED_STEP:	return KLFValue();
	case Control::TIME_STRETCH:	return KLFValue();
	}
	return KLFValue();
}

bool Controller::processMapping(const ScriptMapping & m, int wh, int v)
{
	auto s = m.script;
	if (!s) return false;
	s->doStart(*this, m);
	return false;
}

/**
 *
 */
vector<CommandMapping> * Controller::getCommandMap(int hash)
{
	auto ci = cmdMap.find(hash);
	if (ci == cmdMap.end()) {
		return nullptr;
	}

	return &ci->second;
}

vector<ControlMapping> * Controller::getControlMap(int hash)
{
	auto ci = ctlMap.find(hash);
	if (ci == ctlMap.end()) {
		return nullptr;
	}

	return &ci->second;
}

vector<ScriptMapping> * Controller::getScriptMap(int hash)
{
	auto ci = klfMap.find(hash);
	if (ci == klfMap.end()) {
		return nullptr;
	}

	return &ci->second;
}

/**
 * not really a hash function into our unordered map. just maps command, channel and note/ctrl/progno onto a 12 bit int
 */
int Controller::makeMidiHash(int cmd, int chan, int which) {
	return (cmd << 12) | (chan << 8) | which;
}

/**
 * @param mdicmd MIDI_NOTE_ON, MIDI_CTRL, or MIDI_PROG
 */
void Controller::addCommandMapping(CommandMapping & mapping, int mdicmd, int channel, int which)
{
	cmdMap[makeMidiHash(mdicmd, channel, which)].push_back(mapping);
}


/**
* @param mdicmd MIDI_NOTE_ON, MIDI_CTRL, or MIDI_PROG
*/
void Controller::addControlMapping(ControlMapping & mapping, int mdicmd, int channel, int which)
{
	ctlMap[makeMidiHash(mdicmd, channel, which)].push_back(mapping);
}

/**
* @param mdicmd MIDI_NOTE_ON, MIDI_CTRL, or MIDI_PROG
*/
void Controller::addScriptMapping(ScriptMapping & mapping, int mdicmd, int channel, int which)
{
	klfMap[makeMidiHash(mdicmd, channel, which)].push_back(mapping);
}


status_t Controller::addScript(script_id_t id, const char * src)
{
	boost::filesystem::path s(src);
	if (!exists(s)) {
		return ERR_ERROR;
	}
	scripts.push_back(KLFun(id, src));
	return ERR_OK;
}

status_t Controller::lockAndLoadScripts(vector<string>& errors)
{
	for (KLFun &it : scripts) {
		if (!it.loaded) {
			vector<string> errorMessages;
			status_t err = it.load(errorMessages);
			if (err == ERR_OK) {
				it.loaded = true;
			}
		}
	}
	return ERR_OK;
}
