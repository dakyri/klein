
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

#include "tinyxml2.h"

#include "Klein.h"
#include "GUI/KleinEditor.h"
 

#ifdef KLEIN_DEBUG
ofstream dbf;

#endif

KleinProgram::KleinProgram()
{
	masterGain = 0.7;

	strcpy (name, "Init");
}

/**
 *
 */
Klein::Klein(audioMasterCallback audioMaster)
	: AudioEffectX(audioMaster, 16, 0/*trackIdBase(kMaxTrack)*/)
	, controller(*this)
	, nTracks(8)
	, nLoopsPerTrack(4)
	, kleinView(nullptr)
	, tracksSetupDone(false), inputSetupDone(false)
	, programs(nullptr)
{
	nInputPort = 1;
	nOutputPort = 4;

#ifdef KLEIN_DEBUG
	dbf.open("debug.txt");
	dbf << unitbuf;
#endif
#ifndef MINIMAL_TEST
	vector<string> configErrorList;
	status_t err = loadConfig("config.xml", configErrorList);
	if (err == ERR_OK) {
		controller.lockAndLoadScripts(configErrorList);
	}
	if (!inputSetupDone) {
		setNInput(nInputPort);
	}
	if (!tracksSetupDone) { 
		setNTracks(nTracks);
	}

	programs = new KleinProgram[numPrograms];
#endif

	nChunkFrames = 0;
	nChunkFrameRemaining = 0;

	masterGain = 0.7;

	if (programs) {
		setProgram(0);
	}

	vu = 0;
	
//	SGLFO::InitializeDefaultWavetables();
	setNumInputs (2*nInputPort);
	setNumOutputs (2*nOutputPort);
//	hasVu ();
	canProcessReplacing ();
	setUniqueID ('KLYN');
	suspend ();		// flush buffer
	updateBlockSize(); // forces call to our override of setBlockSize which will allocate buffers
#ifdef KLEIN_DEBUG
	dbf << "Built the thing " << blockSize << " " << sampleRate << endl;
#endif

	setEditor(kleinView = new KleinEditor(this));
}

//------------------------------------------------------------------------
Klein::~Klein()
{
	short	i;
	if (programs)
		delete[] programs;
	// KleinEditor should be deleted by base class
}


/**************************************************************************************
 * VST PROGRAM/PARAMETER HOOKS
 *************************************************************************************/
