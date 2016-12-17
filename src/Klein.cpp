
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

#undef KLEIN_DEBUG 11
#ifdef KLEIN_DEBUG
ofstream dbf;

#endif

#include "tinyxml2.h"

#include "Klein.h"
#include "AEffEditor.hpp"

unordered_map<string, int> kvFilterIdx = {
	/*
	{"Unity", sgfUnity},
	{"Cheesy LP", sgfCheesy303},
	{"Cheesy HP", sgfCheesy303Hp},
	{"Cheesy BP", sgfCheesy303Bp},
	{"Moog1 LP", sgfMoog1Lp},
	{"Moog1 HP", sgfMoog1Hp},
	{"Moog1 BP", sgfMoog1Bp},
	{"Moog2 LP", sgfMoog2Lp},
	{"Moog2 HP", sgfMoog2Hp},
	{"Moog2 BP", sgfMoog2Bp}*/
};


unordered_map<string, int> kvLFWaveformIdx = {
	/*
	{"Sine", sglfwSine},
	{"+Exp", sglfwPosExp},
	{"-Exp", sglfwNegExp},
	{"+Saw", sglfwPosSaw},
	{"-Saw", sglfwNegSaw},

	{"Square", sglfwSquare}*/
};

unordered_map<string, int> kvLFTargetIdx = {
	{"Direct", kDirectLevel},
	{"Dir Pan", kDirectPan}
};

//----------------------------------------------------------------------------- 

KleinProgram::KleinProgram()
{
	fOut = 0.7;
	fDirectLevel = 1;
	fDirectPan = 0;

	strcpy (name, "Init");
}




Klein::Klein(audioMasterCallback audioMaster)
	: AudioEffectX(audioMaster, 16, kNumParams)
	, controller(*this)
	, nTracks(4)
	, nLoopsPerTrack(4)
{
	nInputPort = 1;
	nOutputPort = 4;
	tracksSetupDone = false;
	
	loadConfig("config.xml");
	if (!tracksSetupDone) { 
		setNTracks(nTracks);
	}
	programs = new KleinProgram[numPrograms];


	nChunkFrames = 0;
	nChunkFrameRemaining = 0;

	fOut = 0.7;
	fDirectLevel = 1;
	fDirectPan = 0;

	if (programs) {
		setProgram(0);
	}

	vu = 0;

#ifdef KLEIN_DEBUG
	dbf.open("debug.out");
#endif
	
//	SGLFO::InitializeDefaultWavetables();
	setNumInputs (2*nInputPort);
	setNumOutputs (2*nOutputPort);
	hasVu ();
	canProcessReplacing ();
	setUniqueID ('KLYN');
	suspend ();		// flush buffer
	updateBlockSize(); // forces call to our override of setBlockSize which will allocate buffers
#ifdef KLEIN_DEBUG
	dbf << "Built the thing " << blockSize << " " << sampleRate << endl;
#endif
}

//------------------------------------------------------------------------
Klein::~Klein()
{
	short	i;
	if (programs)
		delete[] programs;
}

//------------------------------------------------------------------------
void
Klein::setProgram (long program)
{
	KleinProgram * ap = &programs[program];

	curProgram = program;
	short i;
	fOut = ap->fOut;
	fDirectLevel = ap->fDirectLevel;
	fDirectPan = ap->fDirectPan;
}

//------------------------------------------------------------------------
void Klein::setProgramName (char *name)
{
	strcpy (programs[curProgram].name, name);
}

//------------------------------------------------------------------------
void Klein::getProgramName (char *name)
{
	if (!strcmp (programs[curProgram].name, "Init"))
		sprintf (name, "%s %d", programs[curProgram].name, curProgram + 1);
	else
		strcpy (name, programs[curProgram].name);
}

//------------------------------------------------------------------------
void Klein::suspend ()
{
	nChunkFrames = 0;	// resets lfo check
}

