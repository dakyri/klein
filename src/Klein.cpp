
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;
#undef DEBUG 
#ifdef DEBUG
FILE	*dbf=NULL;
#endif


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
	{"FeedB", kFeedback},
	{"Direct", kDirectLevel},
	{"Dir Pan", kDirectPan}
};

//----------------------------------------------------------------------------- 

KleinProgram::KleinProgram()
{
	fTapDelay[0] = 1;
	fTapLevel[0] = 0.7;
	fTapChannelSwap[0] = false;
	fTapPan[0] = 0;
	fTapSendDest[0] = 0;


	fFeedback = 0;
	fOut = 0.7;
	fDirectLevel = 1;
	fDirectPan = 0;

	fFilterResonance[0] = 0;
	fFilterFrequency[0] = 0.2;
//	fFilterType[0] = sgfCheesy303;
	fFilterEnable[0] = true;


	fLFORate[0] = 1;
	fLFODepth[0] = 0;
	fLFOType[0] = 0;
	fLFOTarget[0] = kFilter1Frequency;

	strcpy (name, "Init");
}




Klein::Klein(audioMasterCallback audioMaster)
	: AudioEffectX (audioMaster, 16, kNumParams)
	, controller(*this)
	, delayLine(2, 2)
{
//	delayLine = new SGDelayLine(2, MaxSGDelayTime, sampleRate);
	short	i;
	for (i=0; i<NSGDelayTap; i++) {
		tapSig[i] = new float[SGDelayChunkFrames*2];
	}
	for (i=0; i<NSGDelayFilter; i++) {
		lFilterin[i] = new float[SGDelayChunkFrames];
		rFilterin[i] = new float[SGDelayChunkFrames];
	}
	programs = new KleinProgram[numPrograms];

	nChunkFrames = 0;
	nChunkFrameRemaining = 0;

	fTapDelay[0] = 1;
	fTapLevel[0] = 0.7;
	fTapPan[0] = 0;
	fTapChannelSwap[0] = false;
	fTapSendDest[0] = 0;
	fFeedback = 0;
	fOut = 0.7;
	fDirectLevel = 1;
	fDirectPan = 0;

	fFilterResonance[0] = 0;
	fFilterFrequency[0] = 0.2;
//	fFilterType[0] = sgfCheesy303;
	fFilterEnable[0] = true;

	fLFORate[0] = 1;
	fLFODepth[0] = 0;
	fLFOType[0] = 0;
	fLFOTarget[0] = kFilter1Frequency;

	if (programs) {
		setProgram (0);
	}

	vu = 0;

#ifdef DEBUG
	if (!dbf) {
		dbf = fopen("debug", "w");
		setbuf(dbf, NULL);
	}
#endif
	
//	SGLFO::InitializeDefaultWavetables();
	setNumInputs (2);
	setNumOutputs (2);
	hasVu ();
	canProcessReplacing ();
	setUniqueID ('KLYN');

	suspend ();		// flush buffer
#ifdef DEBUG
	fprintf(dbf, "Built the fucking thing %d %g\n", blockSize, sampleRate);
#endif
}

//------------------------------------------------------------------------
Klein::~Klein()
{
#ifdef DEBUG
	if (dbf) {
		fclose(dbf);
		dbf = NULL;
	}
#endif
	short	i;
	if (programs)
		delete[] programs;
	for (i=0; i<NSGDelayTap; i++) {
		if (tapSig[i]) {
			delete [] tapSig[i];
		}
	}
	for (i=0; i<NSGDelayFilter; i++) {
		if (lFilterin[i]) {
			delete [] lFilterin[i];
		}
		if (rFilterin[i]) {
			delete [] rFilterin[i];
		}
	}
}

