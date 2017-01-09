#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "KLFunValue.h"

using namespace std;

class Sym {
public:
	Sym(const string &_name, Sym * const _scope, const type_t _type);
	virtual ~Sym();

protected:
	string name;
	Sym *scope;

	type_t type;
	int contextIdx; /** index of our object within the relevant context */

	friend class SymTab;
};

/**
 * symbol table. this maintains ownership of all created symbols
 */
class SymTab {
public:
	SymTab();
	virtual ~SymTab();

	Sym *define(const string &_name, Sym * const _scope, const type_t _type);
	Sym *define(const string &_name, const type_t _type);
	Sym *find(const string &_name, Sym * const _scope);
	Sym *find(const string &_name);
	bool del(const string &_name, Sym * const _scope);
	bool del(Sym *);

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