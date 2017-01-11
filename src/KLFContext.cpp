#include "KLFContext.h"
#include "Sym.h"

KLFContext::KLFContext(Sym *_scope, KLFContext *p)
	: parent(p), child(nullptr), scope(_scope) {
}

KLFContext::~KLFContext() {
	if (child) delete child;
}

/**
* if no child, then add one. if so, return the new top context
*/
KLFContext * KLFContext::thunk(Sym *s) {
	if (child) {
		child->reset();
		return child;
	}
	child = new KLFContext(s, this);
	child->allocateVars();
	return child;
}
/**
* pops our contxt stack. or not, if we have no parent
*/
KLFContext *
KLFContext::unthunk() {
	if (parent == nullptr) {
		return this; // we are the root context
	}
	KLFContext *p = parent;
	delete p->child; // ie delete this node and all children
	p->child = nullptr;
	return p;
}

/**
* restore state of this node to as-new
*/
void KLFContext::reset() {

}

void KLFContext::allocateVars()
{
	if (scope) {
		for (auto it : scope->children) {
			switch (it->type) {
			case T_BOOLEAN:
			case T_INTEGER:
			case T_FLOAT:
			case T_BLOCK:
				vars.push_back(KLFValue(it));
				break;
			}
		}
	}
}

KLFValue KLFContext::getValue(Sym const * sym)
{
	if (sym->scope == nullptr || sym->scope != scope) {
		return KLFValue();
	}
	if (vars.size() > sym->value.contextIdx) {
		return vars[sym->value.contextIdx];
	}
	return KLFValue();
}

void KLFContext::setValue(Sym const * sym, const KLFValue & v)
{
	if (sym->scope == nullptr || sym->scope != scope) {
		return;
	}
	if (vars.size() > sym->value.contextIdx) {
		vars[sym->value.contextIdx] = v;
	}
	/*
	switch (sym->type) {
	case T_BOOLEAN:
		if (vars.size() > sym->value.contextIdx) {
			vars[sym->value.contextIdx].value.Bool = v.BoolValue();
		}
		break;
	case T_INTEGER:
		if (vars.size() > sym->value.contextIdx) {
			vars[sym->value.contextIdx].value.Int = v.IntValue();
		}
		break;
	case T_FLOAT:
		if (vars.size() > sym->value.contextIdx) {
			vars[sym->value.contextIdx].value.Float = v.FloatValue();
		}
		break;
	case T_STRING:
		if (vars.size() > sym->value.contextIdx) {
			*vars[sym->value.contextIdx].value.String = v.FloatValue();
		}
		break;
	case T_TIME:
		if (vars.size() > sym->value.contextIdx) {
			vars[sym->value.contextIdx].value.Time = v.TimeValue();
		}
		break;
	}*/

}

/**
*
*/
KLFContext *
KLFContext::contextFor(Sym *s) const {
	KLFContext *ctx = const_cast<KLFContext*>(this);
	for (; ctx != nullptr; ctx = ctx->parent) {
		if (ctx->scope == s->scope) {
			return ctx;
		}
	}
	return nullptr;
}

