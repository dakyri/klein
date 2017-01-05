#include "KLFun.h"


KLFun::KLFun()
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
