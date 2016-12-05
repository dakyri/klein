#ifndef _KLEIN_H
#define _KLEIN_H


#include "audioeffectx.h"
#include <string>
#include <unordered_map>

using namespace std;

#include "Bufferator.h"
#include "Delay.h"

#define MaxSGDelayTime	4.0
#define MinSGDelayBeats	0.0
#define MaxSGDelayBeats	8.0
#define MinSGDelayLFRate	0.1
#define MaxSGDelayLFRate	64.0
#define MaxTapGain	2.0

#define NSGDelayLFO	1
#define NSGDelayFilter	1
#define NSGDelayTap	1

#define SGDelayChunkFrames	64

enum
{
	kTap1Delay,
	kTap1Level,
	kTap1Pan,
	kTap1ChannelSwap,
	kTap1SendDest,
	kFeedback,
	kFilter1Type,
	kFilter1Frequency,
	kFilter1Resonance,
	kFilter1Enable,
	kLFO1Rate,
	kLFO1Depth,
	kLFO1Type,
	kLFO1Target,

	kOut,
	kDirectLevel,
	kDirectPan,

	kNumParams
};

class Klein;

class KleinProgram
{
	friend class Klein;
public:
	KleinProgram();
	~KleinProgram() {}
private:
	float fTapDelay[NSGDelayTap];
	float fTapLevel[NSGDelayTap];
	float fTapPan[NSGDelayTap];
	float fFeedback;
	float fFilterFrequency[NSGDelayFilter];
	float fFilterResonance[NSGDelayFilter];
	float fOut;
	float fDirectLevel;
	float fDirectPan;
	long fFilterType[NSGDelayFilter];
	float fLFORate[NSGDelayLFO];
	float fLFODepth[NSGDelayLFO];
	long fLFOType[NSGDelayLFO];
	long fLFOTarget[NSGDelayLFO];
	bool fFilterEnable[NSGDelayFilter];
	bool fTapChannelSwap[NSGDelayTap];
	short fTapSendDest[NSGDelayTap];
	char name[24];
};


class Klein : public AudioEffectX
{
public:
	Klein(audioMasterCallback audioMaster);
	virtual ~Klein();

/* VST callbacks */
	virtual void process(float **inputs, float **outputs, long sampleframes);
	virtual void processReplacing(float **inputs, float **outputs, long sampleFrames);
	virtual void setProgram(long program);
	virtual void setProgramName(char *name);
	virtual void getProgramName(char *name);
	virtual void setParameter(long index, float value);
	virtual float getParameter(long index);
	virtual void getParameterLabel(long index, char *label);
	virtual void getParameterDisplay(long index, char *text);
	virtual void getParameterName(long index, char *text);
	virtual float getVu();
	virtual void suspend();

	char *parameterName(long index);

private:
	void setFilterEnabled(short i, bool en);
	void setFilterType(short i, long t);
	void setFrequency(short i, float f);
	void setResonance(short i, float rez);
	void setLFDepth(short, float);
	void setLFRate(short, float);
	void setLFWave(short, short);

	void subProcessReplacing(float *in1, float *in2, float *out1, float *out2,
								float *dest, float *del, long sampleframes);
	float getTempo();

	void LFOCheck(short which);
	KleinProgram *programs;

	Delay delayLine;

	float *tapSig[NSGDelayTap];

	float			fTapDelay[NSGDelayTap];
	float			fTapLevel[NSGDelayTap];
	float			fTapPan[NSGDelayTap];
	bool			fTapChannelSwap[NSGDelayTap];
	short			fTapSendDest[NSGDelayTap];

//	SGFilter		lfilt[NSGDelayFilter];
//	SGFilter		rfilt[NSGDelayFilter];

	float			*lFilterin[NSGDelayFilter];
	float			*rFilterin[NSGDelayFilter];

	float			fFilterResonance[NSGDelayFilter];
	float			fFilterFrequency[NSGDelayFilter];
	long			fFilterType[NSGDelayFilter];
	bool			fFilterEnable[NSGDelayFilter];

//	SGLFO			lfo[NSGDelayLFO];

	float			fLFORate[NSGDelayLFO];
	float			fLFODepth[NSGDelayLFO];
	long			fLFOType[NSGDelayLFO];
	long			fLFOTarget[NSGDelayLFO];

	float			fFeedback;

	float			fOut;
	float			fDirectLevel;
	float			fDirectPan;

	long			nChunkFrames;
	long			nChunkFrameRemaining;

	float			vu;
private:
	float			beatT;
	float			delayT[NSGDelayTap];// delay in secs
	float			lGain[NSGDelayTap];
	float			rGain[NSGDelayTap];
	float			feedback;
	float			directL, directR;
	float			lfd[NSGDelayLFO];
};

extern unordered_map<string, int> kvFilterIdx;
extern unordered_map<string, int> kvLFTargetIdx;
extern unordered_map<string, int> kvLFWaveformIdx;

#endif