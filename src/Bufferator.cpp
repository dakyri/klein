#include <iostream>

#include "Bufferator.h"

Bufferator *Bufferator::instance = nullptr;
/*
static class SampleGfxInfo
{
	SampleGfxInfo(int length, float[][] dataMin, float[][] dataMax) {
		this.length = length;
		this.dataMin = dataMin;
		this.dataMax = dataMax;
	}
	SampleGfxInfo() {
		this(0, nullptr, nullptr);
	}
	float[][] dataMin;
	float[][] dataMax;
	int length;
}
*/

/*
class BufferatorException extends Exception {
	final int severity;
	private final string path;

	BufferatorException(string msg, int severity) {
		this(msg, severity, "");
	}

	BufferatorException(string msg, int severity, string path) {
		super(msg);
		this.severity = severity;
		this.path = path;
	}

	string getPath() {
		return path;
	}
}
*/

SampleInfo::SampleInfo()
{
	path = "";
	id = -1;
	nTotalFrames = 0;
	nChannels = 0;
//	gfxCache = nullptr;
	chunkChannelCount = 0;
}

bool SampleInfo::setSampleData(string _path, int _id)
{
	if (path == "") {
		return false;
	}
	path = _path;
	id = _id;
	nTotalFrames = 0;
	status_t err;
	if ((err = audioFile.setTo(path, O_RDWR)) != ERR_OK) {
		if (err == EEXIST) {
			setError(path + " not found", Bufferator::ErrorLevel::FNF_ERROR_EVENT);
		}
		else {
			setError("Catastrophe! Some kind of IO exception! " + err);
		}
		return false;

	}
	nChannels = (short)audioFile.nChannels;
	nTotalFrames = (int)audioFile.nFrames;
	cerr << "set to " << path << " " << nChannels << " chan " << nTotalFrames << " frame" << endl;

	requestChunk(0, 2); // request start chunks from the interface request list
	requestChunk(1, 2);
	//				for (int i=0; i<sampleChunk.length; i++) {
	//					sampleChunk[i] = readChunk(i);
	//				}
	return true;
}

/*
Observable<SampleGfxInfo> SampleInfo::getMinMax(final int npoints) {
	return Observable.create(new Observable.OnSubscribe<SampleGfxInfo>() {
		@Override
			void call(Subscriber< ? super SampleGfxInfo> subscriber) {
			if (path == null || path.equals("") || nTotalFrames == 0 || nChannels == 0 || npoints == 0) {
				Log.d("Bufferator::minmax()", "null data checked " + npoints);
				subscriber.onCompleted();
				return;
			}
			synchronized(self) { // only one request per SampleInfo simultaneous: this ensures cache gets used
				if (gfxCache != null && gfxCache.length >= npoints) {
					Log.d("Bufferator::minmax()", "found graphic cache " + npoints);
					subscriber.onNext(gfxCache);
					subscriber.onCompleted();
					return;
				}
				Log.d("Bufferator::minmax()", string.format("in thread to make collect %d point of %d paths %s", npoints, nTotalFrames, path));
				int tsp = 0;
				int nFramesPerPx = (int)Math.ceil((float)nTotalFrames / npoints);
				int iw = 0;
				float[][] pMin = new float[nChannels][npoints];
				float[][] pMax = new float[nChannels][npoints];

				AudioFile cmmAudioFile;
				//					gfxCache = mokMinMax(nChannels, npoints);
				try {
					cmmAudioFile = getAudioFile(path);
					int framesRead = 0;
					float[] buffer = new float[nFramesPerPx * nChannels];

					while (tsp < nTotalFrames * nChannels && iw < npoints) {
						for (int k = 0; k < nChannels; k++) {
							pMin[k][iw] = pMax[k][iw] = 0;
						}
						framesRead = cmmAudioFile.readFrames(buffer, nFramesPerPx);

						//							Log.d("Bufferator::minmax()", string.format("in thread to read %d %d", framesRead, nFramesPerPx));
						tsp = 0;
						for (int j = 0; j < nFramesPerPx && j < framesRead; j++) {
							for (int k = 0; k < nChannels; k++) {
								float s = buffer[tsp + k];
								if (s < pMin[k][iw]) pMin[k][iw] = s;
								else if (s > pMax[k][iw]) pMax[k][iw] = s;
							}
							tsp += nChannels;
						}

						iw++;
					}
					cmmAudioFile.close();
					Log.d("Bufferator::minmax()", string.format("got %d %d", pMin[0].length, pMax[0].length));
					subscriber.onNext(gfxCache = new SampleGfxInfo(npoints, pMin, pMax));
					subscriber.onCompleted();
				}
				catch (IOException e) {
					subscriber.onError(
						new Exception("Catastrophe! Some kind of IO exception, building image for " + path + ", " + e.getMessage()));
				}
				catch (AudioFileException e) {
					subscriber.onError(
						new Exception("How Bizzare! An audio file exception, while building image for " + path + ", " + e.getMessage()));
				}
				catch (Exception e) {
					subscriber.onError(
						new Exception("Completely suprising error " + e.getClass().getCanonicalName() + ", " + e.getMessage()));
				}
			}
		}
	})
		.subscribeOn(Schedulers.io())
		.observeOn(AndroidSchedulers.mainThread());
}
*/

