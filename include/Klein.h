#ifndef _KLEIN_H
#define _KLEIN_H

#include "tinyxml2.h"

#include "audioeffectx.h"

#include <string>
#include <unordered_map>

using namespace std;

#include "Bufferator.h"
#include "KleinTrack.h"
#include "Controller.h"
#include "InputInfo.h"

enum ParameterId {
	kIdMasterGain,
	kNumGlobalParams,
	kTrackParamBase=kNumGlobalParams
};

enum TrackParameterId {
	kIdTrackInputGain,
	kIdTrackGain,
	kIdTrackPan,
	kIdTrackFeedback,
	kNumTrackParams
};

const int kMaxTrack = 16;

// 0 <= track < MAX_TRACK
inline int trackIdBase(int track) { return kTrackParamBase + track*kNumTrackParams; }
inline int trackParamId4Idx(int idx) { return (idx - kTrackParamBase) % kNumTrackParams;  }
inline int track4Idx(int idx) { return (idx - kTrackParamBase) / kNumTrackParams; }
inline int idTrackInputGain(int track) { return trackIdBase(track) + kIdTrackInputGain; }
inline int idTrackGain(int track) { return trackIdBase(track) + kIdTrackGain; }
inline int idTrackPan(int track) { return trackIdBase(track) + kIdTrackPan; }
inline int idTrackFeedback(int track) { return trackIdBase(track) + kIdTrackPan; }


class KleinEditor;

class KleinProgram
{
	friend class Klein;
public:
	KleinProgram();
	~KleinProgram() {}
private:
	float masterGain;
	float inputGain[kMaxTrack];
	float outputGain[kMaxTrack];
	float feedback[kMaxTrack];
	float pan[kMaxTrack];

	char name[24];
};


class Klein : public AudioEffectX
{
public:

	enum {
		KF_OK = 0,
		KF_ERROR = 200,
	};

	Klein(audioMasterCallback audioMaster);
	virtual ~Klein();


	static const int MAX_CHANNELS = 8;

/* VST callbacks */
	virtual void process(float **inputs, float **outputs, long sampleframes) override;
	virtual void processReplacing(float **inputs, float **outputs, long sampleFrames) override;
	virtual long processEvents(VstEvents* events) override;

	virtual void setProgram(long program) override;
	virtual void setProgramName(char *name) override;
	virtual void getProgramName(char *name) override;

	virtual void setParameter(long index, float value) override;
	virtual float getParameter(long index) override;
	virtual void getParameterLabel(long index, char *label) override;
	virtual void getParameterDisplay(long index, char *text) override;
	virtual void getParameterName(long index, char *text) override;
	virtual bool getParameterProperties(long index, VstParameterProperties* p) override;

	virtual float getVu() override;
	virtual void suspend() override;

	virtual long getChunk(void** data, bool isPreset = false) override;
	virtual long setChunk(void* data, long byteSize, bool isPreset = false) override;

	virtual long getMidiProgramName(long channel, MidiProgramName* midiProgramName) override;
	virtual long getCurrentMidiProgram(long channel, MidiProgramName* currentProgram) override;
	virtual long getMidiProgramCategory(long channel, MidiProgramCategory* category) override;
	virtual bool hasMidiProgramsChanged(long channel) override;
	virtual bool getMidiKeyName(long channel, MidiKeyName* keyName) override;

	virtual bool getEffectName(char* name) override;
	virtual bool getVendorString(char* text) override;
	virtual bool getProductString(char* text) override;
	virtual long getVendorVersion() override;
	virtual long canDo(char* text) override;

	virtual void inputConnected(long index, bool state);	// Input at <index> has been (dis-)connected,
	virtual void outputConnected(long index, bool state);	// Same as input; state == true: connected
	virtual bool getInputProperties(long index, VstPinProperties* properties);
	virtual bool getOutputProperties(long index, VstPinProperties* properties);

	virtual void allocateBuffers(long blockSize) override;

	char *parameterName(long index);

	KleinTrack &getTrack(int which);

	// commands
	bool selectTrack(int which);
	bool globalMute();
	bool globalPause();
	bool globalReset();

	void setInPort(const KleinTrack &t, const int port);
	void setOutPort(const KleinTrack &t, const int port);
	InputInfo *getInputInfo(const int pin);

protected:
	float getTempo();
	void allocateChildBuffers(long blockSize);

	KleinProgram *programs;
	bool tracksSetupDone;
	bool inputSetupDone;

	int nTracks;
	int nLoopsPerTrack;
	int nInputPort;
	int nOutputPort;

	vector<unique_ptr<KleinTrack>> track;
	vector<InputInfo> input;
	Controller controller;

	status_t loadConfig(const char *path, vector<string> &errorList);
	tinyxml2::XMLError loadTrackConfig(tinyxml2::XMLElement *element, vector<string> &errorList);
	tinyxml2::XMLError loadInputConfig(tinyxml2::XMLElement *element, vector<string> &errorList);
	tinyxml2::XMLError loadScriptConfig(tinyxml2::XMLElement *element, vector<string> &errorList);
	tinyxml2::XMLError loadMidiMapConfig(tinyxml2::XMLElement *element, vector<string> &errorList);

	int setNTracks(int n, int id = 0, bool clear = false);
	int setNInput(int n, int pin = 0, bool clear = false);
	void setNLoopsPerTrack(int n);

	long nChunkFrames;
	long nChunkFrameRemaining;

	float vu;

	KleinEditor *kleinView;

// vst parameters
	float masterGain;
};


#include "debug.h"

#endif