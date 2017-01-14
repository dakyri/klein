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
	case T_OBJECT: if (value.Object) delete value.Object;
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

void KListBlock::dump(std::ostream & stream, int indent)
{
	for (auto it : child) {
		it->dump(stream, indent+1);
	}

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

void KIfBlock::dump(std::ostream & stream, int indent)
{
	stream << string('\t', indent) << "if ";
	if (expression != nullptr) expression->dump(stream, 0);
	stream << endl;
	if (ifBranch != nullptr) ifBranch->dump(stream, indent+1);
	if (elseBranch != nullptr) {
		stream << string('\t', indent) << "else " << endl;
		elseBranch->dump(stream, indent + 1);
	}
}

int KCtlAssBlock::execute(const Controller &c, const KLFBaseContext &base, const KLFContext &top)
{
	KLFValue v = expression->evaluate(c, base, top);
	c.setControl(control->control, base.target, v.IntValue());
	return 0;
}

void KCtlAssBlock::dump(std::ostream & stream, int indent)
{
	stream << string('\t', indent) << control->scriptName << " = ";
	expression->dump(stream, 0);
	stream << endl;
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

void KVarAssBlock::dump(std::ostream & stream, int indent)
{
	stream << string('\t', indent) << sym->getName() << " = ";
	expression->dump(stream, 0);
	stream << endl;
}

int KCmdBlock::execute(const Controller &c, const KLFBaseContext &base, const KLFContext &top)
{
	c.processCommand(command->command, base.target);
	return 0;
}

void KCmdBlock::dump(std::ostream & stream, int indent)
{
	stream << string('\t', indent) << command->scriptName << endl;
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

void KBinop::dump(std::ostream & stream, int indent)
{
	stream << "(";
	if (left) left->dump(stream, 0);
	stream << " bop ";
	if (right) right->dump(stream, 0);
	stream << ")";
}

void KUnop::dump(std::ostream & stream, int indent)
{
	stream << "unop (";
	if (operand) operand->dump(stream, 0);
	stream << ")";

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

void KConstant::dump(std::ostream & stream, int indent)
{
	stream << value.StringValue();
}

KLFValue KRValue::evaluate(const Controller &c, const KLFBaseContext &base, const KLFContext &top)
{
	KLFContext *sc = top.contextFor(sym);
	if (sc != nullptr) {
		return sc->getValue(sym);
	}
	return KLFValue();
}

void KRValue::dump(std::ostream & stream, int indent)
{
	stream << sym->getName();
}

KLFValue KControl::evaluate(const Controller &c, const KLFBaseContext &base, const KLFContext &top)
{
	return c.getControl(control->control, base.target);
}

void KControl::dump(std::ostream & stream, int indent)
{
	stream << control->scriptName;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "ParserDriver.h"
#include <fstream>
#include <boost\filesystem\path.hpp>

using namespace KLF;

ParserDriver klfParser;


KLFun::KLFun(script_id_t _id, const char * const _path)
	: id(_id), path(_path)
	, main(nullptr), onClick(nullptr), onEndClick(nullptr), onSustain(nullptr), onEndSustain(nullptr)
	, loaded(false), sym(nullptr)
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
	if (onClick != nullptr) {
		delete onClick;
		onClick = nullptr;
	}
	if (onEndClick != nullptr) {
		delete onEndClick;
		onEndClick = nullptr;
	}
	if (onSustain != nullptr) {
		delete onSustain;
		onSustain = nullptr;
	}
	if (onEndSustain != nullptr) {
		delete onEndSustain;
		onEndSustain = nullptr;
	}
	return ERR_OK;
}

bool KLFun::setEventHandler(int eventId, KBlock * code)
{
	bool added = false;
	switch (eventId) {
	case KLFEvent::CLICK:
		if (!onClick) {
			onClick = code;
			added = true;
		}
		break;
	case KLFEvent::END_CLICK:
		if (!onEndClick) {
			onEndClick = code;
			added = true;
		}
		break;
	case KLFEvent::SUSTAIN:
		if (!onSustain) {
			onSustain = code;
			added = true;
		}
		break;
	case KLFEvent::END_SUSTAIN:
		if (!onEndSustain) {
			onEndSustain = code;
			added = true;
		}
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