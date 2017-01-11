#pragma once

class KleinTrack;

#include <vector>

#include "KLFunValue.h"
#include "Command.h"

class Sym;
class Controller;
using namespace std;
/**
 * ?? should we implement the stack here ourselves, or use something like an stl list
 */
class KLFContext
{
public:
	KLFContext(Sym *_scope, KLFContext *p = nullptr);
	virtual ~KLFContext();

	KLFContext * thunk(Sym *s);
	KLFContext * unthunk();
	void reset();
	void allocateVars();
	void setValue(Sym const *sym, const KLFValue &v);
	KLFValue getValue(Sym const * sym);
	KLFContext * contextFor(Sym *s) const;

	KLFContext *parent;
	KLFContext *child;
	Sym *scope;

	vector<KLFValue> vars;
};

class KLFBaseContext: public KLFContext
{
public:
	KLFBaseContext(tgt_id_t t = -1)
		: KLFContext(nullptr, nullptr), target(t), attack(0), lastUpdate(0) { }
	virtual ~KLFBaseContext() { }

	tgt_id_t target;

	time_t attack;
	time_t lastUpdate;
	int clickCount;
	int sustainCount;

};