void
Klein::setProgram (VstInt32 program)
{
	KleinProgram * ap = &programs[program];

	curProgram = program;
	short i;
	masterGain = ap->masterGain;
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
void Klein::setParameter (VstInt32 index, float value)
{
	KleinProgram * ap = &programs[curProgram];

#ifdef KLEIN_DEBUG
	dbf << "set p " <<  index << ", " << value << endl;
#endif
	switch (index) {
		case kIdMasterGain: masterGain = ap->masterGain = value; break;
		default:
			int trackid = track4Idx(index);
			if (trackid >= 0 && trackid < kMaxTrack && trackid < track.size()) {
				switch (trackParamId4Idx(index)) {
				case kIdTrackInputGain: return track[trackid]->setInputGain(ap->inputGain[trackid]=value);
				case kIdTrackGain:		return track[trackid]->setOutputGain(ap->outputGain[trackid] = value);
				case kIdTrackPan:		return track[trackid]->setPan(ap->pan[trackid] = value);
				case kIdTrackFeedback:	return track[trackid]->setFeedback(ap->feedback[trackid] = value);
				}
			}
	}

	if (editor) {
//		editor->postUpdate();
		((AEffGUIEditor*)editor)->setParameter(index, value);
	}

}

//------------------------------------------------------------------------
float Klein::getParameter (VstInt32 index)
{
	float v = 0;


#ifdef KLEIN_DEBUG
	dbf << "get p " << index << endl;
#endif
	switch (index)	{
		case kIdMasterGain: v = masterGain; break;
		default:
			int trackid = track4Idx(index);
			if (trackid >= 0 && trackid < track.size()) {
				switch (trackParamId4Idx(index)) {
				case kIdTrackInputGain: return track[trackid]->getInputGain();
				case kIdTrackGain:		return track[trackid]->getOutputGain();
				case kIdTrackPan:		return track[trackid]->getPan();
				case kIdTrackFeedback:	return track[trackid]->getFeedback();
				}
			}
	}
	return v;
}

//------------------------------------------------------------------------
char *
Klein::parameterName(long index)
{
#ifdef KLEIN_DEBUG
	dbf << "param name " << index << endl;
#endif
	switch (index) {
		case kIdMasterGain: return "MasterGain";
		default:
			int trackid = track4Idx(index);
			if (trackid >= 0 && trackid < kMaxTrack && trackid < track.size()) {
				switch (trackParamId4Idx(index)) {
				case kIdTrackInputGain: return "Input";
				case kIdTrackGain:		return "Gain";
				case kIdTrackPan:		return "Pan";
				case kIdTrackFeedback:	return "Feedback";
				}
			}
			
	}
	return "";
}

void Klein::getParameterName(VstInt32 index, char *label)
{
	sprintf(label, " %10s", parameterName(index));
}

bool Klein::getParameterProperties(VstInt32 index, VstParameterProperties * p)
{
#ifdef KLEIN_DEBUG
	dbf << "getParameterProperties " << index << endl;
#endif
	return false;
	/*
	switch (index) {
	case kIdMasterGain: {
		return "MasterGain";
	}
	default:
		int trackid = track4Idx(index);
		if (trackid >= 0 && trackid < kMaxTrack && trackid < track.size()) {
			switch (trackParamId4Idx(index)) {
			case kIdTrackInputGain: return "Input";
			case kIdTrackGain:		return "Gain";
			case kIdTrackPan:		return "Pan";
			case kIdTrackFeedback:	return "Feedback";
			}
		}

	}
	return false;*/
}

//------------------------------------------------------------------------
void Klein::getParameterDisplay(VstInt32 index, char *text)
{
#ifdef KLEIN_DEBUG
	dbf << "param display " << index << endl;
#endif
	switch (index) {
	case kIdMasterGain: dB2string(masterGain, text, 10); break;
	default:
		int trackid = track4Idx(index);
		if (trackid >= 0 && trackid < kMaxTrack && trackid < track.size()) {
			switch (trackParamId4Idx(index)) {
			case kIdTrackInputGain: dB2string(track[trackid]->getInputGain(), text, 10); break;
			case kIdTrackGain:		dB2string(track[trackid]->getOutputGain(), text, 10); break;
			case kIdTrackPan:		float2string(track[trackid]->getPan(), text, 10); break;
			case kIdTrackFeedback:	dB2string(track[trackid]->getFeedback(), text, 10); break;
			}
		}

	}
}

//------------------------------------------------------------------------
void Klein::getParameterLabel(VstInt32 index, char *label)
{
#ifdef KLEIN_DEBUG
	dbf << "param label " << index << endl;
#endif
	switch (index) {
	case kIdMasterGain: strcpy(label, "   dB   ");	break;
	default:
		int trackid = track4Idx(index);
		if (trackid >= 0 && trackid < kMaxTrack && trackid < track.size()) {
			switch (trackParamId4Idx(index)) {
			case kIdTrackInputGain:  strcpy(label, "   dB   ");	break;
			case kIdTrackGain:		 strcpy(label, "   dB   ");	break;
			case kIdTrackPan:		 strcpy(label, "        ");	break;
			case kIdTrackFeedback:	 strcpy(label, "   dB   ");	break;
			}
		}
	}
}


// midi program names:
// as an example, GM names are used here. in fact, VstXSynth doesn't even support
// multi-timbral operation so it's really just for demonstration.
// a 'real' instrument would have a number of voices which use the
// programs[channelProgram[channel]] parameters when it receives
// a note on message.

//------------------------------------------------------------------------
VstInt32 Klein::getMidiProgramName(VstInt32 channel, MidiProgramName* mpn)
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
VstInt32 Klein::getCurrentMidiProgram(VstInt32 channel, MidiProgramName* mpn)
{/*
 if (channel < 0 || channel >= 16 || !mpn)
 return -1;
 long prg = channelPrograms[channel];
 mpn->thisProgramIndex = prg;
 fillProgram(channel, prg, mpn);*/
	return 0;
}


//------------------------------------------------------------------------
VstInt32 Klein::getMidiProgramCategory(VstInt32 channel, MidiProgramCategory* cat)
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

VstInt32 Klein::getChunk(void** data, bool isPreset) {
	return 0;
}

VstInt32 Klein::setChunk(void* data, VstInt32 byteSize, bool isPreset) {
	return 0;
}


/**************************************************************************************
 * VST AUDIO HOOKS
 *************************************************************************************/
// @deprecated
void Klein::process (float **inputs, float **outputs, VstInt32 nFrames)
{
#ifndef MINIMAL_TEST
	float cvu = vu;
	long currentOutFrame = 0;
	while (currentOutFrame < nFrames) {
		const VstTimeInfo *t = getTimeInfo(kVstTempoValid);
		long framesThisChunk = nFrames - currentOutFrame;
		for (unique_ptr<KleinTrack> &ti : track) {
			long l = ti->boringFrames(t, currentOutFrame);
			if (l > 0 && l < framesThisChunk) {
				framesThisChunk = l;
			}
		}
		for (unique_ptr<KleinTrack> &ti : track) {
			const long framesHandled = ti->processAdding(inputs, outputs, currentOutFrame, nFrames);
			float tvu = ti->getVu();
			if (tvu > cvu) {
				cvu = tvu;
			}
		}
		currentOutFrame += framesThisChunk;
	}
	vu = cvu;
#endif
}


/**
 *
 */
void Klein::processReplacing (float **inputs, float **outputs, VstInt32 nFrames)
{
#ifndef MINIMAL_TEST
#if KLEIN_DEBUG >= 10
	dbf << "process replacing " << nFrames << endl;
#endif
	for (int i = 0; i < nOutputPort * 2; ++i) {
		memset(outputs[i], 0, nFrames * sizeof(float));
	}
	
	for (InputInfo& it : input) {
		if (it.thruMode != InputInfo::NO_THRU && it.thruTrack >= 0 && it.thruTrack < track.size()) {
			int ip = it.pin;
			int op = 2*track[it.thruTrack]->getOutPort();
			if (op < nOutputPort) {
				for (int i = 0; i < nFrames; i++) {
					outputs[op][i] += inputs[ip][i];
					outputs[op+1][i] += inputs[ip+1][i];
				}
			}
		}
	}
	// add currently playing track loops to outputs
	// if recording, insert current input to appropriate sample in recording track
	float cvu = vu;
	long currentOutFrame = 0;
	while (currentOutFrame < nFrames) {
#if KLEIN_DEBUG >= 11
		dbf << "process replacing loop currentOutFrame " << currentOutFrame << endl;
#endif
		const VstTimeInfo *t = getTimeInfo(kVstTempoValid);
		long framesThisChunk = nFrames-currentOutFrame;
		for (unique_ptr<KleinTrack> &ti: track) {
			long l = ti->boringFrames(t, currentOutFrame);
			if (l > 0 && l < framesThisChunk) {
				framesThisChunk = l;
			}
#if KLEIN_DEBUG >= 12
			dbf << "process replacing 3 framesThisChunk " << framesThisChunk << ", l " << l << endl;
#endif
		}
#if KLEIN_DEBUG >= 11
		dbf << "process replacing 3 framesThisChunk " << framesThisChunk << endl;
#endif
		for (unique_ptr<KleinTrack> &ti : track) {
			const long framesHandled = ti->processAdding(inputs, outputs, currentOutFrame, nFrames);
			float tvu = ti->getVu();
			if (tvu > cvu) {
				cvu = tvu;
			}
		}
		currentOutFrame += framesThisChunk;
	}
	vu = cvu;
#endif
}

float
Klein::getTempo()
{
	const VstTimeInfo *t = getTimeInfo(kVstTempoValid);
	return t->tempo;
}

void Klein::allocateChildBuffers(long blockSize)
{
	for (unique_ptr<KleinTrack> & i: track) {
		i->allocateBuffers(blockSize);
	}
}

//------------------------------------------------------------------------
void Klein::suspend()
{
	nChunkFrames = 0;	// resets lfo check
}

//------------------------------------------------------------------------
float Klein::getVu()
{
	float cvu = vu;

	vu = 0;
	return cvu;
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

VstInt32 Klein::canDo(char* text)
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


/**
 * VST hook for pin propertes. We're assuming for the moment that it's stereo in and stereo out
 * but we are going to open that assumption up eventually, so we could have mono ins, or we could
 * split a stereo input
 */
bool Klein::getInputProperties(long index, VstPinProperties* properties)
{
	bool ret = false;
	InputInfo *ip = getInputInfo(index);
	if (ip != nullptr) {
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


/**
 * VST hook for pin propertes. outputs are stereo pairs
 */
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

VstInt32 Klein::getVendorVersion() {
	return 1;
}


//-----------------------------------------------------------------------------------------
VstInt32 Klein::processEvents(VstEvents* ev)
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

bool Klein::getMidiKeyName(VstInt32 channel, MidiKeyName* key)
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
bool Klein::hasMidiProgramsChanged(VstInt32 channel)
{
	return false;	// updateDisplay ()
}

/**************************************************************************************
 * CONFIGURATION
 *************************************************************************************/

status_t
Klein::loadConfig(const char *path, vector<string> &errorList)
{
	tinyxml2::XMLDocument doc;

	tinyxml2::XMLError err = doc.LoadFile(path);
	if (err != tinyxml2::XML_NO_ERROR) {
		errorList.push_back(doc.GetErrorStr1());
		return err;
	}
	tinyxml2::XMLElement* root = doc.RootElement();

	string namestr = root->Value();
	if (namestr == "klein") {
#ifdef KLEIN_DEBUG
		dbf << "xml config: got root" << endl;
#endif
		const char *inputPortAttrVal = root->Attribute("nInputPort");
		const char *outputPortAttrVal = root->Attribute("nOutputPort");
		if (inputPortAttrVal) nInputPort =  atoi(inputPortAttrVal);
		if (outputPortAttrVal) nOutputPort = atoi(outputPortAttrVal);

		tinyxml2::XMLElement *childElement = root->FirstChildElement();
		while (childElement != nullptr) {
			std::string childName = childElement->Value();
#ifdef KLEIN_DEBUG
			dbf << "xml config: main got child " << childName << endl;
#endif
			if (childName == "tracks") {
				err = loadTrackConfig(childElement, errorList);
				tracksSetupDone = true;
			} else if (childName == "inputs") {
				err = loadInputConfig(childElement, errorList);
				inputSetupDone = true;
			} else if (childName == "scripts") {
				err = loadScriptConfig(childElement, errorList);
			} else if (childName == "midiMap") {
				err = loadMidiMapConfig(childElement, errorList);
			}
			if (err != tinyxml2::XML_NO_ERROR) {
				errorList.push_back(doc.GetErrorStr1());
			}
			childElement = childElement->NextSiblingElement();
		}

	} else {
		errorList.push_back("Expected 'klein' as root element");
		return KF_ERROR;
	}
	return errorList.size() > 0;
}

tinyxml2::XMLError Klein::loadTrackConfig(tinyxml2::XMLElement *element, vector<string> &errorList) {
	

	const char *trackAttrVal = element->Attribute("nTracks");
	const char *loopAttrVal = element->Attribute("nLoopsPerTracks");
	tinyxml2::XMLElement *childElement = element->FirstChildElement();
	if (trackAttrVal) {
		nTracks = atoi(trackAttrVal);
	} else {
		nTracks = 8;
	}
	if (loopAttrVal) {
		nLoopsPerTrack = atoi(loopAttrVal);
	} else {
		nLoopsPerTrack = 4;
	}
	track.clear();
	int i = 0;
	int nTrackAdded = 0;
	while (childElement != nullptr) {
		std::string childName = childElement->Value();
#ifdef KLEIN_DEBUG
		dbf << "xml config: track config got " << childName << endl;
#endif
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

#ifdef KLEIN_DEBUG
			dbf << "xml config: adding track " << trackId << ", " << nTrackAdded << endl;
#endif
			track.push_back(make_unique<KleinTrack>(trackId, inPortId, outPortId, nLoopsPerTrack, ss, su));
			++nTrackAdded;
		}
		childElement = childElement->NextSiblingElement();
	}
	setNTracks(nTracks, i);
	return tinyxml2::XML_NO_ERROR;
}

tinyxml2::XMLError Klein::loadInputConfig(tinyxml2::XMLElement *element, vector<string> &errorList) {
	input.clear();
	int i = 0;
	int nInputAdded = 0;
	tinyxml2::XMLElement *childElement = element->FirstChildElement();
	while (childElement != nullptr) {
		std::string childName = childElement->Value();
#ifdef KLEIN_DEBUG
		dbf << "xml config: input config got " << childName << endl;
#endif
		if (childName == "input") {
			const char *pinAttrVal = childElement->Attribute("pin");
			const char *stereoAttrVal = childElement->Attribute("stereo");
			const char *thruTrackAttrVal = childElement->Attribute("thruTrack");
			const char *thruModeAttrVal = childElement->Attribute("thruMode");

			int pinId = pinAttrVal ? atoi(pinAttrVal) : 2*i;
			bool stereo = stereoAttrVal ? (string(stereoAttrVal)=="true") : true;
			InputInfo::ThruMode thruMode = thruModeAttrVal ? InputInfo::findMode(thruModeAttrVal) : InputInfo::ON_SELECT;
			int thruTrack = thruTrackAttrVal ? atoi(thruTrackAttrVal) : -1;

			if (i <= pinId) i = pinId + 1;

#ifdef KLEIN_DEBUG
			dbf << "xml config: adding input " << pinId << ", " << nInputAdded << endl;
#endif
			input.emplace_back(pinId, stereo, thruMode);
			++nInputAdded;
		}
		childElement = childElement->NextSiblingElement();
	}
	setNInput(nInputPort, i);
	return tinyxml2::XML_NO_ERROR;
}

tinyxml2::XMLError Klein::loadScriptConfig(tinyxml2::XMLElement *element, vector<string> &errorList) {
	tinyxml2::XMLElement *childElement = element->FirstChildElement();
	while (childElement != nullptr) {
		std::string childName = childElement->Value();
#ifdef KLEIN_DEBUG
		dbf << "xml config: got script " << childName << endl;
#endif
		if (childName == "script") {
			const char *idAttrVal = childElement->Attribute("id");
			const char *srcAttrVal = childElement->Attribute("src");
			if (idAttrVal && srcAttrVal) {
				status_t err = controller.addScript(atoi(idAttrVal), srcAttrVal);
				if (err != ERR_OK) {
					errorList.push_back(string("Failed to load script ") + " srcAttrVal");
				}
			}
		}

		childElement = childElement->NextSiblingElement();
	}
	return tinyxml2::XML_NO_ERROR;
}

tinyxml2::XMLError Klein::loadMidiMapConfig(tinyxml2::XMLElement *element, vector<string> &errorList) {
	tinyxml2::XMLElement *childElement = element->FirstChildElement();
	while (childElement != nullptr) {
		std::string childName = childElement->Value();
#ifdef KLEIN_DEBUG
		dbf << "xml config: midimap: got child " << childName << endl;
#endif
		const char *channelAttrVal = childElement->Attribute("channel");
		const char *whichAttrVal = childElement->Attribute("which");
		const char *contextAttrVal = childElement->Attribute("context");

		const char *functionAttrVal = childElement->Attribute("function");
		const char *controlAttrVal = childElement->Attribute("control");
		const char *scriptAttrVal = childElement->Attribute("script");
		const char *targetAttrVal = childElement->Attribute("target");

		int channel = channelAttrVal ? atoi(channelAttrVal) : 0;
		int which = whichAttrVal ? atoi(whichAttrVal): 0;

		// TODO XXXX FIXME ???? map function/control/script here into mapping

		tgt_id_t tgt = -1;
		if (targetAttrVal) {
			tgt = atoi(targetAttrVal);
		}
		if (functionAttrVal) {
#ifdef KLEIN_DEBUG
			dbf << "xml config: midimap: got function attr " << functionAttrVal << endl;
#endif
			Command *c = Command::find(functionAttrVal);
			if (c != nullptr) {
				CommandMapping mapping(c->command, tgt); // <<<<<<<<<<<<<<<<<<<<<< ?????
				if (childName == "note") {
					controller.addCommandMapping(mapping, MIDI_NOTE_ON, channel, which);
				}
				else if (childName == "ctrl") {
					controller.addCommandMapping(mapping, MIDI_CTRL, channel, which);
				}
				else if (childName == "prog") {
					controller.addCommandMapping(mapping, MIDI_PROG, channel, which);
				}
			}
		}
		if (controlAttrVal) {
#ifdef KLEIN_DEBUG
			dbf << "xml config: midimap: got control attr " << controlAttrVal << endl;
#endif
			Control *c = Control::find(controlAttrVal);
			if (c != nullptr) {
				ControlMapping mapping(c->control, tgt); 
				if (childName == "note") {
					controller.addControlMapping(mapping, MIDI_NOTE_ON, channel, which);
				}
				else if (childName == "ctrl") {
					controller.addControlMapping(mapping, MIDI_CTRL, channel, which);
				}
				else if (childName == "prog") {
					controller.addControlMapping(mapping, MIDI_PROG, channel, which);
				}
			}
		}
		if (scriptAttrVal) {
#ifdef KLEIN_DEBUG
			dbf << "xml config: midimap: got script attr " << scriptAttrVal << endl;
#endif
			script_id_t id = atoi(scriptAttrVal);
			ScriptMapping mapping(id, tgt); // connect to the actual scripts later: check it all then
			if (childName == "note") {
				controller.addScriptMapping(mapping, MIDI_NOTE_ON, channel, which);
			}
			else if (childName == "ctrl") {
				controller.addScriptMapping(mapping, MIDI_CTRL, channel, which);
			}
			else if (childName == "prog") {
				controller.addScriptMapping(mapping, MIDI_PROG, channel, which);
			}
		}
		childElement = childElement->NextSiblingElement();
	}
	return tinyxml2::XML_NO_ERROR;

}

int Klein::setNTracks(int _nTrack, int id, bool clear)
{
	nTracks = _nTrack;
	if (clear) {
		track.clear();
	}
	int n = nTracks - track.size();
	for (int i = 0; i < n; i++) {
#ifdef KLEIN_DEBUG
		dbf << "xml config: adding track " << track.size() << ", id " << id << endl;
#endif
		track.push_back(make_unique<KleinTrack>(id++, 0, 0, nLoopsPerTrack, SYNC_TRACK, SYNC_LOOP));
	}
	return n;
}

int Klein::setNInput(int _nInput, int pin, bool clear)
{
	nInputPort = _nInput;
	if (clear) {
		input.clear();
	}
	int n = nInputPort - input.size();
	for (int i = 0; i < n; i++) {
#ifdef KLEIN_DEBUG
		dbf << "xml config: adding input " << input.size() << ", pin " << pin << endl;
#endif
		input.emplace_back(pin, true, InputInfo::ON_SELECT);
		pin += 2;
	}
	return n;

}

void Klein::setNLoopsPerTrack(int n)
{
	nLoopsPerTrack = n;
	for (unique_ptr<KleinTrack> &it: track) {
		it->setNLoops(n);
	}
}

void Klein::setBlockSize(VstInt32 blocksize) {
	AudioEffectX::setBlockSize(blocksize);
	allocateChildBuffers(blocksize);
}

KleinTrack & Klein::getTrack(int which)
{
	// TODO: maybe we should put in a guard and exception
	return *track[0];
}

bool Klein::selectTrack(int which)
{
	return false;
}

bool Klein::globalMute()
{
	return false;
}

bool Klein::globalPause()
{
	return false;
}

bool Klein::globalReset()
{
	return false;
}

void Klein::setInPort(const KleinTrack & t, const int port)
{
	t.setInPort(port);
}

void Klein::setOutPort(const KleinTrack & t, const int port)
{
	t.setOutPort(port);
}

InputInfo * Klein::getInputInfo(const int pin)
{
	for (auto it = input.begin(); it != input.end(); ++it) {
		if (it->pin == pin || (it->pin + 1 == pin && it->isStereo)) {
			return  &(*it);
		}
	}
	return nullptr;
}