//------------------------------------------------------------------------
float Klein::getVu ()
{
	float cvu = vu;
	
	vu = 0;
	return cvu;
}
//fFilterFrequency = ap->fFilterFrequency = 
//1FilterResonance = ap->fFilterResonance
//------------------------------------------------------------------------
void Klein::setParameter (long index, float value)
{
	KleinProgram * ap = &programs[curProgram];

#ifdef KLEIN_DEBUG
	dbf << "set p " <<  index << ", " << value << endl;
#endif
	switch (index)
	{
		case kOut :      fOut = ap->fOut = value; break;
		case kDirectLevel :  fDirectLevel = ap->fDirectLevel = value; break;
		case kDirectPan :  fDirectPan = ap->fDirectPan = 2*value-1; break;
	}
	if (editor)
		editor->postUpdate ();
}

//------------------------------------------------------------------------
float Klein::getParameter (long index)
{
	float v = 0;

	switch (index)
	{
		case kOut :      v = fOut; break;
		case kDirectLevel:  v = fDirectLevel; break;
		case kDirectPan:  v = (fDirectPan+1)/2; break;
	}
	return v;
}

//------------------------------------------------------------------------
char *
Klein::parameterName(long index)
{
	switch (index)
	{
		case kOut :				return "Output";
		case kDirectLevel :		return "DirectLevel";
		case kDirectPan :		return "DirectPan";
	}
	return "";
}

void Klein::getParameterName (long index, char *label)
{
	sprintf(label, " %10s", parameterName(index));
}

//------------------------------------------------------------------------
void Klein::getParameterDisplay (long index, char *text)
{
	switch (index)
	{
		case kOut :      dB2string (fOut, text); break;
		case kDirectLevel :      dB2string (fDirectLevel, text); break;
		case kDirectPan :      dB2string (fDirectPan, text); break;
	}
}

//------------------------------------------------------------------------
void Klein::getParameterLabel (long index, char *label)
{
	switch (index)
	{
		case kOut : 
		case kDirectLevel : 
		case kDirectPan :		strcpy (label, "   dB   ");	break;
	}
}

/**************************************************************************************
 * VST HOOKS
 *************************************************************************************/

void Klein::process (float **inputs, float **outputs, long nFrames)
{
	float cvu = vu;
	long currentOutFrame = 0;
	while (currentOutFrame < nFrames) {
		const VstTimeInfo *t = getTimeInfo(kVstTempoValid);
		long framesThisChunk = nFrames - currentOutFrame;
		for (auto ti = track.begin(); ti != track.end(); ++ti) {
			long l = ti->boringFrames(t, currentOutFrame);
			if (l > 0 && l < framesThisChunk) {
				framesThisChunk = l;
			}
		}
		for (auto ti = track.begin(); ti != track.end(); ++ti) {
			const long framesHandled = ti->processAdding(inputs, outputs, currentOutFrame, nFrames);
			float tvu = ti->getVu();
			if (tvu > cvu) {
				cvu = tvu;
			}
		}
		currentOutFrame += framesThisChunk;
	}
	vu = cvu;

}


//---------------------------------------------------------------------------
void Klein::processReplacing (float **inputs, float **outputs, long nFrames)
{
	float cvu = vu;
	for (int i = 0; i < nOutputPort * 2; ++i) {
		memset(outputs[i], 0, nFrames * sizeof(float));
	}
	long currentOutFrame = 0;
	while (currentOutFrame < nFrames) {
		const VstTimeInfo *t = getTimeInfo(kVstTempoValid);
		long framesThisChunk = nFrames-currentOutFrame;
		for (auto ti = track.begin(); ti != track.end(); ++ti) {
			long l = ti->boringFrames(t, currentOutFrame);
			if (l > 0 && l < framesThisChunk) {
				framesThisChunk = l;
			}
		}
		for (auto ti = track.begin(); ti != track.end(); ++ti) {
			const long framesHandled = ti->processAdding(inputs, outputs, currentOutFrame, nFrames);
			float tvu = ti->getVu();
			if (tvu > cvu) {
				cvu = tvu;
			}
		}
		currentOutFrame += framesThisChunk;
	}
	vu = cvu;
}

float
Klein::getTempo()
{
	const VstTimeInfo *t = getTimeInfo(kVstTempoValid);
	return t->tempo;
}

