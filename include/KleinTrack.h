#pragma once

#include <vector>
#include <memory>
#include <string>

#include "Bufferator.h"
#include "aeffectx.h"

enum SyncSource {
	DFLT_SRC,
	SYNC_HOST,
	SYNC_TRACK,
	SYNC_MIDI
};

enum SyncUnit {
	DFLT_UNIT,
	SYNC_LOOP,
	SYNC_CYCLE,
	SYNC_SUBCYCLE
};

SyncSource syncSrc4(string s);
SyncUnit syncUnit4(string s);
string syncSrc4(SyncSource s);
string syncUnit4(SyncUnit s);

class SampleLoop {
	shared_ptr<SampleInfo> sampleInfo;
};

class KleinTrack
{
public:
	KleinTrack(int trackId, int nLoops);
	KleinTrack(int trackId, int inPortId, int outPortId, int nLoops, SyncSource syncSrc, SyncUnit syncUnit);

	virtual ~KleinTrack();

	void setInputGain(int v);
	void setOutputGain(int v);
	void setPan(int v);
	void setFeedback(int v);
	void setSecondaryFeedback(int v);
	void setPitchBend(int v);
	void setPitchOct(int v);
	void setPitchStep(int v);
	void setSpeedBend(int v);
	void setSpeedOct(int v);
	void setSpeedStep(int v);
	void setTimeStretch(int v);

	void setInPort(const int port);
	void setOutPort(const int port);

	void setNLoops(const int nLoops);

	void setSyncSrc(const SyncSource ss);
	void setSyncUnit(const SyncUnit su);

	float processAdding(float ** const inputs, float ** const  outputs, const long startOffset, const long sampleFrames);
	long boringFrames(const VstTimeInfo * const t, const long startOffset);

protected:
	int id;
	int inPortId;
	int outPortId;

	SyncSource syncSrc;
	SyncUnit syncUnit;

	bool mute;

	float inputGain;
	float outputGain;
	float pan;
	float feedback;
	float secondaryFeedback;
	float pitchBend;
	float speedBend;
	float timeStretch;

	vector<SampleLoop> loops;
};

