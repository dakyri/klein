#pragma once

#include "vstgui.h"

class Controller;
class KleinTrack;
class LabelledKnob;

class TrackStrip : public CViewContainer {
public:
	TrackStrip(Controller &c, KleinTrack *t, CCoord x, CCoord y, CFrame *parent);
	virtual ~TrackStrip();

	void setComponents4(Controller &c, KleinTrack *t);
	void clearComponents();

	void displayTrackStatus();
	void displaySampleData();
	void displaySelectedTrack();
	void displaySelectedLoop();
	void displayCurrentLayer();

protected:
	Controller &controller;
	KleinTrack *track;

	vector<LabelledKnob*> knobs;
};