void Klein::allocateBuffers(long blockSize)
{
	for (KleinTrack & i: track) {
		i.allocateBuffers(blockSize);
	}
}

/*

const char* plugCanDos [] =
{
"sendVstEvents",
"sendVstMidiEvent",
"sendVstTimeInfo",
"receiveVstTimeInfo",
"offline",
"plugAsChannelInsert",
"plugAsSend",
"mixDryWet",
"noRealTime",
"multipass",
"metapass",

#if VST_2_1_EXTENSIONS
,
"midiProgramNames",
"conformsToWindowRules"		// mac: doesn't mess with grafport. general: may want
// to call sizeWindow (). if you want to use sizeWindow (),
// you must return true (1) in canDo ("conformsToWindowRules")
#endif // VST_2_1_EXTENSIONS

#if VST_2_3_EXTENSIONS
,
"bypass"
#endif // VST_2_3_EXTENSIONS
};

*/
long Klein::canDo(char* text)
{
	if (!strcmp(text, "receiveVstEvents"))
		return 1;
	if (!strcmp(text, "receiveVstMidiEvent"))
		return 1;
	if (!strcmp(text, "midiProgramNames"))
		return 1;
	if (!strcmp(text, "1in1out")) {
		return 1;
	}
	if (!strcmp(text, "1in2out")) {
		return -1;
	}
	if (!strcmp(text, "2in1out")) {
		return -1;
	}
	if (!strcmp(text, "2in2out")) {
		return -1;
	}
	if (!strcmp(text, "2in4out")) {
		return -1;
	}
	if (!strcmp(text, "4in2out")) {
		return -1;
	}
	if (!strcmp(text, "4in4out")) {
		return -1;
	}
	if (!strcmp(text, "4in8out")) {	// 4:2 matrix to surround bus
		return -1;
	}
	if (!strcmp(text, "8in4out")) {	// surround bus to 4:2 matrix
		return -1;
	}
	if (!strcmp(text, "8in8out")) {
		return -1;
	}
	return -1;	// explicitly can't do; 0 => don't know
}

void Klein::inputConnected(long index, bool state)
{
}

void Klein::outputConnected(long index, bool state)
{
}


bool Klein::getInputProperties(long index, VstPinProperties* properties)
{
	bool ret = false;

	if (index < nInputPort * 2) {
		sprintf(properties->label, "Input %1d", (index / 2) + 1);
		sprintf(properties->shortLabel, "In %1d", (index / 2) + 1);
		properties->flags = kVstPinIsActive;
		if (index % 2 == 0) {
			properties->flags |= kVstPinIsStereo;
		}
		ret = true;
	}
	return ret;
}

bool Klein::getOutputProperties(long index, VstPinProperties* properties)
{
	bool ret = false;
	
	if (index < nOutputPort*2) {
		sprintf(properties->label, "Output %1d", (index/2)+1);
		sprintf(properties->shortLabel, "Out %1d", (index / 2) + 1);
		properties->flags = kVstPinIsActive;
		if (index % 2 == 0) {
			properties->flags |= kVstPinIsStereo;	
		}
		ret = true;
	}
	return ret;
}


bool Klein::getEffectName(char* name)
{
	strcpy(name, "Klein");
	return true;
}

bool Klein::getVendorString(char* text)
{
	strcpy(text, "Mayaswell Media Technologies");
	return true;
}


bool Klein::getProductString(char* text)
{
	strcpy(text, "Klein Looper");
	return true;
}

long Klein::getVendorVersion() {
	return 1;
}


// midi program names:
// as an example, GM names are used here. in fact, VstXSynth doesn't even support
// multi-timbral operation so it's really just for demonstration.
// a 'real' instrument would have a number of voices which use the
// programs[channelProgram[channel]] parameters when it receives
// a note on message.

//------------------------------------------------------------------------
long Klein::getMidiProgramName(long channel, MidiProgramName* mpn)
{/*
	long prg = mpn->thisProgramIndex;
	if (prg < 0 || prg >= 128)
		return 0;
	fillProgram(channel, prg, mpn);
	if (channel == 9)
		return 1;*/
	return 0;
}

