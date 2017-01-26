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
	{ Command::AUTO_RECORD,	true, "AutoRecord", "Auto Record" },
	{ Command::BACKWARD,	true, "Backward", "Backward" },
	{ Command::BOUNCE,		true, "Bounce", "Bounce" },
	{ Command::CHECKPOINT,	true, "Checkpoint", "Checkpoint" },
	{ Command::CLEAR,		true, "Clear", "Clear" },
	{ Command::CONFIRM,		true, "Confirm", "Confirm" },
	{ Command::FOCUS_LOCK,	true, "FocusLock", "Focus Lock" },
	{ Command::FORWARD,		true, "Forward", "Forward" },
	{ Command::GLOBAL_MUTE,	true, "GlobalMute", "Global Mute" },
	{ Command::GLOBAL_PAUSE, true, "GlobalPause", "Global Pause" },
	{ Command::GLOBAL_RESET, true, "GlobalReset", "Global Reset" },
	{ Command::HALF_SPEED,	true, "HalfSpeed", "Half Speed" },
	{ Command::INSERT,		true, "Insert", "Insert" },
	{ Command::INSERT_DIVIDE, true, "InsertDivide", "Insert Divide" },
	{ Command::INSERT_DIVIDE_3, true, "InsertDivide3", "Insert Divide 3" },
	{ Command::INSERT_DIVIDE_4, true, "InsertDivide4", "Insert Divide 4" },
	{ Command::INSERT_MULTIPLY, true, "InsertMultiply", "Insert Multiply" },
	{ Command::INSERT_MULTIPLY_3, true, "InsertMultiply3", "Insert Multiply 3" },
	{ Command::INSERT_MULTIPLY_4, true, "InsertMultiply4", "Insert Multiply 4" },
	{ Command::LOOP1,	true, "Loop1", "Loop 1" },
	{ Command::LOOP2,	true, "Loop2", "Loop 2" },
	{ Command::LOOP3,	true, "Loop3", "Loop 3" },
	{ Command::LOOP4,	true, "Loop4", "Loop 4" },
	{ Command::LOOP5,	true, "Loop5", "Loop 5" },
	{ Command::LOOP6,	true, "Loop6", "Loop 6" },
	{ Command::LOOP7,	true, "Loop7", "Loop 7" },
	{ Command::LOOP8,	true, "Loop8", "Loop 8" },
	{ Command::MIDI_START,	true, "MidiStart", "Midi Start" },
	{ Command::MIDI_STOP,	true, "MidiStop", "Midi Stop" },
	{ Command::MULTIPLY,	true, "Multiply", "Multiply" },
	{ Command::MUTE,		true, "Mute", "Mute" },
	{ Command::MUTE_MIDI_START, true, "MuteMidiStart", "Mute Midi Start" },
	{ Command::MUTE_REALIGN, true, "MuteRealign", "Mute Realign" },
	{ Command::NEXT_LOOP,	true, "NextLoop", "Next Loop" },
	{ Command::NEXT_TRACK,	true, "NextTrack", "Next Track" },
	{ Command::OVERDUB,		true, "Overdub", "Overdub" },
	{ Command::PAUSE,		true, "Pause", "Pause" },
	{ Command::PITCH_CANCEL, true, "PitchCancel", "Pitch Cancel" },
	{ Command::PITCH_DOWN,	true, "PitchDown", "Pitch Down" },
	{ Command::PITCH_NEXT,	true, "Pitch Next", "Pitch Next" },
	{ Command::PITCH_PREVIOUS, true, "PitchPrevious", "Pitch Previous" },
	{ Command::PITCH_STEP,	true, "PitchStep", "Pitch Step" },
	{ Command::PITCH_UP,	true, "PitchUp", "Pitch Up" },
	{ Command::PLAY,		true, "Play", "Play" },
	{ Command::PREVIOUS_LOOP, true, "PreviousLoop", "Previous Loop" },
	{ Command::PREVIOUS_TRACK, true, "PreviousTrack", "Previous Track" },
	{ Command::REALIGN,		true, "Realign", "Realign" },
	{ Command::RECORD,		true, "Record", "Record" },
	{ Command::REDO,		true, "Redo", "Redo" },
	{ Command::REHEARSE,	true, "Rehearse", "Rehearse" },
	{ Command::RELOAD_SCRIPTS, true, "ReloadScripts", "Reload Scripts" },
	{ Command::REPLACE,		true, "Replace", "Replace" },
	{ Command::RESET,		true, "Reset", "Reset" },
	{ Command::RESTART,		true, "Restart", "Restart" },
	{ Command::RESTART_ONCE, true, "RestartOnce", "Restart Once" },
	{ Command::REVERSE,		true, "Reverse", "Reverse" },
	{ Command::SAMPLE_2,	true, "Sample2", "Sample 2" },
	{ Command::SAMPLE_3,	true, "Sample3", "Sample 3" },
	{ Command::SAMPLE_4,	true, "Sample4", "Sample 4" },
	{ Command::SAMPLE_5,	true, "Sample5", "Sample 5" },
	{ Command::SAMPLE_6,	true, "Sample6", "Sample 6" },
	{ Command::SAMPLE_7,	true, "Sample7", "Sample 7" },
	{ Command::SAMPLE_8,	true, "Sample8", "Sample 8" },
	{ Command::SAMPLE_9,	true, "Sample9", "Sample 9" },
	{ Command::SAVE_AUDIO_RECORDING, true, "SaveAudioRecording", "Save Audio Recording" },
	{ Command::SAVE_LOOP,	true, "SaveLoop", "Save Loop" },
	{ Command::SHUFFLE,		true, "Shuffle", "Shuffle" },
	{ Command::SLIP_BACKWARD, true, "SlipBackward", "Slip Backward" },
	{ Command::SLIP_FORWARD, true, "SlipForward", "Slip Forward" },
	{ Command::SOLO,		 true, "Solo", "Solo" },
	{ Command::SPEED_CANCEL, true, "SpeedCancel", "Speed Cancel" },
	{ Command::SPEED_DOWN,	true, "SpeedDown", "Speed Down" },
	{ Command::SPEED_NEXT,	true, "SpeedNext", "Speed Next" },
	{ Command::SPEED_PREV,	true, "SpeedPrev", "Speed Prev" },
	{ Command::SPEED_STEP,	true, "SpeedStep", "Speed Step" },
	{ Command::SPEED_TOGGLE, true, "SpeedToggle", "Speed Toggle" },
	{ Command::SPEED_UP,	true, "SpeedUp", "Speed Up" },
	{ Command::START_AUDIO_RECORDING,	true, "StartAudioRecording", "Start Audio Recording" },
	{ Command::START_POINT, true, "StartPoint", "Start Point" },
	{ Command::STATUS,		true, "Status", "Status" },
	{ Command::STOP_AUDIO_RECORDING,	true, "StopAudioRecording", "Stop Audio Recording" },
	{ Command::STUTTER,		true, "Stutter", "Stutter" },
	{ Command::SUBSTITUTE,	true, "Substitute", "Substitute" },
	{ Command::SUSTAIN_,	true, "Sustain", "Sustain" },
	{ Command::SYNC_MASTER_TRACK,	true, "SyncMasterTrack", "Sync Master Track" },
	{ Command::SYNC_START_POINT,	true, "SyncStartPoint", "Sync Start Point" },
	{ Command::TRACK_1,		true, "Track1", "Track 1" },
	{ Command::TRACK_2,		true, "Track2", "Track 2" },
	{ Command::TRACK_3,		true, "Track3", "Track 3" },
	{ Command::TRACK_4,		true, "Track4", "Track 4" },
	{ Command::TRACK_5,		true, "Track5", "Track 5" },
	{ Command::TRACK_6,		true, "Track6", "Track 6" },
	{ Command::TRACK_7,		true, "Track7", "Track 7" },
	{ Command::TRACK_8,		true, "Track8", "Track 8" },
	{ Command::TRACK_COPY,	true, "TrackCopy", "Track Copy" },
	{ Command::TRACK_COPY_TIMING,	true, "TrackCopyTiming", "Track Copy Timing" },
	{ Command::TRACK_GROUP, true, "TrackGroup", "Track Group" },
	{ Command::TRACK_RESET, true, "TrackReset", "Track Reset" },
	{ Command::TRIM_END,	true, "TrimEnd", "Trim End" },
	{ Command::TRIM_START,	true, "TrimStart", "Trim Start" },
	{ Command::UI_,				true, "Ui ", "Ui " },
	{ Command::UNDO,			true, "Undo", "Undo" },
	{ Command::WINDOW_BACKWARD, true, "WindowBackward", "Window Backward" },
	{ Command::WINDOW_FORWARD,	true, "WindowForward", "Window Forward" },
	{ Command::WINDOW_START_BACKWARD,	true, "WindowStartBackward", "Window Start Backward" },
	{ Command::WINDOW_START_FORWARD,	true, "WindowStartForward", "Window Start Forward" },
	{ Command::WINDOW_END_BACKWARD,		true, "WindowEndBackward", "Window End Backward" },
	{ Command::WINDOW_END_FORWARD,		true, "WindowEndForward", "Window End Forward" },
};