//------------------------------------------------------------------------
void
Klein::setProgram (long program)
{
	KleinProgram * ap = &programs[program];

	curProgram = program;
	short i;
	for (i=0; i<NSGDelayTap; i++) {
		fTapDelay[i] = ap->fTapDelay[i];	
		fTapPan[i] = ap->fTapPan[i];
		fTapLevel[i] = ap->fTapLevel[i];
		fTapChannelSwap[i] = ap->fTapChannelSwap[i];
		fTapSendDest[i] = ap->fTapSendDest[i];

	}

	fFeedback = ap->fFeedback;
	fOut = ap->fOut;
	fDirectLevel = ap->fDirectLevel;
	fDirectPan = ap->fDirectPan;

	setResonance (0, ap->fFilterResonance[0]);
	setFrequency (0, ap->fFilterFrequency[0]);
	setFilterType(0, ap->fFilterType[0]);
	setFilterEnabled (0, ap->fFilterEnable[0]);
#if (SGDELAY_WHICH >= 3)
	setResonance (1, ap->fFilterResonance[1]);
	setFrequency (1, ap->fFilterFrequency[1]);
	setFilterType(1, ap->fFilterType[1]);
	setFilterEnabled (1, ap->fFilterEnable[1]);
#endif

	for (i=0; i<NSGDelayLFO; i++) {
		setLFRate(i, ap->fLFORate[i]);
		setLFDepth(i, ap->fLFODepth[i]);
		setLFWave(i, ap->fLFOType[i]);
		fLFOTarget[i] = ap->fLFOTarget[i];
	}
}


//------------------------------------------------------------------------
void
Klein::setLFWave (short which, short t)
{
	programs[curProgram].fLFOType[which] = fLFOType[which] = t;
//	lfo[which].Waveform(kvLFWaveformIdx[t].value);
}


//------------------------------------------------------------------------
void
Klein::setLFRate (short which, float t)
{
	programs[curProgram].fLFORate[which] = fLFORate[which] = t;
}

//------------------------------------------------------------------------
void
Klein::setLFDepth (short which, float t)
{
	programs[curProgram].fLFODepth[which] = fLFODepth[which] = t;
}

//------------------------------------------------------------------------
void
Klein::setFrequency (short i, float freq)
{
	programs[curProgram].fFilterFrequency[i] = fFilterFrequency[i] = freq;
//	lfilt[i].Param0(freq);
//	rfilt[i] = lfilt[i];
}


//------------------------------------------------------------------------
void
Klein::setResonance (short i, float freq)
{
	programs[curProgram].fFilterResonance[i] = fFilterResonance[i] = freq;
//	lfilt[i].Param1(freq);
//	rfilt[i] = lfilt[i];
}

//------------------------------------------------------------------------
void
Klein::setFilterType (short i, long t)
{
	programs[curProgram].fFilterType[i] = fFilterType[i] = t;
//	lfilt[i].SetType(kvFilter[t].value);
//	rfilt[i] = lfilt[i];
}


