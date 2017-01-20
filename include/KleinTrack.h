#ifndef __KLEIN_TRACK
#define __KLEIN_TRACK

#include <vector>
#include <memory>
#include <string>
#include <list>
#include <mutex>

#include "Bufferator.h"
#include "aeffectx.h"
#include "KLFunValue.h"
#include "Command.h"

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
	TRAK_REC,
	TRAK_DUB,
	TRAK_INSERT,
	TRAK_DELETE,
	TRAK_DEAD
};

enum PlayDirection {
	PLAY_FWD,
	PLAY_BAK,
	PLAY_FWDBAK,
	PLAY_BAKFWD
};

TrackMode trackMode4(string s);
string trackMode4(TrackMode s);
SyncSource syncSrc4(string s);
string syncSrc4(SyncSource s);
SyncUnit syncUnit4(string s);
string syncUnit4(SyncUnit s);
PlayDirection playDir4(string s);
string playDir4(PlayDirection s);

/**
 *
 * 1 file per loop. layers shown as loop points.
 * a lot of the time files won't be written out ... probably only on very long ones
 */
class SampleLoop {
public:
	bool initLoop();
	bool reset();

	shared_ptr<SampleInfo> sampleInfo;
	vector<int> layers; /** array of layer offsets */
};

class KleinTrack
{
public:
	KleinTrack(int trackId, int nLoops);
	KleinTrack(int trackId, int inPortId, int outPortId, int nLoops, SyncSource syncSrc, SyncUnit syncUnit);

	virtual ~KleinTrack();

	void setInputGain(int v);
	void setInputPan(int v);
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

	float getInputGain();
	float getInputPan();
	float getOutputGain();
	float getPan();
	float getFeedback();
	float getSecondaryFeedback();

	void setInPort(const int port) const;
	void setOutPort(const int port) const;
	int getInPort() const;
	int getOutPort() const;

	void setNLoops(const int nLoops);

	void setSyncSrc(const SyncSource ss);
	void setSyncUnit(const SyncUnit su);
	void setPlayDirection(const PlayDirection p);
// getters for track properties
	TrackMode getMode();
	SyncSource getSyncSrc();
	SyncUnit getSyncUnit();
	PlayDirection getPlayDirection();

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
	bool isRecording();

	void allocateBuffers(long blocksize);

	void pushCommand(CommandStackItem & c);
	void clearCommands();

	static string getUniqueFilename();

	// commands
	bool play();
	bool pause();
	bool doLoop(int i);
	bool doMute();
	bool overdub();
	bool record();

	// constants
	static const unsigned int framesPerControlCycle = 64;
	static const unsigned int nOutChannels = 2;
protected:
	int playChunk(
		const shared_ptr<SampleInfo>& csf, float * const loopOutBuf, float * const loopRawBuf, const int nRequestedFrames, const int currentDataFrame, const bool directionFwd);
	void calculateControlsStereo(float &l, float &r);
	void loopEnd(const shared_ptr<SampleInfo>& csf);
	void doSync();
	long getTotalCurrentSampleFrames();
	int addToRecordBuffer(const shared_ptr<SampleInfo> & csf, const float gain, float * const inl, float * const inr, const int nFrames, bool inc = true);
	int addToRecordBuffer(const shared_ptr<SampleInfo> & csf, const float gain, float * const ins, const int nFrames, bool inc = true);
	void startRecordBuffer(const shared_ptr<SampleInfo> & csf, const int startFrame);
	void finalizeRecord(const shared_ptr<SampleInfo> & csf);

	vector<SampleLoop>::iterator allocateSampleLoop();
 
	int id;
	mutable int inPortId;
	mutable int outPortId;
	bool selected;

	vector<SampleLoop>::iterator currentSampleLoop;
	int loopStartFrame;
	int loopLengthFrames;

	int currentLoopFrame;
	int controlFrame;
	bool currentDirectionFwd;

	float vu;

	SyncSource syncSrc;
	SyncUnit syncUnit;
	PlayDirection playDirection;
	TrackMode trackMode;

	bool mute;

	float inputGain;
	float inputPan;
	float outputGain;
	float pan;
	float feedback;
	float secondaryFeedback;
	float pitchBend;
	float speedBend;
	float timeStretch;

	float *loopOutBuf;
	float *loopRawBuf;
	float *leftCycleBuffer;
	float *rightCycleBuffer;

	float *recordBuffer;
	int recordBufferLen;
	int nFramesRecorded;
	int recordStartFrame;

	vector<SampleLoop> loops;

	mutex commandsLock;
	vector<CommandStackItem> commands;

	float lAmp;
	float rAmp;
	float tune; /** ???? probably not the way we're going to do this. placeholder for more useful pitch and timestretch */
	float psi;
	float lastFraction;
	int nextDataFrame;

};

void writeToBufferMono(const float gain, float * const inl, float * const outl, const int nFrames);
void writeToBufferPair(const float gain, float * const ins, float * const outl, float * const outr, const int nFrames);

#include "debug.h"

#endif