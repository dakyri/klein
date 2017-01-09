#pragma once

class KleinTrack;

#include <vector>

#include "KLFunValue.h"

class KLFContext
{
public:
	KLFContext() { }
	virtual ~KLFContext() { }

	vector<KLFValue> vars;
};

class KLFBaseContext: public KLFContext
{
public:
	KLFBaseContext() : track(nullptr) { }
	virtual ~KLFBaseContext() { }

	KleinTrack *track;
};