/**
* reads and allocates a new chunk info buffer
*
* @param chunk
* @return the new buffer info
* @throws IOException
* @throws AudioFileException
*/
SampleChunkInfo * SampleInfo::readChunk(int chunk) 
{
	if (!audioFile()) {
		return nullptr;
	}
	float * buffer = new float[Bufferator::FRAMES_PER_CHUNK*nChannels];
	int csf = Bufferator::chunkStartFrame(chunk);
	audioFile.seekToFrame(csf);
	int framesRead = audioFile.readFrames(buffer, Bufferator::FRAMES_PER_CHUNK);
	SampleChunkInfo * sci = new SampleChunkInfo(csf, framesRead, chunk, buffer);
	return sci;

}

/**
* @see #getChunk(int)
* @param currentDataFrame address of the needed chunk
* @return the chunk, or null if not found
*/
SampleChunkInfo *SampleInfo::getChunkFor(int currentDataFrame) {
	return getChunk(Bufferator::chunk4Frame(currentDataFrame));
}

/**
*  gets a reference to the buffer for the requested chunk ... at the moment, assume we are being called by the
*  audio thread, which is the only place at the moment where it is called from. if not found, we will try to
*  request it
* @param chunk the index of the needed chunk
* @return the chunk, or null if not found
*/
SampleChunkInfo *SampleInfo::getChunk(int chunk)
{
	if (chunk < 0 || chunk >= sampleChunk.size())
		return nullptr;
	SampleChunkInfo *sci = sampleChunk[chunk];
	if (sci == nullptr || sci->data == nullptr) {
		requestChunk(chunk, 1);
		return nullptr;
	}
	else {
		// TODO check overhead of this
		sci->timeStamp = System.currentTimeMillis();
	}
	return sci;

}

/**
* @param requestedChunk which chunk we want
* @param from which thread is requesting ... keep separate lists to avoid synch issues, 1 for audio list, else interface list
* @return false if the chunk is valid, in range, but can't be requested; true otherwise
*/
bool SampleInfo::requestChunk(int requestedChunk, int from)
{
	int[] reqChunk;
	if (sampleChunk == null || requestedChunk > sampleChunk.length) {
		return true;
	}
	if (from == 1) reqChunk = reqChunkAuThread;
	else reqChunk = reqChunkIfThread;

	for (int i = 0; i<reqChunk.length; i++) {
		if (reqChunk[i] == requestedChunk) {
			return true;
		}
		else if (reqChunk[i] == -1) {
			reqChunk[i] = requestedChunk;
			return true;
		}
	}
	return false;
}

