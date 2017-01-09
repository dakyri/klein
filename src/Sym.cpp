#include "Sym.h"

SymTab stab;

SymTab::SymTab()
{
}

SymTab::~SymTab()
{
}

Sym * SymTab::define(const string &_name, Sym * const _scope, const type_t _type)
{
	Sym *sym =  new Sym(_name, _scope, _type);
	long h = hash(sym->name, sym->scope);
	return sym;
}

Sym * SymTab::define(const string & _name, const type_t _type)
{
	return define(_name, topScope(), _type);
}

Sym * SymTab::find(const string & _name, Sym * const _scope)
{
	return nullptr;
}

Sym * SymTab::find(const string & _name)
{
	return nullptr;
}

bool SymTab::del(const string & _name, Sym * const _scope)
{
	return false;
}

bool SymTab::del(Sym *)
{
	return false;
}

Sym * SymTab::topScope()
{
	return scopes.size() > 0 ? *scopes.end() : nullptr;
}

void SymTab::enterScope(Sym *sym)
{
	scopes.push_back(sym);
}

bool SymTab::leaveScope(Sym *sym)
{
	bool ret = false;
	if (scopes.size() > 0) {
		if (*scopes.end() == sym) ret = true;
	}
	scopes.pop_back();
	return ret;
}

void SymTab::clearScope()
{
	scopes.clear();
}

long SymTab::hash(const string name, const Sym* const scope)
{
	long shite = 0;
	short nc = name.length();

	for (short i = 0; i<nc; i++) {
		shite += name[i];
	}
	return (((unsigned)scope & 0xffff) + shite)/* % stabLen*/;
}

Sym::Sym(const string &_name, Sym * const _scope, const type_t _type)
	: name(_name), scope(_scope), type(_type)
{
}

Sym::~Sym()
{
}
