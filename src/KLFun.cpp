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
KIfBlock::run() {
	if (expression != nullptr && expression->run()) {
		if (ifBranch != nullptr) {
			ifBranch->run();
		}
	}
	else {
		if (elseBranch != nullptr) {
			elseBranch->run();
		}

	}
	return 0;
}