bool SampleInfo::updateLoadedChunks()
{
	bool b1 = processChunkRequests(reqChunkAuThread);
	bool b2 = processChunkRequests(reqChunkIfThread);

	return b1 && b2;
}

bool SampleInfo::processChunkRequests(vector<int> &reqList) 
{
	for (int ch: reqList) {

		if (ch >= 0 && ch <sampleChunk.size()) {
			if (sampleChunk[ch] == nullptr || sampleChunk[ch]->data == nullptr) {
				cerr << "process " << reqList.size() << " requests " << ch << " " << path << endl; 
				SampleChunkInfo *sci = readChunk(ch);
				if (sci != nullptr) {
					sampleChunk[ch] = sci;
					chunkChannelCount += nChannels;
				}
			}
		}
		reqList[i] = -1;
	}
	return true;
}

int SampleInfo::trimAllocatedChunks(int fsi)
{
	if (nTotalFrames == 0 || sampleChunk == null || sampleChunk.length == 0) {
		return 0;
	}
	if (fsi <= 0) {
		return 0;
	}
	if (chunkChannelCount <= refCount*nChannels*minBufferPerSample) {
		return 0;
	}
	// find the fsi oldest chunks
	SampleChunkInfo * togo = new SampleChunkInfo[fsi];
	for (int i = 0; i<fsi; i++) {
		togo[i] = null;
	}

	for (auto sci : sampleChunk) {
		if (sci != null) {
			if (!isRequiredChunk(sci->id)) {
				checkAddOldest(sci, togo);
			}
		}
	}
	int fsid = 0;
	for (int i = 0; i<togo.length; i++) {
		SampleChunkInfo sci = togo[i];
		if (sci != null) {
			// that data should be safe to clear now, but just in case this chunk's being accessed in another thread we should maybe leave it
			// till the system decides it has no references ...
			sci.data = null;
			sampleChunk[sci.id] = null;
			togo[i] = null;
			fsid++;
			chunkChannelCount -= nChannels;
			cerr << "just freed " << sci.id << " in " << path << ", count is  " << chunkChannelCount << endl;
		}
	}
	return fsid;
}

void SampleInfo::checkAddOldest(SampleChunkInfo *sci, vector<SampleChunkInfo*> &  togo)
{
	auto it = togo.begin();
	auto fnd = togo.end();
	while (it != togo.end()) {
		auto scit = *it;
		if (scit == nullptr) {
			fnd = it;
			break;
		}
		else if (sci->timeStamp < scit->timeStamp) {
			fnd = it;
			break;
		}
		else {
		}
		++it;
	}
	if (fnd != togo.end()) {
		for (int i = togo.length - 1; i>fnd; i--) {
			togo[i] = togo[i - 1];
		}
		togo[fnd] = sci;
	}
}

SampleInfo::~SampleInfo()
{
	nTotalFrames = 0;
	// audio file is closed automatically
	for (auto sci: sampleChunk) {
		if (sci != nullptr) {
			delete sci;
		}
	}
}

void SampleInfo::clearRequiredChunks()
{
	nRequiredChunks = 0;
}

void SampleInfo::addRequiredChunk(int reqdChunk)
{
	if (isRequiredChunk(reqdChunk)) {
		return;
	}
	if (nRequiredChunks < requiredChunks.size() - 1) {
		requiredChunks[nRequiredChunks++] = reqdChunk;
	}
}

bool SampleInfo::isRequiredChunk(int c)
{
	for (int ch : requiredChunks) {
		if (ch == c) return true;
	}
	return false;
}

