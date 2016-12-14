#include <iostream>
#include <chrono>

#include "Bufferator.h"

Bufferator::ErrorLevel SampleInfo::lastErrorLevel;
string SampleInfo::lastErrorMessage;


/**
 * initially adapted from an android buffer runner.
 * planning on using this for a looper, so maybe won't be backed up by file unless the loop gets big or it gets saved for re-use.

 * we're perceiving files as fixed sized pages of data, and that's how we're pulling stuff from disk into our 'chunks'
 * the in-memory chunks will get manipulated in the planned use for this function, so chunks will get split, have sections
 * inserted or deleted into the file ... so the 'chunk' data in memory will drift from what was on disk.
 */
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

/*
using namespace std::chrono;
milliseconds now() {
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch());
}
*/
SampleInfo::SampleInfo(string _path, int _fileType, int _nChannels, int _sampleFormat, int _sampleRate)
	: path(_path), id(-1), nChannels(_nChannels), nTotalFrames(0), chunkChannelCount(0)
	, audioFile(SampleFile::WAVE_TYPE, nChannels, _sampleFormat, _sampleRate)
{
}

SampleInfo::SampleInfo()
	: path(""), id(-1), nChannels(0), nTotalFrames(0), chunkChannelCount(0)
{
//	gfxCache = nullptr;
}

void SampleInfo::setError(string msg, Bufferator::ErrorLevel level) {
	lastErrorMessage = msg;
	lastErrorLevel = level;
}

/**
 * open our connected audio file, which is presumed to already exist
 */
bool SampleInfo::openAudioFile(string _path, int _id)
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

	requestPage(0); // request start chunks from the interface request list
	requestPage(1);
	return true;
}

