#include <iostream>
#include <algorithm>

#include "Controller.h"
#include "Klein.h"

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
	{ Command::STOP_AUDIO_RECORDING,	"StopAudioRecording", "Stop Audio Recording" },
	{ Command::START_POINT, "StartPoint", "Start Point" },
	{ Command::STATUS,		"Status", "Status" },
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
		vector<ControlMapping> *cmv = getMidiMap(makeMidiHash(midiData));
		if (cmv != nullptr) {
			for (auto cmi : *cmv) {

			}
		}
		break;
	}
	case MIDI_CTRL: {
		vector<ControlMapping> *cmv = getMidiMap(makeMidiHash(midiData));
		if (cmv != nullptr) {
			for (auto cmi : *cmv) {
				
			}
		}
		break;
	}
	case MIDI_PROG: {
		vector<ControlMapping> *cmv = getMidiMap(makeMidiHash(midiData));
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
vector<ControlMapping> * Controller::getMidiMap(int hash)
{
	auto ci = midiMap.find(hash);
	if (ci == midiMap.end()) {
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

int makeMidiHash(int cmd, int chan, int which) {
	return (cmd << 12) | (chan << 8) | which
}

void Controller::addNoteMapping(ControlMapping & mapping, int channel, int which)
{
	midiMap[makeMidiHash(MIDI_NOTE_ON, channel, which)].push_back(mapping);
}

void Controller::addCtrlMapping(ControlMapping & mapping, int channel, int which)
{
	midiMap[makeMidiHash(MIDI_CTRL, channel, which)].push_back(mapping);
}

void Controller::addProgMapping(ControlMapping & mapping, int channel, int which)
{
	midiMap[makeMidiHash(MIDI_PROG, channel, which)].push_back(mapping);
}

status_t Controller::loadScript(const char * id, const char * src)
{
	return ERR_OK;
}