/*
Observable<Path[]> SampleInfo::getSamplePaths(final int w, final int h) {
	return getMinMax(w).map(new Func1<SampleGfxInfo, Path[]>() {
		@Override
			Path[] call(SampleGfxInfo gfc) {
			//					Log.d("padsample", "got minimaxa "+gfc.length+", "+gfc.dataMax[0].length+" for "+w+", "+h);
			Path[] drawPath = new Path[nChannels];

			float zp[] = new float[nChannels];
			float ch = h / nChannels;
			for (int k = 0; k<nChannels; k++) {
				zp[k] = (ch*k + (ch / 2));
				drawPath[k] = new Path();
				drawPath[k].moveTo(0, zp[k]);
				for (int j = 0; j<gfc.dataMax[k].length; j++) {
					drawPath[k].lineTo(j, zp[k] + (ch*gfc.dataMax[k][j] / 2));
				}
				drawPath[k].lineTo(gfc.dataMin[k].length, zp[k]);
				for (int j = gfc.dataMin[k].length - 1; j >= 0; j--) {
					drawPath[k].lineTo(j, zp[k] + (ch*gfc.dataMin[k][j] / 2));
				}
				drawPath[k].close();
			}
			return drawPath;
		}
	});
}
*/

float SampleInfo::getFrameCount() {
	return nTotalFrames;
}


Bufferator *Bufferator::getInstance(Bufferator::Client * spg) {
	if (instance != nullptr) return instance;
	return (instance = new Bufferator(spg));
}

Bufferator::Bufferator(Bufferator::Client * spm)
{
	sampleManager = spm;
	checkAvailableHeap();
}

Bufferator::~Bufferator() {

}
/*
Subscription monitorState(Action1<BufferatorException> action) {
	return stateSubject.observeOn(AndroidSchedulers.mainThread()).subscribe(action);
}
*/
/*
Subscription monitorGfx(Action1<SampleInfo> observer) {
	return gfxSubject.observeOn(AndroidSchedulers.mainThread()).subscribe(observer);
}
*/
bool Bufferator::fixMemoryPanic()
{
	int totalChunkChannel = 0;
	cacheLock.lock();
	for (auto wpip: cache) {
		auto spip = wpip.lock();
		if (spip) {
			totalChunkChannel += spip->chunkChannelCount;
		}
	}
	cacheLock.unlock();
	fairChunkBufferChannelCount = (int)(bufferRamLimit * totalChunkChannel);
	cerr << "memory panic with " << totalChunkChannel << " chunk/channels, bound now set to " << fairChunkBufferChannelCount << " initially " << initialChunkBufferChannelCount << endl;
	return true;
}

void Bufferator::checkAvailableHeap()
{
	if (sampleManager != nullptr) {
		ActivityManager am = (ActivityManager)sampleManager.getContext().getSystemService(Context.ACTIVITY_SERVICE);
		ActivityManager.MemoryInfo aMemInf = new ActivityManager.MemoryInfo();
		am.getMemoryInfo(aMemInf);
		Runtime rt = Runtime.getRuntime();

	}

	Debug.MemoryInfo dMemInf = new Debug.MemoryInfo();
	Debug.getMemoryInfo(dMemInf);

	maxMemory = rt.maxMemory() / (float)(1024 * 1024);
	threshMemory = aMemInf.threshold / (float)(1024 * 1024);
	availMemory = aMemInf.availMem / (float)(1024 * 1024);
	totalPssMemory = dMemInf.getTotalPss() / (float)1024;

	fairBufferMemory = ((maxMemory - totalPssMemory)*bufferRamLimit);
	int chunkBufferChannelBytes = FRAMES_PER_CHUNK * 4;
	fairChunkBufferChannelCount = (((int)fairBufferMemory*(1024 * 1024))) / chunkBufferChannelBytes;
	initialChunkBufferChannelCount = fairChunkBufferChannelCount;

	cerr << "thresh %fMB avail %fMB max %fMB totalPss %fMB -> %d channel-chunks of size %d (%fMB allowed total)", threshMemory, availMemory, maxMemory, totalPssMemory, fairChunkBufferChannelCount, chunkBufferChannelBytes, fairBufferMemory));

	// S2 .... thresh 64.000000MB avail 256.933594MB max 48.000000MB class 48 large class 128
	// S2 with large heap: thresh 64.000000MB avail 424.964844MB max 128.000000MB class 48 large class 128

}

