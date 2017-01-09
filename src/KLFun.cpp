#include "Command.h"
#include "KLFun.h"

KBlock::KBlock()
{

}

KBlock::~KBlock()
{

}

KListBlock::KListBlock()
{
}

KListBlock::~KListBlock()
{
	clear();
}

int KListBlock::execute(KLFContext & c)
{
	return 0;
}

void KListBlock::add(KBlock *b)
{
	child.push_back(b);
}

void KListBlock::clear()
{
	for (auto it : child) {
		delete it;
	}
	child.clear();
}

KIfBlock::KIfBlock()
	: expression(nullptr), ifBranch(nullptr), elseBranch(nullptr)
{
}

KIfBlock::~KIfBlock()
{
	if (expression) delete expression;
	if (ifBranch) delete ifBranch;
	if (elseBranch) delete elseBranch;
}

int
KIfBlock::execute(KLFContext &c) {
	if (expression != nullptr && expression->evaluate(c)()) {
		if (ifBranch != nullptr) {
			ifBranch->execute(c);
		}
	}
	else {
		if (elseBranch != nullptr) {
			elseBranch->execute(c);
		}

	}
	return 0;
}

int KCtlAssBlock::execute(KLFContext & c)
{
	return 0;
}

int KVarAssBlock::execute(KLFContext & c)
{
	return 0;
}

int KCmdBlock::execute(KLFContext & c)
{
	return 0;
}

KLFValue KBinop::evaluate(KLFContext & c)
{
	return KLFValue();
}

KLFValue KUnop::evaluate(KLFContext & c)
{
	return KLFValue();
}

KLFValue KConstant::evaluate(KLFContext & c)
{
	return KLFValue();
}

KLFValue KRValue::evaluate(KLFContext & c)
{
	return KLFValue();
}

KLFValue KControl::evaluate(KLFContext & c)
{
	return KLFValue();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "ParserDriver.h"
#include <fstream>
#include <boost\filesystem\path.hpp>

using namespace KLF;

ParserDriver klfParser;


KLFun::KLFun(script_id_t _id, const char * const _path)
	: id(_id), path(_path), main(nullptr), loaded(false)
{
}


KLFun::~KLFun()
{
	clear();
}

bool KLFun::doStart(KLFContext & c)
{
	return false;
}


/**
 * return this function to an empty state ready for parsing
 */
status_t KLFun::clear()
{
	if (main != nullptr) {
		delete main;
		main = nullptr;
	}
	return ERR_OK;
}

bool KLFun::setEventHandler(int eventId, KBlock * code)
{
	bool added = false;
	switch (eventId) {
	case KLFEvent::CLICK:
		break;
	case KLFEvent::END_CLICK:
		break;
	case KLFEvent::SUSTAIN:
		break;
	case KLFEvent::END_SUSTAIN:
		break;
	}
	if (!added) {
		delete code;
		return false;
	}
	return true;
}

/**
 * 
 */
status_t KLFun::load(vector<string>& errors)
{
	ifstream infile(path);
	boost::filesystem::path p(path);
	status_t err = klfParser.parse(this, infile, p.leaf().string(), errors);
	if (err == ERR_OK) {
		loaded = true;
	}
	return err;
}
