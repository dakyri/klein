#include "Controller.h"
#include "Klein.h"


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
	long status = midiData[0] & 0xf0;		// ignoring channel
	fprintf(stderr, "command %x in xsynth\n", status);
	if (status == 0x90 || status == 0x80)	// we only look at notes
	{
		long note = midiData[1] & 0x7f;
		long velocity = midiData[2] & 0x7f;
		if (status == 0x80)
			velocity = 0;	// note off by velocity 0
							/*
							if (!velocity && (note == currentNote))
							noteOff();
							else
							noteOn(note, velocity, event->deltaFrames);
							*/
	}
	else if (status == 0xb0) {
		/*
		if (midiData[1] == 0x7e || midiData[1] == 0x7b)	// all notes off
		noteOff();
		*/
	}
	e++;
	return true;
}
