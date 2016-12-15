#include "KleinTrack.h"

#include "aeffectx.h"

#include <unordered_map>
#include <algorithm>
#include <iostream>

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


unordered_map<string, TrackMode> trackMode {
	{ "stop", TRAK_STOP },
	{ "pending", TRAK_PEND },
	{ "pause", TRAK_PAUSE },
	{ "play", TRAK_PLAY },
	{ "record", TRAK_REC },
	{ "dub", TRAK_DUB },
	{ "insert", TRAK_INSERT },
	{ "delete", TRAK_DELETE },
	{ "play", TRAK_PLAY }
};


TrackMode trackMode4(string s) {
	auto p = trackMode.find(s);
	if (p != trackMode.end()) {
		return p->second;
	}
	return TRAK_DEAD;
}


string trackMode4(TrackMode s) {
	auto p = find_if(trackMode.begin(), trackMode.end(), [s](pair<string, TrackMode> t) { return t.second == s; });
	if (p != trackMode.end()) {
		return p->first;
	}
	return "";
}

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
	, currentSampleLoop(loops.begin())
	, currentDirectionFwd(true)
	, psi(1)
	, lAmp(1)
	, rAmp(1)
	, tune(0)
	, lastFraction(0)
	, nextDataFrame(0)
{
	leftCycleBuffer = new float[framesPerControlCycle];
	rightCycleBuffer = new float[framesPerControlCycle];

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



void KleinTrack::recordStart(const ktime_t & at)
{
}

void KleinTrack::recordStop(const ktime_t & at)
{
}

void KleinTrack::overdubStart()
{
}

void KleinTrack::overdubStop()
{
}

void KleinTrack::selectLoop(int i)
{
}

void KleinTrack::nextLoop()
{
}

void KleinTrack::prevLoop()
{
}

/**
 * process a slice up until the point something interesting happens
 */
float KleinTrack::getVu()
{
	return vu;
}

/**
 * processes the current slice. the slices are chosen so as not to overlap the start or end of loop, synchronization, or other
 * significant event
 */
long KleinTrack::processAdding(float ** const inputs, float ** const outputs, const long startOffset, const long nFrames)
{
	const int ii = inPortId * 2;
	const int oi = outPortId * 2;
	float * const inl = inputs[ii] + startOffset;
	float * const inr = inputs[ii +1] + startOffset;
	float * const outl = outputs[oi] + startOffset;
	float * const outr = outputs[oi+1] + startOffset;
	float lvu = 0;
	long nFramesOut = 0;

	/*
	if selected

	if mode = record

	else if mode = overdub
		(gain * input) into interleaved buffer

	
	*/

	if (!isPlaying()) {
		return 0;
	}
	if (currentSampleLoop == loops.end()) {
		return 0;
	}
	shared_ptr<SampleInfo> csf = currentSampleLoop->sampleInfo;
	if (!csf) {
		return 0;
	}

	nFramesOut = 0;
	int ll = loopLengthFrames;
	if (ll < 2) ll = 2;
	int sf = loopStartFrame;
	int buffInd = 0;
	//		Log.d("player", String.format("pad %d to play %d %d", id, nFrames, nOutChannels));
	while (nFramesOut < nFrames) {
		int nFramesRemaining = nFrames - nFramesOut;
		int nIterFrames = currentDirectionFwd ?
			((nFramesRemaining > ll - currentLoopFrame) ? (ll - currentLoopFrame) : nFramesRemaining) :
			((nFramesRemaining > currentLoopFrame) ? currentLoopFrame : nFramesRemaining);
		//			Log.d("player", String.format("pad %d loop frame %d dir %d to make %d %d %d %d tune is %g", id, currentLoopFrame, currentDirection, nFrames, nFramesOut, nFramesRemaining, nIterFrames, state.tune));
		if (nIterFrames > 0) {
			int currentDataFrame = currentLoopFrame + sf;




			int scistart = 0;
			int scilength = 0;
			float* scidata = nullptr;
			SampleChunkInfo *sci = csf->getChunk4Frame(currentDataFrame);
			float nextL = 0;
			float nextR = 0;
			int cpageid = Bufferator::page4Frame(currentDataFrame);
			if (sci != nullptr) {
				scilength = sci->nFrames;
				scidata = sci->data;
				scistart = sci->startFrame;
				if (tune != 0) { // setup buffer endpoints if we're interpolating TODO or we are tuning it with lfo!!!!
					if (currentDirectionFwd) {
						SampleChunkInfo *scnxt = nullptr;
						if (sci->nextChunkStartFrame() >= sf + ll) { // next chunk is start of loop
							scnxt = csf->getChunk4Frame(sf);
						}
						else {
							scnxt = csf->getChunk4Frame(sci->nextChunkStartFrame());
						}
						if (scnxt != nullptr) {
							nextL = scnxt->data[0];
							nextR = scnxt->data[1];
						}
					} else { // backwards .
						SampleChunkInfo *scnxt = nullptr;
						if (sci->prevChunkEndFrame() < sf) { // next chunk is start of loop
							scnxt = csf->getChunk4Frame(sf);
						}
						else {
							scnxt = csf->getChunk4Frame(sci->prevChunkEndFrame());
						}
						if (scnxt != nullptr) {
							nextL = scnxt->data[scnxt->nFrames - 2];
							nextR = scnxt->data[scnxt->nFrames - 1];
						}
					}
				}
				cerr << "player got " << currentDataFrame << " chunk  " << " len " << scilength << "@  " << scistart
					<< " data[0]  " << scidata[0] << " path  " << csf->path << endl;
			} else {
				cerr << "player failed to find " << currentDataFrame << " chunk " << cpageid << " path " << csf->path << endl;
			}
			int nIterOutFrames = playChunk(
						outl+ buffInd, outr+buffInd, nIterFrames, currentDataFrame,
						scidata, scistart, scilength, nextL, nextR, currentDirectionFwd);
			if (cpageid >= 0) {
				csf->requestPage(cpageid); // will already be loaded, but we'll make sure it's there
				if (currentDirectionFwd) {
					csf->requestPage(cpageid + 1);
					csf->requestPage(cpageid + 2);
				}
				else {
					csf->requestPage(cpageid - 1);
					csf->requestPage(cpageid - 2);
				}
			}







			buffInd += nOutChannels*nIterOutFrames;
			currentLoopFrame = nextDataFrame - sf;
			nFramesOut += nIterOutFrames;
			//				Log.d("player", String.format("pad %d played %d %d requested %d loop %d data %d end %d", id, nIterOutFrames, nFramesOut, nIterFrames, currentLoopFrame, getNextDataFrame(cPadPointer), ll));
		}
		else {
			if (!isPlaying()) {
				break;
			}
		}
		if (currentLoopFrame >= ll) {
			if (currentDirectionFwd) {
				loopEnd();
			}
			else {
				currentLoopFrame = ll;
			}
		}
		else if (currentLoopFrame <= 0) {
			if (currentDirectionFwd) {
				currentLoopFrame = 0;
			}
			else {
				loopEnd();
			}
		}
	}

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
	vu = lvu;
	return nFrames;
}
void KleinTrack::loopEnd() {
	/*
	//		Log.d("player", "end of the loop "+currentLoopFrame + " ... "+ state.loopDirection);
	if (state.loopCount > 0 && ++currentLoopCount >= state.loopCount) {
		stop(true);
	}
	if (state.loopDirection == Direction.FWDBACK || state.loopDirection == Direction.BACKFWD) {
		currentDirection = -currentDirection;
	}
	if (currentDirection >= 0) {
		int lsb = Bufferator.chunk4Frame((int)state.loopStart);
		currentSampleInfo.requestChunk(lsb, 1);
		currentSampleInfo.requestChunk(lsb + 1, 1);
		currentLoopFrame = 0;
	}
	else {
		int ll1 = (int)(state.loopLength - 1);
		int lsb = Bufferator.chunk4Frame((int)(state.loopStart + ll1));
		currentSampleInfo.requestChunk(lsb, 1);
		currentSampleInfo.requestChunk(lsb - 1, 1);
		currentLoopFrame = ll1;
	}
	reloopCPad(cPadPointer);
	doSync();*/
}

long KleinTrack::getTotalCurrentSampleFrames()
{
	if (currentSampleLoop == loops.end()) {
		return 0;
	}
	shared_ptr<SampleInfo> csf = currentSampleLoop->sampleInfo;
	if (!csf) {
		return 0;
	}
	return csf->nTotalFrames;
}


/**
* process a given buffer of output. pulled from android CPadSample.cpp
* can probably be further simplified
*
* @param buff output buffer
* @param nRequestedFrames optimal number of frames to play.
* @param chunkData data etc for the chunk
* @param chunkStartFrame
* @param chunkNFrames
* @param nextBufferStartL start of 'next' buffer
* @param nextBufferStartR
* @param direction >= 0 for forwards, else backwards
*/

int
KleinTrack::playChunk(
	float *buffL, float *buffR, int nRequestedFrames, int currentDataFrame,
	float *chunkData, int chunkStartFrame, int chunkNFrames, float nextBufferStartL, float nextBufferStartR, bool directionFwd)
{
	const int nDataChannels = 2;
	int currentBufferOffset = 0;
	int nDataFramesAvailable = 0;
	if (chunkNFrames > 0) {
		currentBufferOffset = currentDataFrame - chunkStartFrame; // currentBufferOffest initially in frames
		if (currentBufferOffset < 0) {
			currentBufferOffset = 0;
		}
		nDataFramesAvailable = directionFwd ? (chunkNFrames - currentBufferOffset) : (currentBufferOffset + 1);
		if (nDataFramesAvailable < 0) {
			nDataFramesAvailable = 0;
		}
		currentBufferOffset = currentBufferOffset*nDataChannels; // currentBufferOffest now an index into buffer
	}

	float *lSig = leftCycleBuffer;
	float *rSig = rightCycleBuffer;
	unsigned int buffI = 0;
	unsigned int chunkI = currentBufferOffset;
	unsigned int nCycleFrames;
	unsigned int nControlChunkFrames;
	int nOutputFrames = 0;
	float lRaw1 = 0;
	float rRaw1 = 0;
	float lRaw2 = 0;
	float rRaw2 = 0;
	long lframe = -1;
	long frame = -1;
	long nextBufferStartFrame = directionFwd ? (chunkStartFrame + chunkNFrames) : (chunkStartFrame - 1);

	bool pitchShifting = (tune != 0);
	double phs = 0;
	if (currentDataFrame != nextDataFrame) {
		phs = currentDataFrame;
	}
	else {
		phs = currentDataFrame + lastFraction;
	}

	float crAmp = rAmp;
	float clAmp = lAmp;
	if (nDataChannels == 2) {
		nCycleFrames = (nDataFramesAvailable > 0) ? nRequestedFrames : 0; // we just check this egde case ... there isn't a 1:1 correspondence between requirements and data consumed
		while (nCycleFrames > 0) {
			if (controlFrame == 0) {
				calculateControlsStereo(clAmp, crAmp);
			}
			nControlChunkFrames = (controlFrame + nCycleFrames > framesPerControlCycle) ? (framesPerControlCycle - controlFrame) : nCycleFrames;
			if (!pitchShifting) {
				if (nOutputFrames + nControlChunkFrames > nDataFramesAvailable) {
					nCycleFrames = nControlChunkFrames = nDataFramesAvailable - nOutputFrames;
				}
				if (directionFwd) { // forwards, un pitch shifted, stereo sample data into a stereo out
					for (unsigned int i = 0; i<nControlChunkFrames; i++) {
						lSig[i] = chunkData[chunkI++];
						rSig[i] = chunkData[chunkI++];
					}
				}
				else { // backwards, un pitch shifted, stereo sample data into a stereo out
					for (unsigned int i = 0; i<nControlChunkFrames; i++) {
						lSig[i] = chunkData[chunkI--];
						rSig[i] = chunkData[chunkI--];
					}
				}
			}
			else {
				lframe = frame = -1;
				lRaw1 = rRaw1 = lRaw2 = rRaw2 = 0;
				if (directionFwd) { // forwards, pitch shifted, stereo sample data into a stereo out
					float rpsi = psi;
					for (unsigned int i = 0; i<nControlChunkFrames; i++) {
						lframe = frame;
						frame = (long)floor(phs);
						float frac = phs - frame;
						if (frame != lframe) {
							if (frame >= nextBufferStartFrame) { // hit the end of data
								nCycleFrames = nControlChunkFrames = i;
								break;
							}
							else {
								chunkI = 2 * (frame - chunkStartFrame);
								lRaw1 = chunkData[chunkI];
								rRaw1 = chunkData[chunkI + 1];
								if (frame >= nextBufferStartFrame - 1) {
									lRaw2 = nextBufferStartL;
									rRaw2 = nextBufferStartR;
								}
								else {
									lRaw2 = chunkData[chunkI + 2];
									rRaw2 = chunkData[chunkI + 3];
								}
							}
						}
						lSig[i] = interpolate(lRaw1, lRaw2, frac);
						rSig[i] = interpolate(rRaw1, rRaw2, frac);
						phs += rpsi;
					}
				}
				else { // backwards, pitch shifted, stereo sample data into a stereo out
					float rpsi = -psi;
					for (unsigned int i = 0; i<nControlChunkFrames; i++) {
						lframe = frame;
						frame = (long)floor(phs);
						float frac = phs - frame;
						if (frame != lframe) {
							if (frame <= nextBufferStartFrame) { // hit the end of data
								nCycleFrames = nControlChunkFrames = i;
								break;
							}
							else {
								chunkI = 2 * (frame - chunkStartFrame);
								lRaw1 = chunkData[chunkI];
								rRaw1 = chunkData[chunkI + 1];
								if (frame <= nextBufferStartFrame + 1) {
									lRaw2 = nextBufferStartL;
									rRaw2 = nextBufferStartR;
								}
								else {
									lRaw2 = chunkData[chunkI - 2];
									rRaw2 = chunkData[chunkI - 1];
								}
							}
						}
						lSig[i] = interpolate(lRaw1, lRaw2, frac);
						rSig[i] = interpolate(rRaw1, rRaw2, frac);
						phs += rpsi;
					}
				}
			}
//			lFilter.apply(lSig, nControlChunkFrames);
//			rFilter.apply(rSig, nControlChunkFrames);
			for (unsigned int i = 0; i<nControlChunkFrames; i++, buffI ++) {
				buffL[buffI] += clAmp*lSig[i];
				buffR[buffI] += crAmp*rSig[i];
			}
			if ((controlFrame += nControlChunkFrames) >= framesPerControlCycle) {
				controlFrame = 0;
			}
			nCycleFrames -= nControlChunkFrames;
			nOutputFrames += nControlChunkFrames;
		}

		/* out of data. process fx tail. this may not be relevant */
		if ((!pitchShifting ? currentDataFrame + nOutputFrames : floor(phs)) >= getTotalCurrentSampleFrames() || nDataFramesAvailable <= 0) {
			nCycleFrames = nRequestedFrames - nOutputFrames;
			while (nCycleFrames > 0) {
				if (controlFrame == 0) {
					calculateControlsStereo(clAmp, crAmp);
				}
				nControlChunkFrames = (controlFrame + nCycleFrames > framesPerControlCycle) ? (framesPerControlCycle - controlFrame) : nCycleFrames;
				memset(rSig, 0, nControlChunkFrames * sizeof(float));
				memset(lSig, 0, nControlChunkFrames * sizeof(float));
//				lFilter.apply(lSig, nControlChunkFrames);
//				rFilter.apply(rSig, nControlChunkFrames);
				for (unsigned int i = 0; i<nControlChunkFrames; i++, buffI ++) {
					buffL[buffI] += clAmp*lSig[i];
					buffR[buffI] += crAmp*rSig[i];
				}
				if ((controlFrame += nControlChunkFrames) >= framesPerControlCycle) {
					controlFrame = 0;
				}
				nCycleFrames -= nControlChunkFrames;
				nOutputFrames += nControlChunkFrames;
				float rpsi = directionFwd ? psi : -psi;
				phs += rpsi*nControlChunkFrames;
			}
		}
	}
	else if (nDataChannels == 1) {
		nCycleFrames = (nDataFramesAvailable > 0) ? nRequestedFrames : 0; // we just check this egde case ... there isn't a 1:1 correspondence between requirements and data consumed
		while (nCycleFrames > 0) {
			if (controlFrame == 0) {
				calculateControlsStereo(clAmp, crAmp);
			}
			nControlChunkFrames = (controlFrame + nCycleFrames > framesPerControlCycle) ? (framesPerControlCycle - controlFrame) : nCycleFrames;
			if (!pitchShifting) {
				if (nOutputFrames + nControlChunkFrames > nDataFramesAvailable) {
					nCycleFrames = nControlChunkFrames = nDataFramesAvailable - nOutputFrames;
				}
				if (directionFwd) { // forwards, un pitch shifted, mono sample data into a stereo out
					for (unsigned int i = 0; i<nControlChunkFrames; i++) {
						lSig[i] = chunkData[chunkI++];
					}
				}
				else {  // backwards, un pitch shifted, mono sample data into a stereo out
					for (unsigned int i = 0; i<nControlChunkFrames; i++) {
						lSig[i] = chunkData[chunkI--];
					}
				}
			}
			else {
				lframe = frame = -1;
				lRaw1 = rRaw1 = lRaw2 = rRaw2 = 0;
				if (directionFwd) { // forwards, pitch shifted, single channel sample data into a stereo out
					float rpsi = psi;
					for (unsigned int i = 0; i<nControlChunkFrames; i++) {
						lframe = frame;
						frame = (long)floor(phs);
						float frac = phs - frame;
						if (frame != lframe) {
							if (frame >= nextBufferStartFrame) { // hit the end of data
								nCycleFrames = nControlChunkFrames = i;
								break;
							}
							else {
								chunkI = frame - chunkStartFrame;
								lRaw1 = chunkData[chunkI];
								if (frame >= nextBufferStartFrame - 1) {
									lRaw2 = nextBufferStartL;
								}
								else {
									lRaw2 = chunkData[chunkI + 1];
								}
							}
						}
						lSig[i] = interpolate(lRaw1, lRaw2, frac);
						phs += rpsi;
					}
				}
				else { // backwards, pitch shifted, single channel sample data into a stereo out
					float rpsi = -psi;
					for (unsigned int i = 0; i<nControlChunkFrames; i++) {
						lframe = frame;
						frame = (long)floor(phs);
						float frac = phs - frame;
						if (frame != lframe) {
							if (frame <= nextBufferStartFrame) { // hit the end of data
								nCycleFrames = nControlChunkFrames = i;
								break;
							}
							else {
								chunkI = frame - chunkStartFrame;
								lRaw1 = chunkData[chunkI];
								if (frame <= nextBufferStartFrame + 1) {
									lRaw2 = nextBufferStartL;
								}
								else {
									lRaw2 = chunkData[chunkI - 1];
								}
							}
						}
						lSig[i] = interpolate(lRaw1, lRaw2, frac);
						phs += rpsi;
					}
				}
			}
//			lFilter.apply(lSig, nControlChunkFrames);
			for (unsigned int i = 0; i<nControlChunkFrames; i++, buffI ++) {
				buffL[buffI] += clAmp*lSig[i];
				buffR[buffI] += crAmp*lSig[i];
			}
			if ((controlFrame += nControlChunkFrames) >= framesPerControlCycle) {
				controlFrame = 0; // -= framesPerControlCycle ... if it's > we have a mess where control cycle is skipped. so line above, nControlChunkFrames = .... etc
			}
			nCycleFrames -= nControlChunkFrames;
			nOutputFrames += nControlChunkFrames;
		}

		/* out of data. process fx tail. this may not be relevant */
		if (((!pitchShifting) ? currentDataFrame + nOutputFrames : floor(phs)) >= getTotalCurrentSampleFrames() || nDataFramesAvailable <= 0) {
			nCycleFrames = nRequestedFrames - nOutputFrames;
			while (nCycleFrames > 0) {
				if (controlFrame == 0) {
					calculateControlsStereo(clAmp, crAmp);
				}
				nControlChunkFrames = (controlFrame + nCycleFrames > framesPerControlCycle) ? (framesPerControlCycle - controlFrame) : nCycleFrames;
				memset(lSig, 0, nControlChunkFrames * sizeof(float));
	//			lFilter.apply(lSig, nControlChunkFrames);
				for (unsigned int i = 0; i<nControlChunkFrames; i++, buffI ++) {
					buffL[buffI] += clAmp*lSig[i];
					buffR[buffI] += crAmp*lSig[i];
				}
				if ((controlFrame += nControlChunkFrames) >= framesPerControlCycle) {
					controlFrame = 0;
				}
				nCycleFrames -= nControlChunkFrames;
				nOutputFrames += nControlChunkFrames;
				float rpsi = directionFwd ? psi : -psi;
				phs += rpsi*nControlChunkFrames;
			}
		}
	}
	if (!pitchShifting) {
		nextDataFrame = currentDataFrame + (directionFwd? nOutputFrames : -nOutputFrames);
		lastFraction = 0;
	}
	else {
		nextDataFrame = floor(phs);
		lastFraction = phs - nextDataFrame;
	}
	return nOutputFrames;
}


inline void
KleinTrack::calculateControlsStereo(float &l, float &r)
{
	float p = pan;
	float g = outputGain;

	if (p < -1) p = -1; else if (p > 1) p = 1;
	if (g < 0) g = 0; else if (g > 1) g = 1;
	r = rAmp = (g)*(1 + p) / 2;
	l = lAmp = (g)*(1 - p) / 2;
}

/*
 * give an indication of how many sample frames until the next interesting thing happens
 * interesting things are:
 *  - loop ends
 *  - resynchronization opportunities
 *  - pretty much anything that will affect another track, or require being affected by another track
 *  - return 0 if we want to say nothing about what's going on, or nothing interesting ever happens
 */
long KleinTrack::boringFrames(const VstTimeInfo * const t, const long startOffset)
{
	return 0;
}

bool KleinTrack::isPlaying()
{
	return true;
}
