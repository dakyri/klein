#pragma once

#include "vstgui.h"

class TrackStatusView;
class TrackWaveView;
class Controller;
class KleinTrack;
class LabelledKnob;
class Klein;

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

	void onTimedUpdate(const Klein &k, const VstTimeInfo *t);

	const int kLabelWidth = 30;
	const int kLabelHeight = 30;
	const int kKnobWidth = 60;
	const int kKnobHeight = 63;
	const int kStatusViewWidth = 60;
	const int kStatusViewHeight = 60;
	const int kWaveViewWidth = 260;
	const int kWaveViewHeight = 60;
protected:
	Controller &controller;
	KleinTrack *track;

	TrackStatusView *trackStatusView;
	TrackWaveView *trackWaveView;

	vector<LabelledKnob*> knobs;
};