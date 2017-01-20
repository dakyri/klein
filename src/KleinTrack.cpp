#include "KleinTrack.h"

#include "aeffectx.h"

#include <unordered_map>
#include <algorithm>
#include <iostream>

#include "InputInfo.h"

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


unordered_map<string, PlayDirection> playDir{
	{ "fwd", PLAY_FWD },
	{ "back", PLAY_BAK },
	{ "bakfwd", PLAY_FWDBAK },
	{ "fwdbak", PLAY_BAKFWD }
};

unordered_map<string, InputInfo::ThruMode> inputProperties {
	{ "none", InputInfo::ThruMode::NO_THRU },
	{ "manual", InputInfo::ThruMode::MANUAL },
	{ "select", InputInfo::ThruMode::ON_SELECT }
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

string syncSrc4(SyncSource s) {
	auto p = find_if(syncSrc.begin(), syncSrc.end(), [s](pair<string, SyncSource> t) { return t.second == s; });
	if (p != syncSrc.end()) {
		return p->first;
	}
	return "default";
}

SyncUnit syncUnit4(string s) {
	auto p = syncUnit.find(s);
	if (p != syncUnit.end()) {
		return p->second;
	}
	return DFLT_UNIT;
}

string syncUnit4(SyncUnit s) {
	auto p = find_if(syncUnit.begin(), syncUnit.end(), [s](pair<string, SyncUnit> t) { return t.second == s; });
	if (p != syncUnit.end()) {
		return p->first;
	}
	return "default";
}

PlayDirection playDir4(string s) {
	auto p = playDir.find(s);
	if (p != playDir.end()) {
		return p->second;
	}
	return PLAY_FWD;
}

string playDir4(PlayDirection s) {
	auto p = find_if(playDir.begin(), playDir.end(), [s](pair<string, PlayDirection> t) { return t.second == s; });
	if (p != playDir.end()) {
		return p->first;
	}
	return "";
}


InputInfo::ThruMode
InputInfo::findMode(const string &s) {
	auto p = inputProperties.find(s);
	if (p != inputProperties.end()) {
		return p->second;
	}
	return InputInfo::ThruMode::NO_THRU;
}


string
InputInfo::findMode(const InputInfo::ThruMode s) {
	auto p = find_if(inputProperties.begin(), inputProperties.end(), [s](pair<string, InputInfo::ThruMode> t) { return t.second == s; });
	if (p != inputProperties.end()) {
		return p->first;
	}
	return "";
}


void writeToBuffer(const float gain, float * const inl, float * const outl, const int nFrames)
{
	for (int i = 0; i < nFrames; i++) {
		outl[i] = gain * inl[i];
	}
}

void writeToBufferPair(const float gain, float * const ins, float * const outl, float * const outr, const int nFrames)
{
	int j = 0;
	for (int i = 0; i < nFrames; i++) {
		outl[i] = gain * ins[j++];
		outr[i] = gain * ins[j++];
	}
}



KleinTrack::KleinTrack(int _trackId, int nLoops)
	: KleinTrack(_trackId, 0, 0, nLoops, SYNC_HOST, SYNC_LOOP)
{
}

KleinTrack::KleinTrack(int _trackId, int _inPortId, int _outPortId, int nLoops, SyncSource _syncSrc, SyncUnit _syncUnit)
	: id(_trackId)
	, inPortId(_inPortId)
	, outPortId(_outPortId)
	, selected(true)
	, syncSrc(_syncSrc)
	, syncUnit(_syncUnit)
	, playDirection(PLAY_FWD)
	, currentDirectionFwd(true)
	, psi(1)
	, lAmp(1)
	, rAmp(1)
	, tune(0)
	, lastFraction(0)
	, nextDataFrame(0)
	, loopOutBuf(nullptr)
	, loopRawBuf(nullptr)
	, recordBuffer(nullptr)
	, recordBufferLen(Bufferator::FRAMES_PER_PAGE)
	, nFramesRecorded(0)
{
	leftCycleBuffer = new float[framesPerControlCycle];
	rightCycleBuffer = new float[framesPerControlCycle];

	setNLoops(nLoops);
}


KleinTrack::~KleinTrack()
{
	if (loopOutBuf != nullptr) delete[] loopOutBuf;
	if (loopRawBuf != nullptr) delete[] loopRawBuf;
	if (leftCycleBuffer != nullptr) delete[] leftCycleBuffer;
	if (rightCycleBuffer != nullptr) delete[] rightCycleBuffer;
}

void
KleinTrack::setInputGain(int v) {
	inputGain = v;
}
float KleinTrack::getInputGain()
{
	return inputGain;
}

void
KleinTrack::setInputPan(int v) {
	inputPan = v;
}
float KleinTrack::getInputPan()
{
	return inputPan;
}

void
KleinTrack::setOutputGain(int v) {
	outputGain = v;
}
float KleinTrack::getOutputGain()
{
	return outputGain;
}


void
KleinTrack::setPan(int v) {
	pan = v;
}
float KleinTrack::getPan()
{
	return pan;
}

void
KleinTrack::setFeedback(int v) {
	feedback = v;
}

float KleinTrack::getFeedback()
{
	return feedback;
}

float KleinTrack::getSecondaryFeedback()
{
	return 0.0f;
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

/**
 * ??? XXXX FIXME we should also check that we are not in a record mode. 
 */
void
KleinTrack::setInPort(const int port) const {
	inPortId = port;
}

void
KleinTrack::setOutPort(const int port) const {
	outPortId = port;
}

int KleinTrack::getInPort() const
{
	return 0;
}

int KleinTrack::getOutPort() const
{
	return 0;
}

void KleinTrack::setNLoops(const int nLoops)
{
	loops.clear();
	for (int i = 0; i < nLoops; i++) {
		loops.push_back(SampleLoop());
	}
	currentSampleLoop = loops.end();
}


void KleinTrack::setSyncSrc(const SyncSource ss) {
	syncSrc = ss;
}

void KleinTrack::setSyncUnit(const SyncUnit su) {
	syncUnit = su;
}

void KleinTrack::setPlayDirection(const PlayDirection p)
{
	playDirection = p;
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
#if KLEIN_DEBUG >= 10
	dbf << "processAdding " << id << " off " << startOffset << ", nFrames " << nFrames << endl;
#endif
	const int ii = inPortId * 2;
	const int oi = outPortId * 2;
	float * const inl = inputs[ii] + startOffset;
	float * const inr = inputs[ii + 1] + startOffset;
	float * const outl = outputs[oi] + startOffset;
	float * const outr = outputs[oi + 1] + startOffset;
	float lvu = 0;
	long nFramesOut = 0;

	shared_ptr<SampleInfo> csf = nullptr;
	if (currentSampleLoop != loops.end()) {
		csf = currentSampleLoop->sampleInfo;
	}
	bool playLoop = false;
	switch (trackMode) {
		case TRAK_PLAY: 
		case TRAK_DUB: {
			playLoop = true;
			break;
		}

		case TRAK_STOP:
		case TRAK_PAUSE: {
			return 0;
		}

		case TRAK_REC: {
			if (!csf) {
				currentSampleLoop = allocateSampleLoop();
				csf = currentSampleLoop->sampleInfo;
			}
			break;
		}
	}	

	nFramesOut = 0;
	if (playLoop) {
		if (!csf) {
			return 0;
		}

		const int ll = loopLengthFrames < 2 ? 2 : loopLengthFrames;
		const int sf = loopStartFrame;
		int buffInd = 0;
		//		Log.d("player", String.format("pad %d to play %d %d", id, nFrames, nOutChannels));
		while (nFramesOut < nFrames) {
#if KLEIN_DEBUG >= 12
			dbf << ">> processAdding.3 " << id << " off " << startOffset << ", nFrames " << nFrames << endl;
#endif
			const int nFramesRemaining = nFrames - nFramesOut;
			const int nIterFrames = currentDirectionFwd ?
				((nFramesRemaining > ll - currentLoopFrame) ? (ll - currentLoopFrame) : nFramesRemaining) :
				((nFramesRemaining > currentLoopFrame) ? currentLoopFrame : nFramesRemaining);
			//			Log.d("player", String.format("pad %d loop frame %d dir %d to make %d %d %d %d tune is %g", id, currentLoopFrame, currentDirection, nFrames, nFramesOut, nFramesRemaining, nIterFrames, state.tune));
			if (nIterFrames > 0) {
				const int currentDataFrame = currentLoopFrame + sf;
				// wtf we should get the one buffer hey
				const int nIterOutFrames = playChunk(csf, loopOutBuf + buffInd, loopRawBuf + buffInd, nIterFrames, currentDataFrame, currentDirectionFwd);
				buffInd += nOutChannels*nIterOutFrames; // >>>>>>>>>>>>>>>><<<<<<<<<<<<< >>>>>> wtf buffind here is for interleaved
				currentLoopFrame = nextDataFrame - sf;
				nFramesOut += nIterOutFrames;
				//				Log.d("player", String.format("pad %d played %d %d requested %d loop %d data %d end %d", id, nIterOutFrames, nFramesOut, nIterFrames, currentLoopFrame, getNextDataFrame(cPadPointer), ll));
			} else {
				if (!isPlaying()) {
					break;
				}
			}
#if KLEIN_DEBUG >= 12
			dbf << "processAdding 2 " << id << " nIterFrames " << nIterFrames << ", nFramesOut " << nFramesOut << endl;
#endif

			if (currentLoopFrame >= ll) {
				if (currentDirectionFwd) {
					loopEnd(csf);
				}
				else {
					currentLoopFrame = ll;
				}
			} else if (currentLoopFrame <= 0) {
				if (currentDirectionFwd) {
					currentLoopFrame = 0;
				}
				else {
					loopEnd(csf);
				}
			}
		}
	}

//	if (selected) {
//		addToBuffer(inputGain, inl, outl, nFrames);
//		addToBuffer(inputGain, inr, outr, nFrames);
//	}

	if (trackMode == TRAK_REC) {
		addToRecordBuffer(csf, inputGain, inl, inr, nFrames);
	}
	else if (trackMode == TRAK_DUB) {
		addToRecordBuffer(csf, inputGain, inl, inr, nFrames, false);
		addToRecordBuffer(csf, feedback, loopRawBuf, nFrames);
		writeToBufferPair(feedback, loopOutBuf, outl, outr, nFrames);
	}
	else if (trackMode == TRAK_INSERT) {
		//add (gain * input) into currentLoop buffer
	}
	else if (trackMode == TRAK_DELETE) {
		//add(gain * input) into currentLoop buffer
	}
	else if (trackMode == TRAK_PLAY) {
		writeToBufferPair(feedback, loopOutBuf, outl, outr, nFrames);
		if (feedback < 1.0) {
			addToRecordBuffer(csf, feedback, loopRawBuf, nFrames);
		}
	}

	vu = lvu;
	return nFrames;
}


void
KleinTrack::loopEnd(const shared_ptr<SampleInfo>& csf) {
	if (playDirection == PLAY_FWDBAK || playDirection == PLAY_BAKFWD) {
		currentDirectionFwd = !currentDirectionFwd;
	}
	if (currentDirectionFwd) {
		int lsb = Bufferator::page4Frame((int)loopStartFrame);
		csf->requestPage(lsb);
		csf->requestPage(lsb + 1);
		currentLoopFrame = 0;
	}
	else {
		int ll1 = (int)(loopLengthFrames- 1);
		int lsb = Bufferator::page4Frame((int)(loopStartFrame + ll1));
		csf->requestPage(lsb);
		csf->requestPage(lsb - 1);
		currentLoopFrame = ll1;
	}

//	reloopCPad(cPadPointer);
	// just resets the lfos in C
	doSync();
}

void KleinTrack::doSync() {
	/*
	int i = 0;
	PadSample p;
	while ((p = ((PadSample)manager.getSamplePlayer(i++))) != null) {
		if (p != this && p.state.syncMasterId == id && (p.isPlaying() || p.isPaused())) {
			if (p.isPaused()) {
				manager.startSamplePlayer(p, false);
				if (listener != null) listener.playStarted(p);
			}
			else {
				manager.startSamplePlayer(p, true);
			}
		}
	}*/
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

int KleinTrack::addToRecordBuffer(const shared_ptr<SampleInfo> & csf, const float gain, float * const inl, float * const inr, const int nFrames, bool inc)
{
	int nr = 0;
	int nf = nFrames;
	float *b = recordBuffer + 2*nFramesRecorded;
	while (nf > 0) {
		nf = recordBufferLen - nFramesRecorded;
		if (nf > nFrames) {
			nf = nFrames;
		}
		for (int i = 0; i < nf; i++) {
			*b++ += gain * inl[i];
			*b++ += gain * inr[i];
		}
		nr += nf;
		if (inc) { // if we're incrementing the buffer pointer we're done overdubbing so we check to see if we dump this
			nFramesRecorded += nf;
			if (nFramesRecorded >= recordBufferLen) {
				finalizeRecord(csf);
			}
		}
	}
	return nr;
}

int KleinTrack::addToRecordBuffer(const shared_ptr<SampleInfo> & csf, const float gain, float * const ins, const int nFrames, bool inc)
{
	int nr = 0;
	int nf = nFrames;
	float *b = recordBuffer + 2 * nFramesRecorded;
	while (nf > 0) {
		nf = recordBufferLen - nFramesRecorded;
		if (nf > nFrames) {
			nf = nFrames;
		}
		for (int i = 0; i < 2*nf; i+=2) {
			*b++ += gain * ins[i];
			*b++ += gain * ins[i+1];
		}
		nr += nf;
		if (inc) { // if we're incrementing the buffer pointer we're done overdubbing so we check to see if we dump this
			nFramesRecorded += nf;
			if (nFramesRecorded >= recordBufferLen) {
				finalizeRecord(csf);
			}
		}
	}
	return nr;

}

void KleinTrack::startRecordBuffer(const shared_ptr<SampleInfo> & csf, const int startFrame)
{
	if (nFramesRecorded != 0) {
		finalizeRecord(csf);
	}
	recordStartFrame = startFrame;
	nFramesRecorded = 0;
	memset(recordBuffer, 0, recordBufferLen * sizeof(float) * nOutChannels);
}

void KleinTrack::finalizeRecord(const shared_ptr<SampleInfo> & csf)
{
	csf->addChunk(recordStartFrame, nFramesRecorded, recordBuffer);
	nFramesRecorded = 0;
}

vector<SampleLoop>::iterator KleinTrack::allocateSampleLoop()
{
	if (currentSampleLoop == loops.end()) {
		currentSampleLoop == loops.begin();
	}
	currentSampleLoop->initLoop();
	return currentSampleLoop;
}

bool SampleLoop::initLoop()
{
	sampleInfo = Bufferator::doCreate(KleinTrack::getUniqueFilename(), SampleFile::WAVE_TYPE, 2, 2, 44100, false);
	return true;
}

bool SampleLoop::reset()
{
	sampleInfo.reset();
	layers.clear();
	return false;
}


/**
 * process a given buffer of output. pulled from android CPadSample.cpp
 * processes a single chunk worth at most, in a single direction, not looping
 *
 * can probably be further simplified.
 * ??? need to change the simple tune stuff to a lenght preserving pitch shift.
 * ??? possibly also a pitch preserving time stretch
 *
 * @param csf shared_ptr to the sample info structure for this chunk
 * @param outBuff output buffer, stereo interleaved
 * @param rawBuf
 * @param nRequestedFrames optimal number of frames to play.
 * @param currentDataFrame
 * @param directionFwd true for forwards, else backwards
 */
int
KleinTrack::playChunk(
	const shared_ptr<SampleInfo>& csf, float *const outBuf, float *const rawBuf, const int nRequestedFrames, const int currentDataFrame, const bool directionFwd)
{
#if KLEIN_DEBUG >= 10
	dbf << "playChunk " << id << " off " << nRequestedFrames << ", nFrames " << currentDataFrame << ", dir " << directionFwd << endl;
#endif
	int chunkStartFrame = 0;
	int chunkNFrames = 0;
	float* chunkData = nullptr;
	SampleChunkInfo *sci = csf->getChunk4Frame(currentDataFrame);
	float nextBufferStartL = 0;
	float nextBufferStartR = 0;
	int cpageid = Bufferator::page4Frame(currentDataFrame);
	if (sci != nullptr) {
		chunkNFrames = sci->nFrames;
		chunkData = sci->data;
		chunkStartFrame = sci->startFrame;
		if (tune != 0) { // setup buffer endpoints if we're interpolating TODO or we are tuning it with lfo!!!!
			const int ll = loopLengthFrames<2 ? 2 : loopLengthFrames;
			const int sf = loopStartFrame;
			if (currentDirectionFwd) {
				SampleChunkInfo *scnxt = nullptr;
				if (sci->nextChunkStartFrame() >= sf + ll) { // next chunk is start of loop
					scnxt = csf->getChunk4Frame(sf);
				}
				else {
					scnxt = csf->getChunk4Frame(sci->nextChunkStartFrame());
				}
				if (scnxt != nullptr) {
					nextBufferStartL = scnxt->data[0];
					nextBufferStartR = scnxt->data[1];
				}
			}
			else { // backwards .
				SampleChunkInfo *scnxt = nullptr;
				if (sci->prevChunkEndFrame() < sf) { // next chunk is start of loop
					scnxt = csf->getChunk4Frame(sf);
				}
				else {
					scnxt = csf->getChunk4Frame(sci->prevChunkEndFrame());
				}
				if (scnxt != nullptr) {
					nextBufferStartL = scnxt->data[scnxt->nFrames - 2];
					nextBufferStartR = scnxt->data[scnxt->nFrames - 1];
				}
			}
		}
#ifdef KLEIN_DEBUG >= 11
		dbf << "player got " << currentDataFrame << " chunk  " << " len " << chunkNFrames << "@  " << chunkStartFrame
			<< " data[0]  " << chunkData[0] << " path  " << csf->getPath() << endl;
#endif
	} else {
#ifdef KLEIN_DEBUG >= 11
		cerr << "player failed to find " << currentDataFrame << " chunk " << cpageid << " path " << csf->getPath() << endl;
#endif
	}

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
			for (unsigned int i = 0; i<nControlChunkFrames; i++, buffI += 2) {
				rawBuf[buffI] = lSig[i];
				rawBuf[buffI + 1] = rSig[i];
				outBuf[buffI] = clAmp*lSig[i];
				outBuf[buffI+1] = crAmp*rSig[i];
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
				for (unsigned int i = 0; i<nControlChunkFrames; i++, buffI +=2) {
					rawBuf[buffI] = lSig[i];
					rawBuf[buffI + 1] = rSig[i];
					outBuf[buffI] = clAmp*lSig[i];
					outBuf[buffI + 1] = crAmp*rSig[i];
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
			for (unsigned int i = 0; i<nControlChunkFrames; i++, buffI +=2) {
				rawBuf[buffI] = lSig[i];
				rawBuf[buffI + 1] = rSig[i];
				outBuf[buffI] = clAmp*lSig[i];
				outBuf[buffI + 1] = crAmp*lSig[i];
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
				for (unsigned int i = 0; i<nControlChunkFrames; i++, buffI += 2) {
					rawBuf[buffI] = lSig[i];
					rawBuf[buffI + 1] = rSig[i];
					outBuf[buffI] = clAmp*lSig[i];
					outBuf[buffI + 1] = crAmp*lSig[i];
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
 * this is called so that we will have the longest possible chunk of buffer to be filled that doesn't loop, change sample chunk
 * or affect (or be affected by) any other loops synchronization
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

bool KleinTrack::isRecording()
{
	return false;
}

SyncSource KleinTrack::getSyncSrc()
{
	return SyncSource();
}

SyncUnit KleinTrack::getSyncUnit()
{
	return SyncUnit();
}

PlayDirection KleinTrack::getPlayDirection()
{
	return PlayDirection();
}

TrackMode KleinTrack::getMode()
{
	return TrackMode();
}

void KleinTrack::allocateBuffers(long blocksize)
{
	if (loopOutBuf != nullptr) delete[] loopOutBuf;
	loopOutBuf = new float[nOutChannels * blocksize];
	if (loopRawBuf != nullptr) delete[] loopRawBuf;
	loopRawBuf = new float[nOutChannels * blocksize];
	if (recordBuffer != nullptr) delete[] recordBuffer;
	recordBufferLen = Bufferator::FRAMES_PER_PAGE;
	if (recordBufferLen < blocksize) {
		recordBufferLen = blocksize;
	}
	recordBuffer = new float[nOutChannels * recordBufferLen];
}

void KleinTrack::pushCommand(CommandStackItem & c)
{
	commandsLock.lock();
	commands.push_back(c);
	commandsLock.unlock();
}

void KleinTrack::clearCommands()
{
	commandsLock.lock();
	commands.clear();
	commandsLock.unlock();
} 

string KleinTrack::getUniqueFilename()
{
	return string();
}

bool KleinTrack::play()
{
	return false;
}

bool KleinTrack::pause()
{
	return false;
}

bool KleinTrack::doLoop(int i)
{
	return false;
}

bool KleinTrack::doMute()
{
	return false;
}

bool KleinTrack::overdub()
{
	return false;
}

bool KleinTrack::record()
{
	return false;
}
