#include "Command.h"
#include "Sym.h"
#include "KLFun.h"
#include "Controller.h"

KLFValue
KLFValue::forType(type_t t) {
	KLFValue v;
	v.Type = T_UNDEFINED;
	v.value.Int = 0;

	switch (t) {
	case T_INTEGER: v.Type = T_INTEGER; return v;
	case T_FLOAT: v.Type = T_FLOAT; return v;
	case T_BOOLEAN: v.Type = T_BOOLEAN; return v;
	case T_TIME: v.Type = T_TIME; return v;
	case T_BLOCK: v.Type = T_BLOCK; return v;
	case T_STRING: v.Type = T_STRING; v.value.String = new string(); return v;
	}
	return v;
}

KLFValue::~KLFValue()
{
	switch (Type) {
	case T_INTEGER: break;
	case T_FLOAT:  break;
	case T_BOOLEAN: break;
	case T_TIME:  break;
	case T_BLOCK:  break;
	case T_STRING: if (value.String)  delete value.String; break;
	}

}

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

int KListBlock::execute(const Controller &c, const KLFBaseContext &ctxt, const KLFContext &top)
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
KIfBlock::execute(const Controller &c, const KLFBaseContext &base, const KLFContext &top) {
	if (expression != nullptr && expression->evaluate(c, base, top)()) {
		if (ifBranch != nullptr) {
			ifBranch->execute(c, base, top);
		}
	} else {
		if (elseBranch != nullptr) {
			elseBranch->execute(c, base, top);
		}
	}
	return 0;
}

int KCtlAssBlock::execute(const Controller &c, const KLFBaseContext &base, const KLFContext &top)
{
	KLFValue v = expression->evaluate(c, base, top);
	c.setControl(control->control, base.target, v.IntValue());
	return 0;
}

int KVarAssBlock::execute(const Controller &c, const KLFBaseContext &base, const KLFContext &top)
{
	KLFValue v = expression->evaluate(c, base, top);
	KLFContext *sc = top.contextFor(sym);
	if (sc != nullptr) {
		sc->setValue(sym, v);
	}
	return 0;
}

int KCmdBlock::execute(const Controller &c, const KLFBaseContext &base, const KLFContext &top)
{
	c.processCommand(command->command, base.target);
	return 0;
}

KLFValue KBinop::evaluate(const Controller &c, const KLFBaseContext &base, const KLFContext &top)
{
	switch (cmd) {
	case KLF::KLFunParser::token_type::PLUS: {
		KLFValue vl = left->evaluate(c, base, top);
		KLFValue vr = right->evaluate(c, base, top);
		return vl + vr;
	}
	case KLF::KLFunParser::token_type::MINUS: {
		KLFValue vl = left->evaluate(c, base, top);
		KLFValue vr = right->evaluate(c, base, top);
		return vl - vr;
	}
	case KLF::KLFunParser::token_type::MULT: {
		KLFValue vl = left->evaluate(c, base, top);
		KLFValue vr = right->evaluate(c, base, top);
		return vl * vr;
	}
	case KLF::KLFunParser::token_type::MOD: {
		KLFValue vl = left->evaluate(c, base, top);
		KLFValue vr = right->evaluate(c, base, top);
		return vl % vr;
	}
	case KLF::KLFunParser::token_type::DIVIDE: {
		KLFValue vl = left->evaluate(c, base, top);
		KLFValue vr = right->evaluate(c, base, top);
		return vl / vr;
	}
	case KLF::KLFunParser::token_type::LT: {
		KLFValue vl = left->evaluate(c, base, top);
		KLFValue vr = right->evaluate(c, base, top);
		return vl < vr;
	}
	case KLF::KLFunParser::token_type::GT: {
		KLFValue vl = left->evaluate(c, base, top);
		KLFValue vr = right->evaluate(c, base, top);
		return vl > vr;
	}
	case KLF::KLFunParser::token_type::NE: {
		KLFValue vl = left->evaluate(c, base, top);
		KLFValue vr = right->evaluate(c, base, top);
		return vl != vr;
	}
	case KLF::KLFunParser::token_type::EQ: {
		KLFValue vl = left->evaluate(c, base, top);
		KLFValue vr = right->evaluate(c, base, top);
		return vl == vr;
	}
	case KLF::KLFunParser::token_type::GE: {
		KLFValue vl = left->evaluate(c, base, top);
		KLFValue vr = right->evaluate(c, base, top);
		return vl >= vr;
	}
	case KLF::KLFunParser::token_type::LE: {
		KLFValue vl = left->evaluate(c, base, top);
		KLFValue vr = right->evaluate(c, base, top);
		return vl <= vr;
	}
	case KLF::KLFunParser::token_type::AND: {
		KLFValue vl = left->evaluate(c, base, top);
		if (!vl.BoolValue()) return KLFValue(false);
		return right->evaluate(c, base, top);
	}
	case KLF::KLFunParser::token_type::OR: {
		KLFValue vl = left->evaluate(c, base, top);
		if (vl.BoolValue()) return KLFValue(true);
		return right->evaluate(c, base, top);
	}
	/*
	case KLF::KLFunParser::token_type::POWER:
		KLFValue vl = left->evaluate(c, base, top);
		KLFValue vr = right->evaluate(c, base, top);
		return vl ^ vr;*/
	}
	return KLFValue(); // integer 0
}

KLFValue KUnop::evaluate(const Controller &c, const KLFBaseContext &base, const KLFContext &top)
{
	switch (cmd) {
	case KLF::KLFunParser::token_type::NEG: {
		KLFValue vl = operand->evaluate(c, base, top);
		return -vl;
	}
	case KLF::KLFunParser::token_type::NOT: {
		KLFValue vl = operand->evaluate(c, base, top);
		return !vl;
	}
	}
	return KLFValue();
}

KLFValue KConstant::evaluate(const Controller &c, const KLFBaseContext &base, const KLFContext &top)
{
	return value;
}

KLFValue KRValue::evaluate(const Controller &c, const KLFBaseContext &base, const KLFContext &top)
{
	KLFContext *sc = top.contextFor(sym);
	if (sc != nullptr) {
		return sc->getValue(sym);
	}
	return KLFValue();
}

KLFValue KControl::evaluate(const Controller &c, const KLFBaseContext &base, const KLFContext &top)
{
	return c.getControl(control->control, base.target);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "ParserDriver.h"
#include <fstream>
#include <boost\filesystem\path.hpp>

using namespace KLF;

ParserDriver klfParser;


KLFun::KLFun(script_id_t _id, const char * const _path)
	: id(_id), path(_path), main(nullptr), loaded(false), sym(nullptr)
{

}


KLFun::~KLFun()
{
	clear();
}

bool KLFun::doStart(const Controller &controller, const KLFBaseContext & ctxt)
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
	string name = p.leaf().string();
	stab.define(name, nullptr, this);
	status_t err = klfParser.parse(this, infile, name, errors);
	if (err == ERR_OK) {
		loaded = true;
	}
	return err;
}

/**
 *
 */
status_t KLFun::unload()
{
	stab.del(sym); // that should clear all our children as well
	sym = nullptr;
	return ERR_OK;
}