bool Bufferator::start() {
	runnerThread = thread(&runner, this);
}

bool Bufferator::runner()
{
	isRunning = true;

//	setPriority(Thread.NORM_PRIORITY);
	while (isRunning) {
		vector<shared_ptr<SampleInfo>> cacheImage;
		cacheLock.lock();
		auto cachei = cache.begin();
		while (cachei != cache.end()) {
			auto wpip = *cachei;
			auto spip = wpip.lock();
			if (spip) {
				cacheImage.push_back(spip);
				++cachei;
			}
			else {
				cachei = cache.erase(cachei);
			}
		}
		cacheLock.unlock();
		for (auto spip: cacheImage) {
			if (!spip->updateLoadedChunks()) {
				dispatchMaf(spip->lastErrorMessage, spip->lastErrorLevel);
			}
		}

		int totalChunkChannel = 0;
		int nSampleInfoRefs = 0;
		for (auto spip : cacheImage) {
			totalChunkChannel += spip->chunkChannelCount;
			nSampleInfoRefs += spip.use_count;
		}
		//						Log.d("bufferateor", string.format("%d %d about to trim", totalChunkChannel, fairChunkBufferChannelCount));
		if (totalChunkChannel > fairChunkBufferChannelCount) { // lets trim the fat!
			int totalChunksSaved = 0;
			// make a list of things we really should keep
			for (auto spip : cacheImage) {
				spip->clearRequiredChunks();
			}
			/*
			for (int i = 0; i< sampleManager.countSamplePlayers(); i++) {
				SamplePlayer ps = sampleManager.getSamplePlayer(i);
				if (ps != nullptr) {
					SampleInfo si = ps.getSampleInfo();
					if (si != nullptr) {
						si.addRequiredChunk(chunk4Frame((int)ps.getRegionStart()));
					}
				}
			}
			*/
			// first minimize stuff that is inactive
			//							Log.d("bufferateor", string.format("checking active players"));
			/*
			for (int i = 0; i< sampleManager.countSamplePlayers(); i++) {
				SamplePlayer ps = sampleManager.getSamplePlayer(i);
				if (!ps.isPlaying()) {
					SampleInfo si = ps.getSampleInfo();
					if (si != nullptr) {
						bool isInActiveUse = false;
						for (int ix = i + 1; ix< sampleManager.countSamplePlayers(); ix++) {
							SamplePlayer pps = sampleManager.getSamplePlayer(ix);
							if (pps != ps && si == pps.getSampleInfo()) {
								if (pps.isPlaying()) {
									isInActiveUse = true;
									break;
								}
							}
						}
						if (!isInActiveUse) {
							totalChunksSaved += si.trimAllocatedChunks(totalChunkChannel - totalChunksSaved - fairChunkBufferChannelCount);
						}
					}
					if (totalChunkChannel - totalChunksSaved <= fairChunkBufferChannelCount) {
						break;
					}
				}
			}*/
			//							Log.d("bufferateor", string.format("checking chunks to trim %d %d %d", totalChunkChannel, totalChunksSaved, fairChunkBufferChannelCount));
			if (totalChunkChannel - totalChunksSaved > fairChunkBufferChannelCount) {
				int fatPerInfoRef = ((totalChunkChannel - totalChunksSaved) - fairChunkBufferChannelCount + nSampleInfoRefs) / nSampleInfoRefs;
				for (auto spip : cacheImage) {
					int fsi = (fatPerInfoRef*spip->nChannels) / spip.use_count;
					int ftpsi = spip->trimAllocatedChunks(fsi);
					totalChunksSaved += ftpsi;
					if (totalChunkChannel - totalChunksSaved <= fairChunkBufferChannelCount) {
						break;
					}
				}
			}
			else { // that's a relief!!!

			}
		}
		// TODO should be a wait on something really
		std::this_thread::sleep_for(std::chrono::milliseconds(0));
	}
	return true;
}



