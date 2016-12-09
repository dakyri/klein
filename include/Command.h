#pragma once

#include <string>
using namespace std;
/**
 * constant for and structure for creating searchable list of Commands, actions which a controller
 * can take to affect the looper
 */
struct Command {
	enum {
		AUTO_RECORD,
		BACKWARD,
		BOUNCE,
		CHECKPOINT,
		CLEAR,
		CONFIRM,
		FOCUS_LOCK,
		FORWARD,
		GLOBAL_MUTE,
		GLOBAL_PAUSE,
		GLOBAL_RESET,
		HALF_SPEED,
		INSERT,
		INSERT_DIVIDE,
		INSERT_DIVIDE_3,
		INSERT_DIVIDE_4,
		INSERT_MULTIPLY,
		INSERT_MULTIPLY_3,
		INSERT_MULTIPLY_4,
		LOOP1,
		LOOP2,
		LOOP3,
		LOOP4,
		LOOP5,
		LOOP6,
		LOOP7,
		LOOP8,
		MIDI_START,
		MIDI_STOP,
		MULTIPLY,
		MUTE,
		MUTE_MIDI_START,
		MUTE_REALIGN,
		NEXT_LOOP,
		NEXT_TRACK,
		OVERDUB,
		PAUSE,
		PITCH_CANCEL,
		PITCH_DOWN,
		PITCH_NEXT,
		PITCH_PREVIOUS,
		PITCH_STEP,
		PITCH_UP,
		PLAY,
		PREVIOUS_LOOP,
		PREVIOUS_TRACK,
		REALIGN,
		REDO,
		REHEARSE,
		RELOAD_SCRIPTS,
		REPLACE,
		RESET,
		RESTART,
		RESTART_ONCE,
		REVERSE,
		SAMPLE_2,
		SAMPLE_3,
		SAMPLE_4,
		SAMPLE_5,
		SAMPLE_6,
		SAMPLE_7,
		SAMPLE_8,
		SAMPLE_9,
		SAVE_AUDIO_RECORDING,
		SAVE_LOOP,
		SHUFFLE,
		SLIP_BACKWARD,
		SLIP_FORWARD,
		SOLO,
		SPEED_CANCEL,
		SPEED_DOWN,
		SPEED_NEXT,
		SPEED_PREV,
		SPEED_STEP,
		SPEED_TOGGLE,
		SPEED_UP,
		START_AUDIO_RECORDING,
		STOP_AUDIO_RECORDING,
		START_POINT,
		STATUS,
		STUTTER,
		SUBSTITUTE,
		SUSTAIN_,
		SYNC_MASTER_TRACK,
		SYNC_START_POINT,
		TRACK_1,
		TRACK_2,
		TRACK_3,
		TRACK_4,
		TRACK_5,
		TRACK_6,
		TRACK_7,
		TRACK_8,
		TRACK_COPY,
		TRACK_COPY_TIMING,
		TRACK_GROUP,
		TRACK_RESET,
		TRIM_END,
		TRIM_START,
		UI_,
		UNDO,
		WINDOW_BACKWARD,
		WINDOW_FORWARD,
		WINDOW_START_BACKWARD,
		WINDOW_START_FORWARD,
		WINDOW_END_BACKWARD,
		WINDOW_END_FORWARD
	};
	Command(int _command, string _scriptName, string _displayName):
		command(_command), scriptName(_scriptName), displayName(_displayName) {}

	int command;
	string scriptName;
	string displayName;

	static Command *find(string nm);
};

/*
 * constant for and structure for creating searchable list of controls, whose values the controller
 * may reasonably assign things too
 */
struct Control {
	enum {
		FEEDBACK,
		INPUT_LEVEL,
		OUTPUT_LEVEL,
		PAN,
		PITCH_BEND,
		PITCH_OCT,
		PITCH_STEP,
		SECONDARY_FEEDBACK,
		SPEED_BEND,
		SPEED_OCT,
		SPEED_STEP,
		TIME_STRETCH
	};
	Control(int _command, string _scriptName, string _displayName) :
		control(_command), scriptName(_scriptName), displayName(_displayName) {}

	int control;
	string scriptName;
	string displayName;

	static Control *find(string nm);
};