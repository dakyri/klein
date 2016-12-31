#ifndef _MIDIDEFS_H
#define _MIDIDEFS_H

typedef unsigned char uchar;

inline uchar midiCmd(uchar in) { return in & 0xf0;  }
inline uchar midiChan(uchar in) { return in & 0x0f; }
inline uchar midiCmd(char * in) { return in[0] & 0xf0; }
inline uchar midiChan(char * in) { return in[0] & 0x0f; }
inline uchar midiNote(char * in) { return in[1] & 0x7f; }
inline uchar midiCtrl(char * in) { return in[1] & 0x7f; }
inline uchar midiProg(char * in) { return in[1] & 0x7f; }
inline uchar midiNoteVel(char * in) { return in[2] & 0x7f; }
inline uchar midiCtrlVal(char * in) { return in[2] & 0x7f; }

const uchar MIDI_CMD_NOT	= 0x00;
const uchar MIDI_NOTE_OFF	= 0x80;
const uchar MIDI_NOTE_ON	= 0x90;
const uchar MIDI_KEY_PRESS	= 0xa0;
const uchar MIDI_CTRL		= 0xb0;
const uchar MIDI_PROG		= 0xc0;
const uchar MIDI_CHAN_PRESS	= 0xd0;
const uchar MIDI_BEND		= 0xe0;
const uchar MIDI_SYS		= 0xf0;


const uchar MIDI_SYSX_START	= 0xf0;
const uchar MIDI_TIME_CODE	= 0xf1;
const uchar MIDI_SONG_POS	= 0xf2;
const uchar MIDI_SONG_SEL	= 0xf3;
const uchar MIDI_CABLE_MSG	= 0xf5;
const uchar MIDI_TUNE_REQ	= 0xf6;
const uchar MIDI_SYSX_END	= 0xf7;
const uchar MIDI_CLOCK		= 0xf8;
const uchar MIDI_START		= 0xfa;
const uchar MIDI_CONT		= 0xfb;
const uchar MIDI_STOP		= 0xfc;
const uchar MIDI_SENSING	= 0xfe;
const uchar MIDI_SYS_RESET	= 0xff;

const uchar	MIDICTL_MODULATION			= 0x01;
const uchar	MIDICTL_BREATH_CONTROLLER	= 0x02;
const uchar	MIDICTL_FOOT_CONTROLLER		= 0x04;
const uchar	MIDICTL_PORTAMENTO_TIME		= 0x05;
const uchar	MIDICTL_DATA_ENTRY			= 0x06;
const uchar	MIDICTL_MAIN_VOLUME			= 0x07;
const uchar	MIDICTL_MIDI_BALANCE		= 0x08;
const uchar MIDICTL_PAN					= 0x0a;
const uchar	MIDICTL_EXPRESSION_CTRL		= 0x0b;
const uchar	MIDICTL_GENERAL_1			= 0x10;
const uchar	MIDICTL_GENERAL_2			= 0x11;
const uchar	MIDICTL_GENERAL_3			= 0x12;
const uchar	MIDICTL_GENERAL_4			= 0x13;
const uchar	MIDICTL_SUSTAIN_PEDAL		= 0x40;
const uchar	MIDICTL_PORTAMENTO			= 0x41;
const uchar	MIDICTL_SOSTENUTO			= 0x42;
const uchar	MIDICTL_SOFT_PEDAL			= 0x43;
const uchar	MIDICTL_HOLD_2				= 0x45;
const uchar	MIDICTL_GENERAL_CTRL_5		= 0x50;
const uchar	MIDICTL_GENERAL_CTRL_6		= 0x51;
const uchar	MIDICTL_GENERAL_CTRL_7		= 0x52;
const uchar	MIDICTL_GENERAL_CTRL_8		= 0x53;
const uchar	MIDICTL_EFFECTS_DEPTH		= 0x5b;
const uchar	MIDICTL_TREMOLO_DEPTH		= 0x5c;
const uchar	MIDICTL_CHORUS_DEPTH		= 0x5d;
const uchar	MIDICTL_CELESTE_DEPTH		= 0x5e;
const uchar	MIDICTL_PHASER_DEPTH		= 0x5f;
const uchar	MIDICTL_DATA_INCREMENT		= 0x60;
const uchar	MIDICTL_DATA_DECREMENT		= 0x61;
const uchar	MIDICTL_RESET_ALL			= 0x79;
const uchar	MIDICTL_LOCAL_CONTROL		= 0x7a;
const uchar	MIDICTL_ALL_NOTES_OFF		= 0x7b;
const uchar	MIDICTL_OMNI_MODE_OFF		= 0x7c;
const uchar	MIDICTL_OMNI_MODE_ON		= 0x7d;
const uchar	MIDICTL_MONO_MODE_ON		= 0x7e;
const uchar	MIDICTL_POLY_MODE_ON		= 0x7f;

const uchar	MIDICTL_TEMPO_CHANGE		= 0x51;


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
	

