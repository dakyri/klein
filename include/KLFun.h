#pragma once

/**
 * The Klein Looper Functional language. KLF script.
 * Keep is simple stupid and get looping working before you go to town on this
 */
#include "mwdefs.h"
#include "KLFunValue.h"
#include "KLFContext.h"
#include "Sym.h"

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

	int virtual execute(const Controller &c, const KLFBaseContext &ctxt, const KLFContext &top) { return 0; }
	KLFValue virtual evaluate(const Controller &c, const KLFBaseContext &ctxt, const KLFContext &top) { return KLFValue(); }
};

class KListBlock: public KBlock {
public:
	KListBlock();
	virtual ~KListBlock();

	int virtual execute(const Controller &c, const KLFBaseContext &ctxt, const KLFContext &top) override;

	void add(KBlock *);
	void clear();
protected:
	vector<KBlock*> child;
};

class KIfBlock : public KBlock {
public:
	KIfBlock();
	virtual ~KIfBlock();

	int virtual execute(const Controller &c, const KLFBaseContext &ctxt, const KLFContext &top) override;

protected:
	KBlock *expression;
	KBlock *ifBranch;
	KBlock *elseBranch;
};

class KCtlAssBlock : public KBlock {
public:
	KCtlAssBlock(Control *c, KBlock *e)
		: control(c), expression(e) {} 
	virtual ~KCtlAssBlock() {
		if (expression) delete expression;
	}

	int virtual execute(const Controller &c, const KLFBaseContext &ctxt, const KLFContext &top) override;

protected:
	Control *control;
	KBlock *expression;
};


class KVarAssBlock : public KBlock {
public:
	KVarAssBlock(Sym *s, KBlock *e)
		: sym(s), expression(e) {}
	virtual ~KVarAssBlock() {
		if (expression) delete expression;
	}

	int virtual execute(const Controller &c, const KLFBaseContext &ctxt, const KLFContext &top) override;

protected:
	Sym *sym;
	KBlock *expression;
};

class KCmdBlock : public KBlock {
public:
	KCmdBlock(Command *c): command(c) {}
	virtual ~KCmdBlock() {
	}

	int virtual execute(const Controller &c, const KLFBaseContext &ctxt, const KLFContext &top) override;

protected:
	Command *command;
};

class KBinop : public KBlock {
public:
	KBinop(op_id_t t, KBlock *l, KBlock *r)
		: cmd(t), left(l), right(r) {}
	virtual ~KBinop() {
		if (left != nullptr) delete left;
		if (right != nullptr) delete right;
	}

	KLFValue virtual evaluate(const Controller &c, const KLFBaseContext &ctxt, const KLFContext &top) override;

protected:
	op_id_t cmd;

	KBlock *left;
	KBlock *right;
};

class KUnop : public KBlock {
public:
	KUnop(op_id_t t, KBlock *o)
		: cmd(t), operand(o) {}
	virtual ~KUnop() {
		if (operand != nullptr) delete operand;
	}

	KLFValue virtual evaluate(const Controller &c, const KLFBaseContext &ctxt, const KLFContext &top) override;

protected:
	op_id_t cmd;

	KBlock *operand;
};

class KConstant : public KBlock {
public:
	KConstant(int i): value(i) {}
	KConstant(float f): value(f) {}
	KConstant(double d): value((float)d) {}
	KConstant(string *s): value(*s) {}
	KConstant(ktime_t s): value(s) {}
	//	KConstant(vector<int> *s) {}

	virtual ~KConstant() {
	}

	KLFValue virtual evaluate(const Controller &c, const KLFBaseContext &ctxt, const KLFContext &top) override;

protected:
	KLFValue value;
};

class KRValue : public KBlock {
public:
	KRValue(Sym *_sym): sym(_sym) {}
	KRValue() {}

	virtual ~KRValue() {
	}

	KLFValue virtual evaluate(const Controller &c, const KLFBaseContext &ctxt, const KLFContext &top) override;

protected:
	Sym *sym;
};

class KControl : public KBlock {
public:
	KControl(Control *c): control(c) {}
	virtual ~KControl() {
	}

	KLFValue virtual evaluate(const Controller &c, const KLFBaseContext &ctxt, const KLFContext &top) override;

protected:
	Control *control;
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

	bool doStart(const Controller &c, const KLFBaseContext &b);
	status_t load(vector<string> &errors);
	status_t unload();
	status_t clear(); /** reset state of blocks and symbols so we can rebuild a compiled script into this */

	bool setEventHandler(int eventId, KBlock *code);

	string &getPath();
	script_id_t getId();

protected:
	string path;
	script_id_t id;
	Sym *sym;

	bool loaded;

	KBlock *main;

	friend class Controller;
};

