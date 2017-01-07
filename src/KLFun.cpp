#include "Command.h"
#include "KLFun.h"


KLFun::KLFun(int _id, const char * const _path)
	: id(_id), path(_path), main(nullptr), loaded(false)
{
}


KLFun::~KLFun()
{
}

bool KLFun::doStart(KLFContext & c)
{
	return false;
}

KBlock::KBlock()
{

}

KBlock::~KBlock()
{

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
KIfBlock::doStart(KLFContext &c) {
	if (expression != nullptr && expression->doStart(c)) {
		if (ifBranch != nullptr) {
			ifBranch->doStart(c);
		}
	}
	else {
		if (elseBranch != nullptr) {
			elseBranch->doStart(c);
		}

	}
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

using namespace KLF;

ParserDriver klfParser;

/**
 * return this function to an empty state ready for parsing
 */
status_t KLFun::clear()
{
	if (main != nullptr) {
		delete main;
		main = nullptr;
	}
}

/**
 * 
 */
status_t KLFun::load(vector<string>& errors)
{
	ifstream infile(path);
	status_t err = klfParser.parse(this, infile, errors);
	if (err == ERR_OK) {
		loaded = true;
	}
	return err;
}