/**
 * searched via b-search ... this list should be alphabetical
 */
vector<Control> controls {
	{ Control::CLICK_COUNT,		false, true, "clickCount", "Click Count" },
	{ Control::EMPTY_LOOP_ACT,	false, true, "emptyLoopAction", "Empty Loop Action" }, // This parameter determines the action that will be performed whenever an empty loop is activated
	{ Control::FEEDBACK,		true, false, "feedback", "Feedback" },
	{ Control::OUTPUT_LEVEL,	true, false, "gain", "Output Level" },
	{ Control::INPUT_LEVEL,		true, false, "inputGain", "Input Level"  },
	{ Control::INPUT_PAN,		true, false, "inputPan", "Input Pan" },
	{ Control::INPUT_PORT,		false, true, "inputPort", "Input Port" },
	{ Control::IS_RECORDING,	false, true, "isRecording", "Is Recording" },
	{ Control::MODE,			false, true, "mode", "Mode" },
	{ Control::OUTPUT_PORT,		false, true, "outputPort", "Output Port" },
	{ Control::PAN,				true, false, "pan", "Output Pan" },
	{ Control::PITCH_BEND,		true, false, "pitchBend", "Pitch Bend" },
	{ Control::PITCH_OCT,		true, false, "pitchOct", "Pitch Oct" },
	{ Control::PITCH_STEP,		true, false, "pitchStep", "Pitch Step" },
	{ Control::SECONDARY_FEEDBACK,	true, false, "secondaryFeedback", "Secondary Feedback" },
	{ Control::SPEED_BEND,		true, false, "speedBend", "Speed Bend" },
	{ Control::SPEED_OCT,		true, false, "speedOct", "Speed Oct" },
	{ Control::SPEED_STEP,		true, false, "speedStep", "Speed Step" },
	{ Control::SUSTAIN_COUNT,	false, true, "sustainCount", "Sustain Count" },
	{ Control::SWITCH_QUANTIZE,	false, true, "sustainCount", "Sustain Count" }, //This parameter determines when the loop switch functions will be performed.
	{ Control::TIME_COPY_MODE,	false, true, "timeCopyMode", "Time Copy Mode" }, // When performing a Time Copy because Empty Loop Action was set to Copy Timing, this parameter specifies the mode you will be in after the copy
	{ Control::TIME_STRETCH,	true, false, "timeStretch", "Time Stretch" }
};