//------------------------------------------------------------------------
void
Klein::setFilterEnabled (short i, bool t)
{
	programs[curProgram].fFilterEnable[i] = fFilterEnable[i] = t;
//	lfilt[i].SetEnabled(t);
//	rfilt[i].SetEnabled(t);
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
	delayLine.reset();
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

#ifdef DEBUG
	fprintf(dbf, "set p %d %g\n", index, value);
#endif
	switch (index)
	{
		case kTap1Delay :   fTapDelay[0] = ap->fTapDelay[0] = MaxSGDelayBeats*value;; break;
		case kTap1Level:	fTapLevel[0] = ap->fTapLevel[0] = MaxTapGain * value; break;
		case kTap1Pan:			fTapPan[0] = ap->fTapPan[0] = 2*value-1; break;
		case kTap1ChannelSwap: fTapChannelSwap[0] = ap->fTapChannelSwap[0] = value; break;

		case kFeedback : fFeedback = ap->fFeedback = value; break;

		case kFilter1Type: /*setFilterType(0, kvFilter[round(nSGFilterType*value)].value); */break;
		case kFilter1Frequency: setFrequency(0, value);  break;
		case kFilter1Resonance: setResonance(0, value);  break;
		case kFilter1Enable: setFilterEnabled(0, value != 0);break;

		case kLFO1Rate: fLFORate[0] = ap->fLFORate[0] = MaxSGDelayLFRate*value; break;
		case kLFO1Depth: fLFODepth[0] = ap->fLFODepth[0] = value; break;
		case kLFO1Type: /*fLFOType[0] = ap->fLFOType[0] = kvLFWaveform[round(nSGLFWaveform*value)].value; */setLFWave(0, fLFOType[0]); break;
		case kLFO1Target:/* fLFOTarget[0] = ap->fLFOTarget[0] = kvLFTarget[round(nSGLFTarget*value)].value; */break;

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
		case kTap1Delay: v = fTapDelay[0]/MaxSGDelayBeats; break;
		case kTap1Level: v = fTapLevel[0]/MaxTapGain; break;
		case kTap1Pan:			v = (fTapPan[0]+1)/2; break;
		case kTap1ChannelSwap:	v = fTapChannelSwap[0]; break;

		case kFeedback : v = fFeedback; break;

		case kFilter1Type: /*v = ((float)filterTypeIndex.IndexOf(fFilterType[0]))/nSGFilterType;*/
			break;
		case kFilter1Frequency: v = fFilterFrequency[0]; break;
		case kFilter1Resonance: v = fFilterResonance[0]; break;
		case kFilter1Enable: v = fFilterEnable[0]; break;
		case kLFO1Rate:		v = fLFORate[0]/MaxSGDelayLFRate; break;
		case kLFO1Depth:		v = fLFODepth[0]; break;
		case kLFO1Type:		/*v = ((float)lfWaveformIndex.IndexOf(fLFOType[0]))/nSGLFWaveform; */break;
		case kLFO1Target:	/*v = ((float)lfTargetIndex.IndexOf(fLFOTarget[0]))/nSGLFTarget;*/ break;

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
		case kTap1Delay :    return "Delay";
		case kTap1Level:return "Volume";
		case kTap1Pan:				return "Pan";
		case kTap1ChannelSwap:		return "Chan Swap";
		case kTap1SendDest:		return "Send Dest";
		case kFeedback : return "Feedback";
		case kFilter1Type:		return "Filter";
		case kFilter1Frequency:	return "Frequency";
		case kFilter1Resonance:	return "Resonance";
		case kFilter1Enable:		return "Enable";
		case kLFO1Rate:			return "LF1 Rate";
		case kLFO1Depth:		return "LF1 Depth";
		case kLFO1Type:			return "LF1 Type";
		case kLFO1Target:		return "LF1 Target";
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
		case kTap1Delay :    float2string (fTapDelay[0], text); break;
		case kTap1Level: float2string (fTapLevel[0], text); break;
		case kTap1Pan:				float2string (fTapPan[0], text);break;
		case kTap1ChannelSwap: strcpy(text, fTapChannelSwap[0]? "on":"off"); break;break;
		case kFeedback : float2string (fFeedback, text);	break;

		case kFilter1Type: /* strcpy(text, lfilt[0].FilterName(fFilterType[0]));*/ break;
		case kFilter1Frequency:	float2string (fFilterFrequency[0], text);break;
		case kFilter1Resonance:	float2string (fFilterResonance[0], text);break;
		case kFilter1Enable: strcpy(text, fFilterEnable[0]? "on":"off"); break;

		case kLFO1Rate:			float2string (fLFORate[0], text);break;
		case kLFO1Depth:			float2string (fLFODepth[0], text); break;
		case kLFO1Type:  /* strcpy(text, SGLFO::WfName(fLFOType[0]));*/ break;
		case kLFO1Target:/*
			if (fLFOTarget[0] < nSGLFTarget) {
				strcpy(text, parameterName(kvLFTarget[fLFOTarget[0]].value));
			} else {
				strcpy(text, "?");
			}*/
			break;

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
		case kTap1Delay :		strcpy (label, "  beats ");	break;
		case kTap1Level:		strcpy (label, "        "); break;
		case kTap1Pan:			strcpy (label, "        "); break;
		case kTap1ChannelSwap:	strcpy (label, "        "); break;
		case kFeedback :		strcpy (label, " amount ");	break;

		case kFilter1Type:		strcpy (label, "        "); break;
		case kFilter1Frequency:	strcpy (label, "        "); break;
		case kFilter1Resonance:	strcpy (label, "        "); break;
		case kFilter1Enable:	strcpy (label, "        "); break;


		case kLFO1Rate:			strcpy (label, "  beats "); break;
		case kLFO1Depth:
		case kLFO1Type:
		case kLFO1Target:		strcpy (label, "        "); break;
		case kOut : 
		case kDirectLevel : 
		case kDirectPan :		strcpy (label, "   dB   ");	break;
	}
}

