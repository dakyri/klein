#include "KleinTrack.h"

#include "aeffectx.h"

#include <unordered_map>
#include <algorithm>

using namespace std;

// or maybe boost::bimap

unordered_map<string, SyncSource> syncSrc {
	{ "default", DFLT_SRC },
	{ "host", SYNC_HOST },
	{ "track", SYNC_TRACK },
	{ "midi", SYNC_MIDI }
};

unordered_map<string, SyncUnit> syncUnit{
	{ "default", DFLT_UNIT },
	{ "loop", SYNC_LOOP },
	{ "cycle", SYNC_CYCLE },
	{ "subCycle", SYNC_SUBCYCLE }
};


SyncSource syncSrc4(string s) {
	auto p = syncSrc.find(s);
	if (p != syncSrc.end()) {
		return p->second;
	}
	return DFLT_SRC;
}

SyncUnit syncUnit4(string s) {
	auto p = syncUnit.find(s);
	if (p != syncUnit.end()) {
		return p->second;
	}
	return DFLT_UNIT;
}

string syncSrc4(SyncSource s) {
	auto p = find_if(syncSrc.begin(), syncSrc.end(), [s](pair<string, SyncSource> t) { return t.second == s; });
	if (p != syncSrc.end()) {
		return p->first;
	}
	return "default";
}

string syncUnit4(SyncUnit s) {
	auto p = find_if(syncUnit.begin(), syncUnit.end(), [s](pair<string, SyncUnit> t) { return t.second == s; });
	if (p != syncUnit.end()) {
		return p->first;
	}
	return "default";
}


KleinTrack::KleinTrack(int _trackId, int nLoops)
	: KleinTrack(_trackId, 0, 0, nLoops, SYNC_HOST, SYNC_LOOP)
{
}

KleinTrack::KleinTrack(int _trackId, int _inPortId, int _outPortId, int nLoops, SyncSource _syncSrc, SyncUnit _syncUnit)
	: id(_trackId)
	, inPortId(_inPortId)
	, outPortId(_outPortId)
	, syncSrc(_syncSrc)
	, syncUnit(_syncUnit)
{
	setNLoops(nLoops);
}


KleinTrack::~KleinTrack()
{
}

void
KleinTrack::setInputGain(int v) {

}

void
KleinTrack::setOutputGain(int v) {

}

void
KleinTrack::setPan(int v) {

}

void
KleinTrack::setFeedback(int v) {

}

void
KleinTrack::setSecondaryFeedback(int v) {

}

void
KleinTrack::setPitchBend(int v) {

}

void
KleinTrack::setPitchOct(int v) {

}

void
KleinTrack::setPitchStep(int v) {

}

void
KleinTrack::setSpeedBend(int v) {

}

void
KleinTrack::setSpeedOct(int v) {

}

void
KleinTrack::setSpeedStep(int v) {

}

void
KleinTrack::setTimeStretch(int v) {

}

void
KleinTrack::setInPort(const int port) {
	inPortId = port;
}

void
KleinTrack::setOutPort(const int port) {
	outPortId = port;
}

void KleinTrack::setNLoops(const int nLoops)
{
	loops.clear();
	for (int i = 0; i < nLoops; i++) {
		loops.push_back(SampleLoop());
	}
}

void KleinTrack::setSyncSrc(const SyncSource ss) {
	syncSrc = ss;
}

void KleinTrack::setSyncUnit(const SyncUnit su) {
	syncUnit = su;
}

/**
 * process a slice up until the point something interesting happens
 */
float KleinTrack::processAdding(float ** const inputs, float ** const outputs, const long startOffset, const long sampleFrames)
{
	const int ii = inPortId * 2;
	const int oi = outPortId * 2;
	float * const inl = inputs[ii] + startOffset;
	float * const inr = inputs[ii +1] + startOffset;
	float * const outl = outputs[oi] + startOffset;
	float * const outr = outputs[oi+1] + startOffset;
	/*

	beatT = 60/getTempo();
	if (fTapLevel[0] > 0) {
	delayT[0] = beatT * fTapDelay[0];// delay in secs
	lGain[0] = fTapLevel[0]*(1-fTapPan[0]);
	rGain[0] = fTapLevel[0]*(1+fTapPan[0]);
	} else {
	lGain[0] = rGain[0] = 0;
	}
	directL = fDirectLevel*(1-fDirectPan);
	directR = fDirectLevel*(1+fDirectPan);


	for (short i=0; i<NSGDelayLFO; i++) {
	lfd[i] = fLFODepth[i];
	}

	feedback = fFeedback;

	#if (KLEIN_DEBUG >= 10)
	dbf << "process replacing nf " << nFrames << " " << SGDelayChunkFrames << " " << (nFrames / SGDelayChunkFrames) << " flt " <<
	" gain " << lGain[0] << " " << rGain[0] << endl;
	#endif
	long	outFrame = 0;

	while (outFrame < nFrames) {

	short j;
	if (nChunkFrameRemaining == 0) {
	LFOCheck(0);
	nChunkFrames = nFrames - outFrame;
	if (nChunkFrames > SGDelayChunkFrames) {
	nChunkFrames = SGDelayChunkFrames;
	}
	nChunkFrameRemaining = SGDelayChunkFrames;
	} else {
	nChunkFrames = nChunkFrameRemaining;
	}

	//		delayLine->Write(inl, inr, nChunkFrames, 1-feedback, false);


	//		delayLine->Read(tapSig[0], nChunkFrames, delayT[0]);
	float	*pl, *pr;
	if (fTapChannelSwap[0]) {
	pr = tapSig[0];
	pl = tapSig[0]+1;
	} else {
	pl = tapSig[0];
	pr = tapSig[0]+1;
	}
	for (j=0; j<nChunkFrames; j++) {
	//			outl[j] = inl[j] + lfilt[0](*pl)*lGain[0];
	//			outr[j] = inr[j] + rfilt[0](*pr)*rGain[0];
	if (outl[j] > cvu) cvu = outl[j];
	if (outr[j] > cvu) cvu = outr[j];
	pl+=2;pr+=2;
	}

	//		delayLine->Overdub(outl, outr, nChunkFrames, feedback, true);

	outFrame += nChunkFrames;
	outl += nChunkFrames;
	outr += nChunkFrames;
	inl += nChunkFrames;
	inr += nChunkFrames;

	nChunkFrameRemaining -= nChunkFrames;
	}

	*/
	return 0.0f;
}

long KleinTrack::boringFrames(const VstTimeInfo * const t, const long startOffset)
{
	return 0;
}
