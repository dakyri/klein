#pragma once

/**
 * The Klein Looper Functional language. KLF script.
 * Keep is simple stupid and get looping working before you go to town on this
 */
#include "mwdefs.h"
#include "KLFunValue.h"
#include "KLFContext.h"

#include <string>
#include <vector>

using namespace std;
struct Control;

enum KLFEvent {
	CLICK,
	END_CLICK,
	SUSTAIN,
	END_SUSTAIN
};

typedef int op_id_t;

/**
 * a piece of KLF
 */
class KBlock {
public:
	KBlock();
	virtual ~KBlock();

	int virtual doStart(KLFContext &c) { return 0; }
	KLFValue virtual evaluate(KLFContext &c) { return KLFValue(); }
};

class KIfBlock : public KBlock {
public:
	KIfBlock();
	virtual ~KIfBlock();

	int virtual doStart(KLFContext &c) override;

protected:
	KBlock *expression;
	KBlock *ifBranch;
	KBlock *elseBranch;
};

class KAssBlock : public KBlock {
public:
	KAssBlock(KBlock *lv, KBlock *e)
		: lvalue(lv), expression(e) {} 
	virtual ~KAssBlock() {
		if (lvalue) delete lvalue;
		if (expression) delete expression;
	}

	int virtual doStart(KLFContext &c) override;

protected:
	KBlock *lvalue;
	KBlock *expression;
};


class KBinop : public KBlock {
public:
	KBinop(op_id_t t, KBlock *l, KBlock *r)
		: cmd(t), left(l), right(r) {}
	virtual ~KBinop() {
		if (left != nullptr) delete left;
		if (right != nullptr) delete right;
	}

	KLFValue virtual evaluate(KLFContext &c) override;

protected:
	op_id_t cmd;

	KBlock *left;
	KBlock *right;
};

class KUnop : public KBlock {
public:
	KUnop(op_id_t t, KBlock *o)
		: cmd(t), op(o) {}
	virtual ~KUnop() {
		if (op != nullptr) delete op;
	}

	KLFValue virtual evaluate(KLFContext &c) override;

protected:
	op_id_t cmd;

	KBlock *op;
};

class KConstant : public KBlock {
public:
	KConstant(int i) {}
	KConstant(float f) {}
	KConstant(double d) {}
	KConstant(string *s) {}
	KConstant(vector<int> *s) {}

	virtual ~KConstant() {
	}

	KLFValue virtual evaluate(KLFContext &c) override;

protected:
};

class KRValue : public KBlock {
public:
	KRValue(string *) {}
	KRValue() {}

	virtual ~KRValue() {
	}

	KLFValue virtual evaluate(KLFContext &c) override;

protected:
};

class KControl : public KBlock {
public:
	KControl(Control *) {}
	virtual ~KControl() {
	}

	KLFValue virtual evaluate(KLFContext &c) override;

protected:
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
	KLFun(script_id_t _id, const char * const _path);
	virtual ~KLFun();

	bool doStart(KLFContext &c);
	status_t load(vector<string> &errors);
	status_t clear(); /** reset state of blocks and symbols so we can rebuild a compiled script into this */

	string &getPath();
	script_id_t getId();

protected:
	string path;
	script_id_t id;

	bool loaded;

	KBlock *main;

	friend class Controller;
};

