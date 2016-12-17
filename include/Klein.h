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

#include "Delay.h"

enum {
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
	float fOut;
	float fDirectLevel;
	float fDirectPan;
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
	virtual void process(float **inputs, float **outputs, long sampleframes);
	virtual void processReplacing(float **inputs, float **outputs, long sampleFrames);
	virtual long processEvents(VstEvents* events);


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

	virtual long getMidiProgramName(long channel, MidiProgramName* midiProgramName);
	virtual long getCurrentMidiProgram(long channel, MidiProgramName* currentProgram);
	virtual long getMidiProgramCategory(long channel, MidiProgramCategory* category);
	virtual bool hasMidiProgramsChanged(long channel);
	virtual bool getMidiKeyName(long channel, MidiKeyName* keyName);

	virtual bool getEffectName(char* name);
	virtual bool getVendorString(char* text);
	virtual bool getProductString(char* text);
	virtual long getVendorVersion();
	virtual long canDo(char* text);

	virtual void inputConnected(long index, bool state);	// Input at <index> has been (dis-)connected,
	virtual void outputConnected(long index, bool state);	// Same as input; state == true: connected
	virtual bool getInputProperties(long index, VstPinProperties* properties);
	virtual bool getOutputProperties(long index, VstPinProperties* properties);

	virtual void allocateBuffers(long blockSize) override;

	char *parameterName(long index);

private:
	float getTempo();
	void allocateBuffers(long blockSize);

	KleinProgram *programs;
	bool tracksSetupDone;

	int nTracks;
	int nLoopsPerTrack;
	int nInputPort;
	int nOutputPort;

	vector<KleinTrack> track;
	Controller controller;

	status_t loadConfig(const char *path);
	tinyxml2::XMLError loadTrackConfig(tinyxml2::XMLElement *element);
	tinyxml2::XMLError loadScriptConfig(tinyxml2::XMLElement *element);
	tinyxml2::XMLError loadMidiMapConfig(tinyxml2::XMLElement *element);

	void setNTracks(int n);
	void setNLoopsPerTrack(int n);

	float			fOut;
	float			fDirectLevel;
	float			fDirectPan;

	long			nChunkFrames;
	long			nChunkFrameRemaining;

	float			vu;
private:
	float			beatT;
	float			feedback;
	float			directL, directR;
};

extern unordered_map<string, int> kvFilterIdx;
extern unordered_map<string, int> kvLFTargetIdx;
extern unordered_map<string, int> kvLFWaveformIdx;

#endif