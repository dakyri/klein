#ifndef _BUFFERATOR
#define _BUFFERATOR

using namespace std;

#include <vector>
#include <string>
#include <ctime>
#include <thread>
#include <memory>
#include <mutex>

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
	int id = 0;
	float* data;
	int startFrame;
	int nFrames;
	time_t timeStamp;

	SampleChunkInfo(int start, int length, int ci, float* dta)
	{
		id = ci;
		startFrame = start;
		data = dta;
		nFrames = length;
		timeStamp = time(nullptr);
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
	static const int FRAMES_PER_CHUNK = 64*1024;

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
	bool runner();
	shared_ptr<SampleInfo> load(string path);
	shared_ptr<SampleInfo> allocateInfoBlock(string path, int mxi);

//	static void free(SampleInfo *inf); // suspect that this is not needed in c++. the weak pointers will be invalid when all shared refs are reset()
	static SampleFile & getAudioFile(string path);
	static int chunkStartFrame(int chunkInd);
	static int chunk4Frame(int fr);
//	static void cleanup();
	static bool run();
	bool stop();

	void dispatchGfxConstruct(SampleInfo tgt);
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
	string path;
	//	int refCount = 0;
	SampleFile audioFile;
	int chunkChannelCount;

	vector<SampleChunkInfo*> sampleChunk;

	//	SampleGfxInfo * gfxCache = nullptr;
	vector<int> reqChunkAuThread;
	vector<int> reqChunkIfThread;

	static Bufferator::ErrorLevel lastErrorLevel;
	static string lastErrorMessage;

public:
	SampleInfo();
	virtual ~SampleInfo();

	bool setSampleData(string _path, int _id);
	void setError(string msg, Bufferator::ErrorLevel lvl = Bufferator::ErrorLevel::ERROR_EVENT);


	/*
	Observable<SampleGfxInfo> getMinMax(final int npoints);
	*/

	SampleChunkInfo *readChunk(int chunk);
	SampleChunkInfo *getChunkFor(int currentDataFrame);
	SampleChunkInfo * getChunk(int chunk);
	string getLastErrorMessage();
	bool requestChunk(int requestedChunk, int from);
	bool updateLoadedChunks();
	bool processChunkRequests(vector<int>& reqList);
	int trimAllocatedChunks(int fsi);
	void checkAddOldest(SampleChunkInfo *sci, vector<SampleChunkInfo*> &  togo);
	//	static SampleChunkInfo* allocateChunkBuffers(int nTotalFrames, short nChannels);
	bool isRequiredChunk(int c);

	vector<int> requiredChunks;
	int nRequiredChunks = 0;
	void clearRequiredChunks();

	void addRequiredChunk(int reqdChunk);

	/*	Observable<Path[]> getSamplePaths(final int w, final int h) ;*/

	float getFrameCount();
};

#endif