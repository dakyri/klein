#pragma once

#include <vector>
#include <memory>
#include <string>
#include <list>
#include <mutex>

#include "Bufferator.h"
#include "aeffectx.h"
#include "KLFunValue.h"

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

enum TrackMode {
	TRAK_STOP,
	TRAK_PEND,
	TRAK_PAUSE,
	TRAK_PLAY,
	TRAK_DEAD,
	TRAK_REC,
	TRAK_DUB,
	TRAK_INSERT,
	TRAK_DELETE,
	TRAK_DEAD
};

TrackMode trackMode4(string s);
string trackMode4(TrackMode s);
SyncSource syncSrc4(string s);
string syncSrc4(SyncSource s);
SyncUnit syncUnit4(string s);
string syncUnit4(SyncUnit s);

class SampleLoop {
public:
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
	
	void recordStart(const ktime_t &at);
	void recordStop(const ktime_t &at);
	void overdubStart();
	void overdubStop();

	void selectLoop(int i);
	void nextLoop();
	void prevLoop();

	float getVu();

	long processAdding(float ** const inputs, float ** const  outputs, const long startOffset, const long sampleFrames);
	long boringFrames(const VstTimeInfo * const t, const long startOffset);
	bool isPlaying();

	static const unsigned int framesPerControlCycle = 64;
	static const unsigned int nOutChannels = 2;
protected:
	int playChunk(
		float *buffL, float *buffR, int nRequestedFrames, int currentDataFrame,
		float *chunkData, int chunkStartFrame, int chunkNFrames, float nextBufferStartL, float nextBufferStartR, bool directionFwd);
	void calculateControlsStereo(float &l, float &r);
	void loopEnd();
	long getTotalCurrentSampleFrames();

	int id;
	int inPortId;
	int outPortId;

	vector<SampleLoop>::iterator currentSampleLoop;
	int loopStartFrame;
	int loopLengthFrames;

	int currentLoopFrame;
	int controlFrame;
	bool currentDirectionFwd;

	float vu;

	SyncSource syncSrc;
	SyncUnit syncUnit;

	TrackMode trackMode;

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

	float lAmp;
	float rAmp;
	float tune; /** ???? probably not the way we're going to do this. placeholder for more useful pitch and timestretch */
	float psi;
	float lastFraction;
	int nextDataFrame;

	float *leftCycleBuffer;
	float *rightCycleBuffer;
};
