#pragma once

#include <string>
using namespace std;

typedef unsigned int cmd_id_t;
typedef unsigned int ctl_id_t;
typedef unsigned int script_id_t;
typedef unsigned int tgt_id_t;


/**
 * constant for and structure for creating searchable list of Commands, actions which a controller
 * can take to affect the looper
 */
struct Command {
	enum {
		NOTHING = 0,
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
		RECORD,
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
		START_POINT,
		STATUS,
		STOP_AUDIO_RECORDING, 
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
	Command(int _command, bool _mappable, string _scriptName, string _displayName):
		command(_command), scriptName(_scriptName), displayName(_displayName), mappable(_mappable) {}

	cmd_id_t command;
	string scriptName;
	string displayName;
	bool mappable;

	static Command *find(string nm);
};

extern Command doNothing;

struct CommandStackItem {
	CommandStackItem()
		: CommandStackItem(doNothing) { }

	CommandStackItem(Command & c)
		: command(c) { }

	Command &command;
};

/*
 * constant for and structure for creating searchable list of controls, whose values the controller
 * may reasonably assign things too. Some are directly mappable, some are script only
 */
struct Control {
	enum {
		// mappable track variables
		FEEDBACK,
		INPUT_LEVEL,
		INPUT_PAN,
		OUTPUT_LEVEL,
		PAN,
		PITCH_BEND,
		PITCH_OCT,
		PITCH_STEP,
		SECONDARY_FEEDBACK,
		SPEED_BEND,
		SPEED_OCT,
		SPEED_STEP,
		TIME_STRETCH,

		// unmappable binding related variables
		CLICK_COUNT,
		SUSTAIN_COUNT, 

		// unmappable track variable
		IS_RECORDING,
		MODE,
		INPUT_PORT,
		OUTPUT_PORT,

		// global variables
		EMPTY_LOOP_ACT,
		SWITCH_QUANTIZE,
		TIME_COPY_MODE,
	};
	Control(int _command, bool _mappable, bool _readOnly, string _scriptName, string _displayName) :
		control(_command), scriptName(_scriptName), displayName(_displayName), mappable(_mappable), readOnly(_readOnly) {}

	ctl_id_t control;
	string scriptName;
	string displayName;
	bool mappable;
	bool readOnly;

	static Control *find(string nm);
};