#pragma once

#include "vstgui.h"

class Controller;
class KleinTrack;

class TrackStrip : public CViewContainer {
public:
	TrackStrip(Controller &c, KleinTrack *t, CCoord x, CCoord y, CFrame *parent);
	virtual ~TrackStrip();

	void setComponents4(Controller &c, KleinTrack *t);
	void clearComponents();
protected:
	Controller &controller;
	KleinTrack *track;
};