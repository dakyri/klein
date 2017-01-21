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
	virtual void process(float **inputs, float **outputs, VstInt32 sampleframes); // @deprecated
	virtual void processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames) override;
	virtual VstInt32 processEvents(VstEvents* events) override;

	virtual void setProgram(VstInt32 program) override;
	virtual void setProgramName(char *name) override;
	virtual void getProgramName(char *name) override;

	virtual void setParameter(VstInt32 index, float value) override;
	virtual float getParameter(VstInt32 index) override;
	virtual void getParameterLabel(VstInt32 index, char *label) override;
	virtual void getParameterDisplay(VstInt32 index, char *text) override;
	virtual void getParameterName(VstInt32 index, char *text) override;
	virtual bool getParameterProperties(VstInt32 index, VstParameterProperties* p) override;

	virtual float getVu(); // @deprecated
	virtual void suspend() override;

	virtual VstInt32 getChunk(void** data, bool isPreset = false) override;
	virtual VstInt32 setChunk(void* data, VstInt32 byteSize, bool isPreset = false) override;

	virtual VstInt32 getMidiProgramName(VstInt32 channel, MidiProgramName* midiProgramName) override;
	virtual VstInt32 getCurrentMidiProgram(VstInt32 channel, MidiProgramName* currentProgram) override;
	virtual VstInt32 getMidiProgramCategory(VstInt32 channel, MidiProgramCategory* category) override;
	virtual bool hasMidiProgramsChanged(VstInt32 channel) override;
	virtual bool getMidiKeyName(VstInt32 channel, MidiKeyName* keyName) override;

	virtual bool getEffectName(char* name) override;
	virtual bool getVendorString(char* text) override;
	virtual bool getProductString(char* text) override;
	virtual VstInt32 getVendorVersion() override;
	virtual VstInt32 canDo(char* text) override;

	virtual void inputConnected(long index, bool state);	// Input at <index> has been (dis-)connected,
	virtual void outputConnected(long index, bool state);	// Same as input; state == true: connected
	virtual bool getInputProperties(long index, VstPinProperties* properties);
	virtual bool getOutputProperties(long index, VstPinProperties* properties);

	virtual void setBlockSize(VstInt32 blockSize) override;

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

	friend KleinEditor;
};


#include "debug.h"

#endif