//------------------------------------------------------------------------
long Klein::getCurrentMidiProgram(long channel, MidiProgramName* mpn)
{/*
	if (channel < 0 || channel >= 16 || !mpn)
		return -1;
	long prg = channelPrograms[channel];
	mpn->thisProgramIndex = prg;
	fillProgram(channel, prg, mpn);*/
	return 0;
}


//------------------------------------------------------------------------
long Klein::getMidiProgramCategory(long channel, MidiProgramCategory* cat)
{
	/*
	cat->parentCategoryIndex = -1;	// -1:no parent category
	cat->flags = 0;					// reserved, none defined yet, zero.
	long category = cat->thisCategoryIndex;
	if (channel == 9)
	{
		strcpy(cat->name, "Drums");
		return 1;
	}
	if (category >= 0 && category < kNumGmCategories)
		strcpy(cat->name, GmCategories[category]);
	else
		cat->name[0] = 0;
	return kNumGmCategories;*/
	return 0;
}


//-----------------------------------------------------------------------------------------
long Klein::processEvents(VstEvents* ev)
{
	cerr << "process "<< ev->numEvents <<" events in xsynth non " << endl;
	for (long i = 0; i < ev->numEvents; i++) {
		if (ev->events[i]->type == kVstMidiType) {
			VstMidiEvent* event = (VstMidiEvent*)ev->events[i];
			controller.processEvent(event);
		}
	}
	return 1;	// want more
}

bool Klein::getMidiKeyName(long channel, MidiKeyName* key)
// struct will be filled with information for 'thisProgramIndex' and 'thisKeyNumber'
// if keyName is "" the standard name of the key will be displayed.
// if false is returned, no MidiKeyNames defined for 'thisProgramIndex'.
{
	/*
	// key->thisProgramIndex;		// >= 0. fill struct for this program index.
	// key->thisKeyNumber;			// 0 - 127. fill struct for this key number.
	key->keyName[0] = 0;
	key->reserved = 0;				// zero
	key->flags = 0;					// reserved, none defined yet, zero.*/
	return false;
}

//------------------------------------------------------------------------
bool Klein::hasMidiProgramsChanged(long channel)
{
	return false;	// updateDisplay ()
}

/**************************************************************************************
* CONFIGURATION
*************************************************************************************/
string configLoadErrorStr = "";

using namespace tinyxml2;

status_t
Klein::loadConfig(const char *path)
{
	XMLDocument doc;

	XMLError err = doc.LoadFile(path);
	if (err != XML_NO_ERROR) {
		configLoadErrorStr = doc.GetErrorStr1();
		return err;
	}
	XMLElement* root = doc.RootElement();

	string namestr = root->Value();
	if (namestr == "klein") {
		const char *inputPortAttrVal = root->Attribute("nInputPort");
		const char *outputPortAttrVal = root->Attribute("nOutputPort");
		if (inputPortAttrVal) nInputPort =  atoi(inputPortAttrVal);
		if (outputPortAttrVal) nOutputPort = atoi(outputPortAttrVal);

		XMLElement *childElement = root->FirstChildElement();
		while (childElement != nullptr) {
			std::string childName = childElement->Value();
			if (childName == "tracks") {
				err = loadTrackConfig(childElement);
			} else if (childName == "scripts") {
				err = loadScriptConfig(childElement);
			} else if (childName == "midiMap") {
				err = loadMidiMapConfig(childElement);
			}
			if (err != XML_NO_ERROR) {
				configLoadErrorStr = doc.GetErrorStr1();
				return err;
			}
			childElement = childElement->NextSiblingElement();
		}

	} else {
		configLoadErrorStr = "Expected 'klein' as root element";
		return KF_ERROR;
	}
	return KF_OK;
}

