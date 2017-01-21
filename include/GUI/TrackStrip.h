#pragma once

#include "vstgui.h"

class TrackStrip : public CView {
public:
	TrackStrip(KleinTrack *t, CCoord x, CCoord y);
	virtual ~TrackStrip();

};