#pragma once

/**
 * The Klein Looper Functional language. KLF script.
 * Keep is simple stupid and get looping working before you go to town on this
 */

#include "KLFunValue.h"

enum KLFEvent {
	CLICK,
	END_CLICK,
	SUSTAIN,
	END_SUSTAIN
};

/**
 * a piece of KLF
 */
class KBlock {
public:
	KBlock();
	virtual ~KBlock();

	int virtual run() { return 0; }
	KLFValue virtual evaluate() { return KLFValue(); }
};

class KIfBlock : public KBlock {
public:
	KIfBlock();
	virtual ~KIfBlock();

	int virtual run();

protected:
	KBlock *expression;
	KBlock *ifBranch;
	KBlock *elseBranch;
};

class KAssBlock : public KBlock {
public:
	KAssBlock();
	virtual ~KAssBlock();

	int virtual run();

protected:
	KBlock *expression;
	KBlock *ifBranch;
	KBlock *elseBranch;
};
/**
* a compiled and loaded KLF
*
* should provide
*  - a map for user defined variables which will be allocated when the script is mapped onto a key, control code or other input
*/
class KLFun
{
public:
	KLFun();
	virtual ~KLFun();

	bool run();

protected:
	KBlock *main;
};