/**
* adds this path to the cache, will have already checked that 'path' is not already added. this will be called from the
* interface thread only. cache is read by the buffer runner only. the sample info stuff is linked separately by audio
* thread and should be thread safe
* @param path
* @param mxi
* @return
*/
shared_ptr<SampleInfo> Bufferator:: allocateInfoBlock(string path, int mxi)
{
	auto p = make_shared<SampleInfo>(new SampleInfo());
	if (!p->setSampleData(path, mxi)) {
		return nullptr;
	}
	int i = 0;
	cacheLock.lock();
	cache.push_back(p);
	cacheLock.lock();
	return p;
}

shared_ptr<SampleInfo> Bufferator::load(string path)
{
	if (path == "") {
		return nullptr;
	}
	int mxi = 1;
	shared_ptr<SampleInfo> fnd = nullptr;
	for (auto wci: cache) {
		auto sci = wci.lock();
		if (sci) {
			if (sci->path == path) {
				fnd = sci;
				break;
			}
			if (sci->id > mxi) {
				mxi = sci->id + 1;
			}
		}
	}
	if (fnd == nullptr) {
		cerr << "load, adding new " << path << endl;
		fnd = allocateInfoBlock(path, mxi);
	}
	if (fnd != nullptr) {
		cerr << "find " << fnd->path << " id " << fnd->id << " ref " << fnd.use_count << " cache size " << cache.size() << endl; 
	}
	else {
		cerr << "fnd is nullptr at end of load" << endl;
	}
	return fnd;
}

void Bufferator::free()
{
	if (instance != nullptr) {
		instance->stop();
		delete instance;
		instance = nullptr;
	}
}

static AudioFile getAudioFile(string path) throws IOException, AudioFileException
{
	return WavFile.openWavFile(new File(path));
}

int Bufferator::chunkStartFrame(int chunkInd)
{
	return chunkInd * Bufferator::FRAMES_PER_CHUNK;
}

int Bufferator::chunk4Frame(int fr)
{
	int chunkInd = fr / Bufferator::FRAMES_PER_CHUNK;
	if (chunkInd < 0) return 0;
	return chunkInd;
}

/*
SampleChunkInfo[] Bufferator::allocateChunkBuffers(int nTotalFrames, short nChannels)
{
	int nc = (nTotalFrames + FRAMES_PER_CHUNK - 1) / FRAMES_PER_CHUNK;
	if (nc <= 0) nc = 1;
	SampleChunkInfo[] sci = new SampleChunkInfo[nc];

	for (int i = 0; i<sci.length; i++) {
		sci[i] = nullptr;
	}

	return sci;
}
*/


bool Bufferator::run()
{
	if (instance == nullptr) {
		return false;
	}
	return instance->runner();
}

bool Bufferator::stop()
{
	if (instance == nullptr) {
		return false;
	}
	isRunning = false;
	runnerThread.join();
	/*
	instance.stateSubject.onCompleted();
	instance.gfxSubject.onCompleted();
	*/
	return false;
}

void Bufferator::dispatchGfxConstruct(SampleInfo tgt)
{
//	gfxSubject.onNext(tgt);
}

void Bufferator::dispatchMafError(string txt)
{
	dispatchMaf(txt, ErrorLevel::ERROR_EVENT);
}

void Bufferator::dispatchMafWarning(string txt)
{
	dispatchMaf(txt, ErrorLevel::WARNING_EVENT);
}

void Bufferator::dispatchMafFatal(string txt)
{
	dispatchMaf(txt, ErrorLevel::FATAL_EVENT);
}

void Bufferator::dispatchMaf(string txt, int severity) {
//	stateSubject.onNext(new BufferatorException(txt, severity));
}
