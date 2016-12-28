#ifndef _BUFFERATOR
#define _BUFFERATOR

using namespace std;

#include <vector>
#include <string>
#include <ctime>
#include <thread>
#include <memory>
#include <mutex>
#include <list>

#include "SampleFile.h"


class SampleGfxInfo
{
public:
	SampleGfxInfo(int l, vector<vector<float>> &min, vector<vector<float>> max) {
		dataMin = min;
		dataMax = max;
	}

	SampleGfxInfo() {
	}

	vector<vector<float>> dataMin;
	vector<vector<float>> dataMax;
};

/*
class BufferatorException extends Exception {
	final int severity;
	final string path;

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
class SampleChunkInfo
{
public:
	float* data;
	int startFrame;
	int nFrames;
	int length;
	time_t timeStamp;
	bool dirty;

	SampleChunkInfo(int start, int _nFrames, int _length, float* dta)
	{
		startFrame = start;
		data = dta;
		nFrames = _nFrames;
		length = _length;
		timeStamp = time(nullptr);
		dirty = false;
	}

	virtual ~SampleChunkInfo() {
		if (data != nullptr) {
			delete[] data;
		}
	}

	int nextChunkStartFrame() {
		return startFrame + nFrames;
	}

	int prevChunkEndFrame() {
		return startFrame - 1;
	}
};

const int kMaxRequestsPerCycle = 8;

class SampleInfo;

class Bufferator
{
public:
	class Client {

	};

	enum ErrorLevel {
		FATAL_EVENT,
		WARNING_EVENT,
		ERROR_EVENT,
		FNF_ERROR_EVENT
	};

	static const int DEFAULT_CACHE_SIZE = 9;
	static const int FRAMES_PER_PAGE = 64 * 1024;
	static const int MAX_REQUEST_PER_CYCLE = 12;
	static const int MIN_BUFFER_PER_SAMPLE = 3;

private:
	static Bufferator* instance;

	vector<weak_ptr<SampleInfo>> cache;
	mutex cacheLock;

	bool isRunning = true;
	int nCached = 0; // keeping track mainly for statistical purposes
	
	float maxMemory = 0;
	float threshMemory = 0;
	float availMemory = 0;
	float totalPssMemory = 0;
	float bufferRamLimit = 0.9f; // how close to the edge we are sailing ... ie proportion of the total spare we are going to run up to
	float fairBufferMemory = 0;
	int fairChunkBufferChannelCount=0;
	int initialChunkBufferChannelCount=0;
	int minBufferPerSample = 4;
	thread runnerThread;

	/*
	BehaviorSubject<BufferatorException> stateSubject = BehaviorSubject.create();
	PublishSubject<SampleInfo> gfxSubject = PublishSubject.create();
	*/

	Bufferator::Client *sampleManager = nullptr;
private:
	Bufferator(Bufferator::Client *spm);
	virtual ~Bufferator();

public:
	static Bufferator *getInstance(Bufferator::Client *spg = nullptr);
	static void free();
	
	/*
	Subscription monitorState(Action1<BufferatorException> action) {
		return stateSubject.observeOn(AndroidSchedulers.mainThread()).subscribe(action);
	}

	Subscription monitorGfx(Action1<SampleInfo> observer) {
		return gfxSubject.observeOn(AndroidSchedulers.mainThread()).subscribe(observer);
	}
	*/

	bool fixMemoryPanic();
	void checkAvailableHeap();
	bool start();
	bool stop();
	bool runner();
	shared_ptr<SampleInfo> load(string path);
	shared_ptr<SampleInfo> create(
		string _path, SampleFile::FileType _fileType, short _nChannel, short _sampleSize, float _sampleRate, bool doOpen);

	static shared_ptr<SampleInfo> doLoad(string path);
	static shared_ptr<SampleInfo> doCreate(
		string _path, SampleFile::FileType _fileType, short _nChannel, short _sampleSize, float _sampleRate, bool doOpen);

	static int pageStartFrame(int chunkInd);
	static int page4Frame(int fr);
	static bool doStart();
	static bool doStop();

protected:
	shared_ptr<SampleInfo> allocateInfoBlock(int mxi, string path);
	shared_ptr<SampleInfo> allocateInfoBlock(
		int mxi, string _path, SampleFile::FileType _fileType, short _nChannel, short _sampleSize, float _sampleRate, bool doOpen);
	void dispatchGfxConstruct(SampleInfo *tgt);
	void dispatchMafError(string txt);
	void dispatchMafWarning(string txt);
	void dispatchMafFatal(string txt);
	void dispatchMaf(string txt, int severity);

};

class SampleInfo
{
public:
	int id = -1;
	int nTotalFrames = 0;
	short nChannels = 0;

	//	int refCount = 0;
	SampleFile audioFile;
	int chunkChannelCount;

	mutex chunkLock;
	list<SampleChunkInfo*> chunkList;

	//	SampleGfxInfo * gfxCache = nullptr;
	mutex requestLock;
	vector<int> requestedChunks;
	vector<int> requiredChunks;

	static Bufferator::ErrorLevel lastErrorLevel;
	static string lastErrorMessage;

	int16_t *buffer;

public:
	SampleInfo(string path, int _fileType, int _nChannels, int _sampleFormat, int _sampleRate);
	SampleInfo();
	virtual ~SampleInfo();

	bool openAudioFile(string _path="");
	bool createAudioFile(string _path="");
	void setId(int id);
	void setError(string msg, Bufferator::ErrorLevel lvl = Bufferator::ErrorLevel::ERROR_EVENT);

	bool addChunk(long startFrame, long nFrames, float *buffer);

	/*
	Observable<SampleGfxInfo> getMinMax(final int npoints);
	*/

	SampleChunkInfo *readPage(int chunk);
	SampleChunkInfo *getChunk4Frame(off_t dataFrame);
	SampleChunkInfo *findChunk4Frame(off_t dataFrame);

	string getLastErrorMessage() { return lastErrorMessage;  }
	string getPath() { return audioFile.path;  }
	bool requestPage(int page);
	bool updateLoadedChunks();
	bool processChunkRequests();
	int trimAllocatedChunks(int fsi, int refCount = 1);
	void checkAddOldest(list<SampleChunkInfo*>::iterator sci, list<list<SampleChunkInfo*>::iterator> &  togo);
	//	static SampleChunkInfo* allocateChunkBuffers(int nTotalFrames, short nChannels);

protected:
	bool placeChunk(SampleChunkInfo *sci);
	bool insertChunk(SampleChunkInfo * sci);
	bool writeChunk(SampleChunkInfo * sci);
	bool isRequiredChunk(int c);
	void addRequiredChunk(int reqdChunk);
	void clearRequiredChunks();

	/*	Observable<Path[]> getSamplePaths(final int w, final int h) ;*/

	float getFrameCount();

	friend Bufferator;
};


#endif