/**
* create our connected audio file, which is presumed to not exist. out data is going there eventually maybe
*/
bool SampleInfo::createAudioFile(string _path, int _id)
{
	if (_path == "") {
		return false;
	}
	path = _path;
	id = _id;
	nTotalFrames = 0;
	status_t err;
	if ((err = audioFile.setTo(path, O_RDWR | O_CREAT)) != ERR_OK) {
		setError("Catastrophe! Some kind of IO exception, while opening " + path);
		return false;

	}
	cerr << "set to " << path << " " << nChannels << " chan " << nTotalFrames << " frame" << endl;

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
*/
SampleChunkInfo * SampleInfo::readPage(int pageno) 
{
	if (!audioFile()) {
		return nullptr;
	}
	int bufferLen = Bufferator::FRAMES_PER_PAGE*nChannels;
	float * buffer = new float[bufferLen];
	int csf = Bufferator::pageStartFrame(pageno);
	audioFile.seekToFrame(csf);
	int framesRead = audioFile.plonk(buffer, Bufferator::FRAMES_PER_PAGE);
	SampleChunkInfo * sci = new SampleChunkInfo(csf, framesRead, bufferLen, buffer);
	return sci;
}

/**
* @see #getChunk(int)
* @param currentDataFrame address of the needed chunk
* @return the chunk, or null if not found
*/
SampleChunkInfo *SampleInfo::getChunk4Frame(off_t dataFrame) {
	if (!audioFile()) {
		return nullptr;
	}
	SampleChunkInfo *scip = findChunk4Frame(dataFrame);
	if (scip != nullptr) {
		time(&scip->timeStamp);
		return scip;
	}
	requestPage(Bufferator::page4Frame(dataFrame));
	return nullptr;
}

SampleChunkInfo *SampleInfo::findChunk4Frame(off_t dataFrame) {
	chunkLock.lock();
	for (auto sci : chunkList) {
		if (sci->startFrame <= dataFrame && dataFrame < sci->startFrame + sci->nFrames) {
			chunkLock.unlock();
			return sci;
		}
	}
	chunkLock.unlock();
	return nullptr;
}

/**
 *  gets a reference to the buffer for the requested chunk ... at the moment, assume we are being called by the
 *  audio thread, which is the only place at the moment where it is called from. if not found, we will try to
 *  request it
 * @param chunk the index of the needed chunk
 * @return the chunk, or null if not found
 */

/**
* @param pageno which chunk we want
* @return false if the chunk is valid, in range, but can't be requested; true otherwise
*/
bool SampleInfo::requestPage(int pageno)
{
	if (pageno < 0 || (pageno+1)*Bufferator::FRAMES_PER_PAGE > audioFile.nFrames) {
		return true;
	}

	for (auto pi: requestedChunks) {
		if (pi == pageno) {
			return true;
		}
	}
	requestLock.lock();
	if (requestedChunks.size() < Bufferator::MAX_REQUEST_PER_CYCLE) {
		requestedChunks.push_back(pageno);
	}
	requestLock.unlock();
	return false;
}

bool SampleInfo::updateLoadedChunks()
{
	return processChunkRequests();
}

bool SampleInfo::processChunkRequests() 
{
	// lock our main request list while building locally the list we process this cycle
	requestLock.lock();
	vector<int> reqList;
	auto ai = requestedChunks.begin();
	while (ai != requestedChunks.end()) {
		reqList.push_back(*ai);
		ai = requestedChunks.erase(ai);
	}
	requestLock.unlock();

	for (int pageno: reqList) {

		if (pageno >= 0 && Bufferator::pageStartFrame(pageno) < audioFile.nFrames) {
			cerr << "process " << reqList.size() << " requests " << pageno << " " << path << endl;
			SampleChunkInfo *sci = readPage(pageno);
			chunkLock.lock();
			auto cli = chunkList.begin();
			bool doInsert = true;
			while (cli != chunkList.end()) {
				if (sci->startFrame < (*cli)->startFrame) { // first position that fits
					if (sci->startFrame + sci->nFrames > (*cli)->startFrame) { // prevent overlap with buffer we're inserting it before
						sci->nFrames = (*cli)->startFrame - sci->startFrame;
					}
					break;
				} else if (sci->startFrame >= (*cli)->startFrame && sci->startFrame < (*cli)->startFrame + (*cli)->nFrames) { // overlap with this buffer
					if (sci->startFrame + sci->nFrames <= (*cli)->startFrame + (*cli)->nFrames) { // full overlap. skip the buffer just read
						doInsert = false;
						break;
					} else { // trim to prevent overlap. we should drop out of loop to insert on the next iteration
						(*cli)->nFrames = sci->startFrame - (*cli)->startFrame;
					}
				}
				++cli;
			}
			if (doInsert) {
				chunkList.insert(cli, sci);
				chunkChannelCount += nChannels;
			}
			chunkLock.unlock();
		}
	}
	return true;
}

/**
 * tries to clear up as many chunks (pages) as it can
 * @param fsi number of chunks to try to clear
 * @returns the number of chunks actually cleared
 */
int SampleInfo::trimAllocatedChunks(int fsi, int refCount)
{
	if (nTotalFrames == 0 || chunkList.size() == 0) {
		return 0;
	}
	if (fsi <= 0) {
		return 0;
	}
	if (chunkChannelCount <= refCount*nChannels*Bufferator::MIN_BUFFER_PER_SAMPLE) {
		return 0;
	}
	// find the fsi oldest chunks
	list<list<SampleChunkInfo*>::iterator> togo;

	chunkLock.lock();
	for (auto sci = chunkList.begin(); sci != chunkList.end(); sci++) {
		if (!isRequiredChunk(Bufferator::pageStartFrame((*sci)->startFrame))) {
			checkAddOldest(sci, togo);
			if (togo.size() >= fsi) {
				break;
			}
		}
	}
	chunkLock.unlock();
	int fsid = 0;
	for (auto scii: togo) {
		// that data should be safe to clear now, but just in case this chunk's being accessed in another thread we should maybe leave it
		// till the system decides it has no references ...
		auto sci = *scii;
		chunkLock.lock();
		chunkList.erase(scii);
		delete sci;
		chunkLock.unlock();
		fsid++;
		chunkChannelCount -= nChannels;
	}
	return fsid;
}

/**
 * inserts the next candidate for trimming into the list, 'togo'. list is sorted by age, oldest (most eligible for trimming) first
 */
void SampleInfo::checkAddOldest(list<SampleChunkInfo*>::iterator sci, list<list<SampleChunkInfo*>::iterator> &  togo)
{
	auto it = togo.begin();
	auto fnd = togo.end();
	while (it != togo.end()) {
		auto scit = **it;
		if (scit == nullptr) {
			fnd = it;
			break;
		}
		else if ((*sci)->timeStamp < scit->timeStamp) {
			fnd = it;
			break;
		}
		else {
		}
		++it;
	}
	if (fnd != togo.end()) {
		togo.insert(fnd, sci);
	}
}

SampleInfo::~SampleInfo()
{
	nTotalFrames = 0;
	// audio file is closed automatically
	for (auto sci: chunkList) {
		if (sci != nullptr) {
			delete sci;
		}
	}
}

/**
 * requirdChunks are chunks that are specifically needed always until cleared. so these are chunks that are not flushed, even if they're old
 * atm this seems to be only the region start
 * currently this is only called inside the buffer processing thread, so we're ok on thread safety
 */
void SampleInfo::clearRequiredChunks()
{
	requiredChunks.clear();
}

void SampleInfo::addRequiredChunk(int reqdChunk)
{
	if (isRequiredChunk(reqdChunk)) {
		return;
	}
	requiredChunks.push_back(reqdChunk);
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
	/*
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
	*/

	// S2 .... thresh 64.000000MB avail 256.933594MB max 48.000000MB class 48 large class 128
	// S2 with large heap: thresh 64.000000MB avail 424.964844MB max 128.000000MB class 48 large class 128

	cerr << " **** memory stats currently not implemented ... needs platform specific solution *** " << endl;
}

bool Bufferator::start() {
	runnerThread = thread(&Bufferator::runner, this);
	return true;
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
			nSampleInfoRefs += spip.use_count();
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
					int fsi = (fatPerInfoRef*spip->nChannels) / spip.use_count();
					int ftpsi = spip->trimAllocatedChunks(fsi, spip.use_count());
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
shared_ptr<SampleInfo> Bufferator::allocateInfoBlock(string path, int mxi)
{
	auto p = make_shared<SampleInfo>();
	if (!p->openAudioFile(path, mxi)) {
		return nullptr;
	}
	cacheLock.lock();
	cache.push_back(p);
	cacheLock.lock();
	return p;
}

shared_ptr<SampleInfo> Bufferator::allocateInfoBlock(string path, int mxi, int nChannels, int format, int type)
{
	auto p = make_shared<SampleInfo>();
	if (!p->createAudioFile(path, mxi)) {
		return nullptr;
	}
	cacheLock.lock();
	cache.push_back(p);
	cacheLock.lock();

}

/**
 * allocate a SampleInfo for buffering. if the file is already loaded, return the existent buffer
 * group ... then open the file and get ready for requests
 */
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
		cerr << "find " << fnd->path << " id " << fnd->id << " ref " << fnd.use_count() << " cache size " << cache.size() << endl; 
	}
	else {
		cerr << "fnd is nullptr at end of load" << endl;
	}
	return fnd;
}

/**
 * allocate a SampleInfo with a new clean buffer, create a sample file to use to back the data on this
 */
shared_ptr<SampleInfo> Bufferator::create(string path, int nChannels, int format, int type)
{
	int mxi = 1;
	shared_ptr<SampleInfo> fnd = nullptr;
	for (auto wci : cache) {
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
	if (fnd != nullptr) {
		return nullptr;
	}
	fnd = allocateInfoBlock(path, mxi, nChannels, format, type);
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

int Bufferator::pageStartFrame(int chunkInd)
{
	return chunkInd * Bufferator::FRAMES_PER_PAGE;
}

int Bufferator::page4Frame(int fr)
{
	int chunkInd = fr / Bufferator::FRAMES_PER_PAGE;
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

void Bufferator::dispatchGfxConstruct(SampleInfo *tgt)
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