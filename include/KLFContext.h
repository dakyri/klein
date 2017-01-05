#pragma once

class KleinTrack;

class KLFContext
{
public:
	KLFContext(): track(nullptr) { }
	virtual ~KLFContext() { }

	KleinTrack *track;
};