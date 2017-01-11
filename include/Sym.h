#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "KLFunValue.h"

using namespace std;

class KLFun;

class Sym {
public:
	Sym(const string &_name, Sym * const _scope, const type_t _type);
	virtual ~Sym();

protected:
	string name;
	Sym *scope;

	type_t type;
	union sym_val_t {
		int contextIdx; /** index of our object within the relevant context */
		KLFun *klfScript; /** POP to the scrupt object we've defined with this symbol */
	} value;

	list<Sym *> children;
	int allocatedChildren; /** number of children on the context stack */

	friend class SymTab;
	friend class KLFContext;
	friend class KLFValue;
};

/**
 * symbol table. this maintains ownership of all created symbols
 */
class SymTab {
public:
	SymTab();
	virtual ~SymTab();

	Sym *define(const string &_name, Sym * const _scope, const type_t _type);
	Sym *define(const string &_name, Sym * const _scope, const KLFun *);
	Sym *define(const string &_name, const type_t _type);
	Sym *find(const string &_name, Sym * const _scope);
	Sym *find(const string &_name);
	bool del(const string &_name, Sym * const _scope, bool delFromScopeChildren=true);
	bool del(const Sym *sym);

	Sym *topScope();
	void enterScope(Sym *sym);
	bool leaveScope(Sym *sym=nullptr); 
	void clearScope();
protected:
	long hash(const string name, const Sym* const scope);

	vector<Sym *> scopes; /** stack of currently active scopes */
	unordered_multimap<long, Sym *> table; /** main symbol table. hash function is not unique. maybe use unique_ptr? */
};

extern SymTab stab;