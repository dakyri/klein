#ifndef __MCDRIVER_HPP__
#define __MCDRIVER_HPP__ 1

#include <string>
#include "KLFunLexer.h"
#include "KLFunParser.hpp"
#include "KLFun.h"

namespace KLF {

class ParserDriver{
public:
	ParserDriver();
	virtual ~ParserDriver();

	status_t parse(KLFun *klf, std::istream& inStream, vector<string> &errorList);

	std::ostream& print(std::ostream &stream);
private:
	KLF::KLFunLexer scanner;
	KLF::KLFunParser parser;

protected:
	friend KLF::KLFunParser;

	KLFun *fun;	/** POP to the function we are building into. will retain ownership of all the structurues we build */


// global variables for passing more complex values around while parsing
	/*
	// propertiies of destinations
	int deviceType;

	// properties of channels
	int nAudioIns;
	int nAudioOuts;
	bool midiThru;
	bool audioThru;
	int channelId;

	// properties of functions that can be represented as interface elements
	bool isLocus;
	bool isModal;
	bool isOncer;
	bool isFixed;
	bool isHeld;
	bool isInit;
	bool hasResetVal;
	int resetVal;

	// properties in lists of formal parameters
	int displayMode;
	bool hasRange;
	bool hasInit;
	bool isEnveloped;
	TypedValue minValue;
	TypedValue maxValue;
	TypedValue initValue;

	// properties of vst plugin definitions
	unsigned signature;
	bool isSynthPlugin;
	bool doLoadPlugin;
	bool mapVstParams;
	int numPrograms;
	int numParams;
	string path;
	bool doLoad;

	// properties of clips
	Time at_t;
	Time dur_t;
	Take *take;

	vector<VstPlugin*> vstplugins;
	vector<Schedulable*> schedulables;
	vector<Lambda*> lambdas;

	Qua *uberQua;*/
};

}
#endif