void
Klein::LFOCheck(short which)
{
	if (fLFODepth[which] > 0) { // !! not lfd which may well zero
//		lfo[which].Frequency(SGDelayChunkFrames/ (beatT*((fLFORate[which]>0)?fLFORate[which]:1.0)));
		float lv = lfd[which]/* *lfo[which]()*/;
		switch(fLFOTarget[which]) {
			case kTap1Delay: {
				float p = fTapDelay[0]*lv;
				p += fTapDelay[0];
				if (p > 8) p = 8;
				else if (p < 0.005) p = 0.005;
				delayT[0] = beatT*p;
				break;
			}
			case kTap1Level: {
				float p = lv;
				p += fTapLevel[0];
				if (p > 2) p = 2;
				else if (p < 0) p = 0;
				lGain[0] = p*(1-fTapPan[0]);
				rGain[0] = p*(1+fTapPan[0]);
				break;
			}
			case kTap1Pan: { 
				float p = lv;
				p += fTapPan[0];
				if (p > 1) p = 1;
				else if (p < -1) p = -1;
				lGain[0] = fTapLevel[0]*(1-p);
				rGain[0] = fTapLevel[0]*(1+p);
				break;
			}

			case kFeedback: {
				float p = 0.5*lv;
				p += fFeedback;
				if (p > 1) p = 1;
				else if (p < -1) p = -1;
				feedback = p;
				break;
			}
			case kDirectLevel: {
				float p = lv;
				p += kDirectLevel;
				if (p > 2) p = 2;
				else if (p < 0) p = 0;
				directL = p*(1-fDirectPan);
				directR = p*(1+fDirectPan);
				break;
			}
			case kDirectPan: { 
				float p = lv;
				p += fDirectPan;
				if (p > 1) p = 1;
				else if (p < -1) p = -1;
				lGain[3] = fDirectLevel*(1-p);
				rGain[3] = fDirectLevel*(1+p);
				break;
			}
			case kFilter1Frequency: {
				float p = 0.5*lv;
				p += fFilterFrequency[0];
				if (p > 1) p = 1;
				else if (p < 0) p = 0;
//				lfilt[0].Param0(p);
//				rfilt[0] = lfilt[0];
				break;
			}
			case kFilter1Resonance: {
				float p = 0.5*lv;
				p += fFilterResonance[0];
				if (p > 1) p = 1;
				else if (p < 0) p = 0;
//				lfilt[0].Param1(p);
//				rfilt[0] = lfilt[0];
				break;
			}
		}
	}
}

//------------------------------------------------------------------------
void Klein::process (float **inputs, float **outputs, long nFrames)
{
	float	*inl= inputs[0];
	float	*inr= inputs[1];
	float	*outl = outputs[0];
	float	*outr = outputs[1];
	float	cvu = vu;

	beatT = 60/getTempo();
	if (fTapLevel[0] > 0) {
		delayT[0] = beatT * fTapDelay[0];// delay in secs
		lGain[0] = fTapLevel[0]*(1-fTapPan[0]);
		rGain[0] = fTapLevel[0]*(1+fTapPan[0]);
	} else {
		lGain[0] = rGain[0] = 0;
	}
	directL = fDirectLevel*(1-fDirectPan);
	directR = fDirectLevel*(1+fDirectPan);


	for (short i=0; i<NSGDelayLFO; i++) {
		lfd[i] = fLFODepth[i];
	}
	feedback = fFeedback;
	
#if (DEBUG >= 10)
		fprintf(dbf, "process replacing nf %d %d %d flt %d %d gain %g %g\n",
				nFrames, SGDelayChunkFrames, nFrames/SGDelayChunkFrames,
				lfilt[0].type, rfilt[0].type, lGain[0], rGain[0]);
#endif
	long	outFrame = 0;

	while (outFrame < nFrames) {

		short j;
		if (nChunkFrameRemaining == 0) {
			LFOCheck(0);
			nChunkFrames = nFrames - outFrame;
			if (nChunkFrames > SGDelayChunkFrames) {
				nChunkFrames = SGDelayChunkFrames;
			}
			nChunkFrameRemaining = SGDelayChunkFrames;
		} else {
			nChunkFrames = nChunkFrameRemaining;
		}

//		delayLine.write(inl, inr, nChunkFrames, 1-feedback, false);


		delayLine.read(tapSig[0], nChunkFrames, delayT[0]);
		float	*pl, *pr;
		if (fTapChannelSwap[0]) {
			pr = tapSig[0];
			pl = tapSig[0]+1;
		} else {
			pl = tapSig[0];
			pr = tapSig[0]+1;
		}
		for (j=0; j<nChunkFrames; j++) {
//			outl[j] += inl[j] + lfilt[0](*pl)*lGain[0];
//			outr[j] += inr[j] + rfilt[0](*pr)*rGain[0];
			if (outl[j] > cvu) cvu = outl[j];
			if (outr[j] > cvu) cvu = outr[j];
			pl+=2;pr+=2;
		}
//		delayLine.overdub(outl, outr, nChunkFrames, feedback, true);

		outFrame += nChunkFrames;
		outl += nChunkFrames;
		outr += nChunkFrames;
		inl += nChunkFrames;
		inr += nChunkFrames;

		nChunkFrameRemaining -= nChunkFrames;
	}

	vu = cvu;
}