Command doNothing(Command::NOTHING, false, "", "");

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

//-----------------------------------------------------------------------------
void
Controller::valueChanged(VSTGUI::CControl* control)
{
	long tag = control->getTag();
	control->setDirty(true);
	/*
	switch (tag)
	{

	case kTap1Delay:
	effect->setParameterAutomated(tag, control->getValue());
	tapDelayFader[0]
	tapDelayDisplay[0]->update(context);
	break;
	case kTap1Level:
	effect->setParameterAutomated(tag, control->getValue());
	tapLevelFader[0]->update(context);
	tapLevelDisplay[0]->update(context);
	break;
	}*/
}

bool
Controller::processEvent(VstMidiEvent * e)
{
	char* midiData = e->midiData;
	int cmd = midiCmd(midiData);	
	int chan = midiChan(midiData);
//	int hash = makeIdxHash(midiData);
	vector<CommandMapping> *cmv;
	vector<ControlMapping> *dmv;
	vector<ScriptMapping> *smv;

	switch (cmd) {
	case MIDI_NOTE_OFF:
	case MIDI_NOTE_ON: { // has duration, secondary value
		int note = midiNote(midiData);
		int vel = midiNoteVel(midiData);
		int rcmd = (cmd == MIDI_NOTE_OFF ? MIDI_NOTE_ON : cmd);
#if KLEIN_DEBUG >= 7
		dbf << "processEvent note" << (cmd == MIDI_NOTE_ON?" on ":" off ") << note<<":"<< vel <<endl;
#endif
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
#if KLEIN_DEBUG >= 7
		dbf << "processEvent ctl " << ctl << ":" << val << endl;
#endif
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
#if KLEIN_DEBUG >= 7
		dbf << "processEvent prog " << prog << endl;
#endif
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
#if KLEIN_DEBUG >= 7
		dbf << "processEvent bend " << endl;
#endif
		break;
	}
	return true;
}

bool Controller::processMapping(const CommandMapping & m) {
	return processCommand(m.command, m.target);
}

bool Controller::processCommand(const cmd_id_t cmd, const tgt_id_t tgt) const
{
#if KLEIN_DEBUG >= 8
	dbf << "processCommand " << cmd << ", " << tgt << endl;
#endif
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

bool Controller::processMapping(const ControlMapping & m, midi_val_t v)
{
#if KLEIN_DEBUG >= 8
	dbf << "processMapping " << m.control << ", " << m.target << endl;
#endif
	return setControl(m.control, m.target, (float)(v/127.0));
}

bool Controller::setControl(const ctl_id_t control, const tgt_id_t tgt, const KLFValue& v) const
{
#if KLEIN_DEBUG >= 9
	dbf << "setControl " << control << ", " << tgt << endl;
#endif
	KleinTrack &t = klein.getTrack(tgt);
	switch (control) {
		case Control::FEEDBACK:		t.setFeedback(v.ControlFloatValue());  return true;
		case Control::INPUT_PAN:	t.setInputPan(v.ControlFloatValue());	return true;
		case Control::INPUT_LEVEL:	t.setInputGain(v.ControlFloatValue());  return true;
		case Control::OUTPUT_LEVEL:	t.setOutputGain(v.ControlFloatValue());   return true;
		case Control::PAN:			t.setPan(v.ControlFloatValue());  return true;
		case Control::PITCH_BEND:	return false;
		case Control::PITCH_OCT:	return false;
		case Control::PITCH_STEP:	return false;
		case Control::SECONDARY_FEEDBACK: t.setSecondaryFeedback(v.ControlFloatValue());  return true;
		case Control::SPEED_BEND:	return false;
		case Control::SPEED_OCT:	return false;
		case Control::SPEED_STEP:	return false;
		case Control::TIME_STRETCH:	return false;
			// unmappable binding related variables
		case Control::CLICK_COUNT:		return false; // readonly
		case Control::SUSTAIN_COUNT:	return false; // readonly
			// unmappable track variable
		case Control::INPUT_PORT:	klein.setInPort(t, v.IntValue()); return true; // read-write
		case Control::OUTPUT_PORT:	klein.setOutPort(t, v.IntValue()); return true; // read-write
		case Control::IS_RECORDING:	return false; // readonly
		case Control::MODE:			return false; // readonly
			// global variables
		case Control::EMPTY_LOOP_ACT:	return false;
		case Control::SWITCH_QUANTIZE:	return false;
		case Control::TIME_COPY_MODE:	return false;
	}
	return false;
}

KLFValue Controller::getControl(const ctl_id_t control, const tgt_id_t tgt) const
{
#if KLEIN_DEBUG >= 9
	dbf << "getControl " << control << ", " << tgt << endl;
#endif
	KleinTrack &t = klein.getTrack(tgt);
	switch (control) {
	case Control::FEEDBACK:		return KLFValue(t.getFeedback());
	case Control::INPUT_LEVEL:	return KLFValue(t.getInputGain());
	case Control::INPUT_PAN:	return KLFValue(t.getInputPan());
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
		// unmappable binding related variables
	case Control::CLICK_COUNT:	return KLFValue();
	case Control::SUSTAIN_COUNT:	return KLFValue();
		// unmappable track variable
	case Control::INPUT_PORT:	return KLFValue(t.getInPort());
	case Control::OUTPUT_PORT:	return KLFValue(t.getOutPort());
	case Control::IS_RECORDING:	return KLFValue(t.isRecording());
	case Control::MODE:	return KLFValue(trackMode4(t.getMode()));
		// global variables
	case Control::EMPTY_LOOP_ACT:	return KLFValue();
	case Control::SWITCH_QUANTIZE:	return KLFValue();
	case Control::TIME_COPY_MODE:	return KLFValue();
	}
	return KLFValue();
}

bool Controller::processMapping(const ScriptMapping & m, midi_what_t wh, midi_val_t v)
{
#if KLEIN_DEBUG >= 7
	dbf << "process script " << wh << ", " << v << endl;
#endif
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

/**
* @param mapping
*/
void Controller::addCommandGuiMapping(CommandGuiMapping & mapping)
{
	guiCmds.push_back(mapping);
}

/**
* @param mapping
*/
void Controller::addControlGuiMapping(ControlGuiMapping & mapping)
{
	guiCtls.push_back(mapping);
}

/**
* @param mapping
*/
void Controller::addScriptGuiMapping(ScriptGuiMapping & mapping)
{
	guiKlfs.push_back(mapping);
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
