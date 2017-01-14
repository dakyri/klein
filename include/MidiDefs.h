#ifndef _MIDIDEFS_H
#define _MIDIDEFS_H

typedef unsigned char uchar;
typedef unsigned char midi_cmd_t;
typedef unsigned char midi_val_t;
typedef unsigned char midi_ctl_t;
typedef unsigned char midi_note_t;
typedef unsigned char midi_prog_t; 
typedef unsigned char midi_vel_t;
typedef unsigned char midi_what_t;

inline midi_cmd_t midiCmd(uchar in) { return in & 0xf0;  }
inline midi_val_t midiChan(uchar in) { return in & 0x0f; }
inline midi_cmd_t midiCmd(char * in) { return in[0] & 0xf0; }
inline midi_val_t midiChan(char * in) { return in[0] & 0x0f; }
inline midi_note_t midiNote(char * in) { return in[1] & 0x7f; }
inline midi_ctl_t midiCtrl(char * in) { return in[1] & 0x7f; }
inline midi_prog_t midiProg(char * in) { return in[1] & 0x7f; }
inline midi_vel_t midiNoteVel(char * in) { return in[2] & 0x7f; }
inline midi_val_t midiCtrlVal(char * in) { return in[2] & 0x7f; }

const midi_cmd_t MIDI_CMD_NOT	= 0x00;
const midi_cmd_t MIDI_NOTE_OFF	= 0x80;
const midi_cmd_t MIDI_NOTE_ON	= 0x90;
const midi_cmd_t MIDI_KEY_PRESS	= 0xa0;
const midi_cmd_t MIDI_CTRL		= 0xb0;
const midi_cmd_t MIDI_PROG		= 0xc0;
const midi_cmd_t MIDI_CHAN_PRESS = 0xd0;
const midi_cmd_t MIDI_BEND		= 0xe0;
const midi_cmd_t MIDI_SYS		= 0xf0;


const midi_cmd_t MIDI_SYSX_START = 0xf0;
const midi_cmd_t MIDI_TIME_CODE	= 0xf1;
const midi_cmd_t MIDI_SONG_POS	= 0xf2;
const midi_cmd_t MIDI_SONG_SEL	= 0xf3;
const midi_cmd_t MIDI_CABLE_MSG	= 0xf5;
const midi_cmd_t MIDI_TUNE_REQ	= 0xf6;
const midi_cmd_t MIDI_SYSX_END	= 0xf7;
const midi_cmd_t MIDI_CLOCK		= 0xf8;
const midi_cmd_t MIDI_START		= 0xfa;
const midi_cmd_t MIDI_CONT		= 0xfb;
const midi_cmd_t MIDI_STOP		= 0xfc;
const midi_cmd_t MIDI_SENSING	= 0xfe;
const midi_cmd_t MIDI_SYS_RESET	= 0xff;

const midi_ctl_t MIDICTL_MODULATION			= 0x01;
const midi_ctl_t MIDICTL_BREATH_CONTROLLER	= 0x02;
const midi_ctl_t MIDICTL_FOOT_CONTROLLER	= 0x04;
const midi_ctl_t MIDICTL_PORTAMENTO_TIME	= 0x05;
const midi_ctl_t MIDICTL_DATA_ENTRY			= 0x06;
const midi_ctl_t MIDICTL_MAIN_VOLUME		= 0x07;
const midi_ctl_t MIDICTL_MIDI_BALANCE		= 0x08;
const midi_ctl_t MIDICTL_PAN				= 0x0a;
const midi_ctl_t MIDICTL_EXPRESSION_CTRL	= 0x0b;
const midi_ctl_t MIDICTL_GENERAL_1			= 0x10;
const midi_ctl_t MIDICTL_GENERAL_2			= 0x11;
const midi_ctl_t MIDICTL_GENERAL_3			= 0x12;
const midi_ctl_t MIDICTL_GENERAL_4			= 0x13;
const midi_ctl_t MIDICTL_SUSTAIN_PEDAL		= 0x40;
const midi_ctl_t MIDICTL_PORTAMENTO			= 0x41;
const midi_ctl_t MIDICTL_SOSTENUTO			= 0x42;
const midi_ctl_t MIDICTL_SOFT_PEDAL			= 0x43;
const midi_ctl_t MIDICTL_HOLD_2				= 0x45;
const midi_ctl_t MIDICTL_GENERAL_CTRL_5		= 0x50;
const midi_ctl_t MIDICTL_GENERAL_CTRL_6		= 0x51;
const midi_ctl_t MIDICTL_GENERAL_CTRL_7		= 0x52;
const midi_ctl_t MIDICTL_GENERAL_CTRL_8		= 0x53;
const midi_ctl_t MIDICTL_EFFECTS_DEPTH		= 0x5b;
const midi_ctl_t MIDICTL_TREMOLO_DEPTH		= 0x5c;
const midi_ctl_t MIDICTL_CHORUS_DEPTH		= 0x5d;
const midi_ctl_t MIDICTL_CELESTE_DEPTH		= 0x5e;
const midi_ctl_t MIDICTL_PHASER_DEPTH		= 0x5f;
const midi_ctl_t MIDICTL_DATA_INCREMENT		= 0x60;
const midi_ctl_t MIDICTL_DATA_DECREMENT		= 0x61;
const midi_ctl_t MIDICTL_RESET_ALL			= 0x79;
const midi_ctl_t MIDICTL_LOCAL_CONTROL		= 0x7a;
const midi_ctl_t MIDICTL_ALL_NOTES_OFF		= 0x7b;
const midi_ctl_t MIDICTL_OMNI_MODE_OFF		= 0x7c;
const midi_ctl_t MIDICTL_OMNI_MODE_ON		= 0x7d;
const midi_ctl_t MIDICTL_MONO_MODE_ON		= 0x7e;
const midi_ctl_t MIDICTL_POLY_MODE_ON		= 0x7f;

const midi_ctl_t MIDICTL_TEMPO_CHANGE		= 0x51;


#define NOTE_C	0
#define NOTE_C_	1
#define NOTE_D	2
#define NOTE_D_	3
#define NOTE_E	4
#define NOTE_F	5
#define NOTE_F_	6
#define NOTE_G	7
#define NOTE_G_	8
#define NOTE_A	9
#define NOTE_A_	10
#define NOTE_B	11
#define N_NOTES 12

#endif
	