//---------------------------------------------------------------------------
void Klein::processReplacing (float **inputs, float **outputs, long nFrames)
{
	float	*inl= inputs[0];
	float	*inr= inputs[1];
	float	*outl = outputs[0];
	float	*outr = outputs[1];
	float	cvu = vu;

	beatT = 60/getTempo();
	if (fTapLevel[0] > 0) {
		delayT[0] = beatT * fTapDelay[0];// delay in secs
		lGain[0] = fTapLevel[0]*(1-fTapPan[0]);
		rGain[0] = fTapLevel[0]*(1+fTapPan[0]);
	} else {
		lGain[0] = rGain[0] = 0;
	}
	directL = fDirectLevel*(1-fDirectPan);
	directR = fDirectLevel*(1+fDirectPan);


	for (short i=0; i<NSGDelayLFO; i++) {
		lfd[i] = fLFODepth[i];
	}

	feedback = fFeedback;
	
#if (DEBUG >= 10)
		fprintf(dbf, "process replacing nf %d %d %d flt %d %d gain %g %g\n",
				nFrames, SGDelayChunkFrames, nFrames/SGDelayChunkFrames,
				lfilt[0].type, rfilt[0].type, lGain[0], rGain[0]);
#endif
	long	outFrame = 0;

	while (outFrame < nFrames) {

		short j;
		if (nChunkFrameRemaining == 0) {
			LFOCheck(0);
			nChunkFrames = nFrames - outFrame;
			if (nChunkFrames > SGDelayChunkFrames) {
				nChunkFrames = SGDelayChunkFrames;
			}
			nChunkFrameRemaining = SGDelayChunkFrames;
		} else {
			nChunkFrames = nChunkFrameRemaining;
		}

//		delayLine->Write(inl, inr, nChunkFrames, 1-feedback, false);


//		delayLine->Read(tapSig[0], nChunkFrames, delayT[0]);
		float	*pl, *pr;
		if (fTapChannelSwap[0]) {
			pr = tapSig[0];
			pl = tapSig[0]+1;
		} else {
			pl = tapSig[0];
			pr = tapSig[0]+1;
		}
		for (j=0; j<nChunkFrames; j++) {
//			outl[j] = inl[j] + lfilt[0](*pl)*lGain[0];
//			outr[j] = inr[j] + rfilt[0](*pr)*rGain[0];
			if (outl[j] > cvu) cvu = outl[j];
			if (outr[j] > cvu) cvu = outr[j];
			pl+=2;pr+=2;
		}

//		delayLine->Overdub(outl, outr, nChunkFrames, feedback, true);

		outFrame += nChunkFrames;
		outl += nChunkFrames;
		outr += nChunkFrames;
		inl += nChunkFrames;
		inr += nChunkFrames;

		nChunkFrameRemaining -= nChunkFrames;
	}

	vu = cvu;
}

float
Klein::getTempo()
{
	const VstTimeInfo *t = getTimeInfo(kVstTempoValid);
	return t->tempo;
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
	/*
	if (index < kNumOutputs)
	{
		sprintf(properties->label, "Vstx %1d", index + 1);
		properties->flags = kVstPinIsActive;
		if (index < 2)
			properties->flags |= kVstPinIsStereo;	// make channel 1+2 stereo
		return true;
	}*/
		return false;
}

bool Klein::getOutputProperties(long index, VstPinProperties* properties)
{
	/*
	if (index < kNumOutputs)
	{
	sprintf(properties->label, "Vstx %1d", index + 1);
	properties->flags = kVstPinIsActive;
	if (index < 2)
	properties->flags |= kVstPinIsStereo;	// make channel 1+2 stereo
	return true;
	}*/
	return false;
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
