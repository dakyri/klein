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
	table.emplace(h, sym);
	return sym;
}

Sym * SymTab::define(const string & _name, Sym * const scope, const KLFun *script)
{
	Sym *sym = define(_name, scope, KLFType::T_KLF);
	sym->value.klfScript = const_cast<KLFun*>(script);
	return sym;
}

Sym * SymTab::define(const string & _name, const type_t _type)
{
	return define(_name, topScope(), _type);
}

Sym * SymTab::find(const string & name, Sym * const scope)
{
	auto range = table.equal_range(hash(name, scope));
	for (auto it = range.first; it != range.second; ++it) {
		if (it->second->name == name && it->second->scope == scope) {
			return it->second;
		}
	}
 	return nullptr;
}

Sym * SymTab::find(const string & name)
{
	for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
		Sym *s = find(name, *it);
		if (s != nullptr) {
			return s;
		}
	}
	Sym *s = find(name, nullptr);
	return s;
}

bool SymTab::del(const string & name, Sym * const scope, bool delFromScopeChildren)
{
	Sym *deletedSym = nullptr;
	auto range = table.equal_range(hash(name, scope));
	for (auto it = range.first; it != range.second; ++it) {
		if (it->second->name == name && it->second->scope == scope) {
			deletedSym = it->second;
			table.erase(it);
			break;
		}
	}
	if (deletedSym != nullptr) {
		for (auto it = deletedSym->children.begin(); it != deletedSym->children.end(); ++it) {
			del((*it)->name, (*it)->scope, false);
			// we are deleting all own children. our (and descendents) scope cleaned automatically, because we're clearing all the children
		}
		if (delFromScopeChildren && deletedSym->scope) { // but maybe remove from our own parents scope
			deletedSym->scope->children.remove(deletedSym);
		}
		delete deletedSym;
		return true;
	}
	return false;
}

bool SymTab::del(const Sym * sym)
{
	if (sym == nullptr) {
		return false;
	}
	return del(sym->name, sym->scope);
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

/**
 * symbol. most values are stored on the context stack, except maybe functions (T_FUNCTION) and script (T_KLF)
 */
Sym::Sym(const string &_name, Sym * const _scope, const type_t _type)
	: name(_name), scope(_scope), type(_type), allocatedChildren(0)
{
	switch (type) {
	case KLFType::T_FLOAT:
	case KLFType::T_BOOLEAN:
	case KLFType::T_STRING:
	case KLFType::T_OBJECT:
	case KLFType::T_TIME:
	case KLFType::T_INTEGER:
	case KLFType::T_BLOCK:
		if (scope != nullptr) { /** these are all on the stack associated with the symbol's scope */
			value.contextIdx = scope->allocatedChildren++;
		}
		break;

	case KLFType::T_KLF: { /** would be a pointer to a KLFun object */
		break;
	}
	case KLFType::T_FUNCTION: { /** a pointer to whatever function is going to represent scope.
								    maybe some operations will be easier (deletion!!!) if the object is owned by the table
									values are in a union, so smart pointers are out I think */
		break;
	}
	case KLFType::T_UNDEFINED: {
		value.contextIdx = -1;
		break;
	}

	}
	if (scope != nullptr) {
		scope->children.emplace_back(this);
	}
}

Sym::~Sym()
{
	/*
	if (scope != nullptr) {
		scope->children.remove(this);
	}*/
	switch (type) {
	case KLFType::T_FLOAT:
	case KLFType::T_BOOLEAN:
	case KLFType::T_STRING:
	case KLFType::T_OBJECT:
	case KLFType::T_TIME:
	case KLFType::T_INTEGER:
	case KLFType::T_BLOCK:
		break;

	case KLFType::T_KLF:
		break;
	case KLFType::T_FUNCTION:
		break;
	case KLFType::T_UNDEFINED:
		break;

	}

}
