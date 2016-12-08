#ifndef __MCDRIVER_HPP__
#define __MCDRIVER_HPP__ 1

#include <string>
#include "MScriptLexer.h"
#include "MScriptParser.hpp"

namespace MSParse {

class ParserDriver{
public:
	ParserDriver();
	virtual ~ParserDriver();

	void parse(std::istream& inStream);

	std::ostream& print(std::ostream &stream);
private:
	MSParse::MScriptLexer scanner;
	MSParse::MScriptParser parser;

protected:
	friend MSParse::MScriptParser;

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