XMLError Klein::loadTrackConfig(tinyxml2::XMLElement *element) {
	const char *trackAttrVal = element->Attribute("nTracks");
	const char *loopAttrVal = element->Attribute("nLoopsPerTracks");
	XMLElement *childElement = element->FirstChildElement();
	nTracks = atoi(trackAttrVal);
	nLoopsPerTrack = atoi(loopAttrVal);
	track.clear();
	int i = 0;
	int nTrackAdded = 0;
	while (childElement != nullptr) {
		std::string childName = childElement->Value();
		if (childName == "track") {
			const char *idAttrVal = childElement->Attribute("id");
			const char *inPortAttrVal = childElement->Attribute("inPort");
			const char *outPortAttrVal = childElement->Attribute("outPort");
			const char *syncSrcAttrVal = childElement->Attribute("syncSrc");
			const char *syncUnitAttrVal = childElement->Attribute("syncUnit");

			int trackId = idAttrVal? atoi(idAttrVal): i;
			int inPortId = inPortAttrVal? atoi(inPortAttrVal): 0;
			int outPortId = outPortAttrVal? atoi(outPortAttrVal): 0;
			SyncSource ss = syncSrcAttrVal ? syncSrc4(syncSrcAttrVal) : SYNC_HOST;
			SyncUnit su = syncUnitAttrVal ? syncUnit4(syncUnitAttrVal) : SYNC_LOOP;

			if (i <= trackId) i = trackId + 1;

			track.push_back(KleinTrack(trackId, inPortId, outPortId, nLoopsPerTrack, ss, su));
			++nTrackAdded;
		}
		while (nTrackAdded < nTracks) {
			track.push_back(KleinTrack(i++, 0, 0, nLoopsPerTrack, SYNC_HOST, SYNC_LOOP));
		}
		childElement = childElement->NextSiblingElement();
	}
	return XML_NO_ERROR;
}

XMLError Klein::loadScriptConfig(tinyxml2::XMLElement *element) {
	XMLElement *childElement = element->FirstChildElement();
	while (childElement != nullptr) {
		std::string childName = childElement->Value();
		const char *idAttrVal = childElement->Attribute("id");
		const char *srcAttrVal = childElement->Attribute("src");

		if (idAttrVal && srcAttrVal) {
			status_t err = controller.loadScript(idAttrVal, srcAttrVal);
			if (err != ERR_OK) {

			}
		}

		childElement = childElement->NextSiblingElement();
	}
	return XML_NO_ERROR;
}

XMLError Klein::loadMidiMapConfig(tinyxml2::XMLElement *element) {
	XMLElement *childElement = element->FirstChildElement();
	while (childElement != nullptr) {
		std::string childName = childElement->Value();
		const char *channelAttrVal = childElement->Attribute("channel");
		const char *whichAttrVal = childElement->Attribute("which");
		const char *contextAttrVal = childElement->Attribute("context");

		const char *functionAttrVal = childElement->Attribute("function");
		const char *controlAttrVal = childElement->Attribute("control");
		const char *scriptAttrVal = childElement->Attribute("script");

		int channel = channelAttrVal ? atoi(channelAttrVal) : 0;
		int which = whichAttrVal ? atoi(whichAttrVal): 0;

		ControlMapping mapping;
		bool hasMapping = false;
		if (functionAttrVal) {
			hasMapping = true;
		}
		else if (controlAttrVal) {
			hasMapping = true;
		}
		else if (scriptAttrVal) {
			hasMapping = true;
		}

		if (hasMapping) {
			if (childName == "note") {
				controller.addNoteMapping(mapping, channel, which);
			}
			else if (childName == "ctrl") {
				controller.addCtrlMapping(mapping, channel, which);
			}
			else if (childName == "prog") {
				controller.addProgMapping(mapping, channel, which);
			}
		}

		childElement = childElement->NextSiblingElement();
	}
	return XML_NO_ERROR;

}

void Klein::setNTracks(int n)
{
	nTracks = n;
	track.clear();
	for (int i = 0; i < n; i++) {
		track.push_back(KleinTrack(i, nLoopsPerTrack));
	}
}

void Klein::setNLoopsPerTrack(int n)
{
	nLoopsPerTrack = n;
	for (auto it = track.begin(); it != track.end(); ++it) {
		it->setNLoops(n);
	}
}

void Klein::allocateBuffers(long blocksize) {
	AudioEffectX::allocateBuffers(blocksize);
	allocateBuffers(blocksize);
}
