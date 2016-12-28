#include "KLFun.h"


KLFun::KLFun()
{
}


KLFun::~KLFun()
{
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
KIfBlock::doStart() {
	if (expression != nullptr && expression->doStart()) {
		if (ifBranch != nullptr) {
			ifBranch->doStart();
		}
	}
	else {
		if (elseBranch != nullptr) {
			elseBranch->doStart();
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
