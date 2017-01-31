#pragma once

#include "vstgui.h"

class Klein;

class TrackWaveView : public CView {
public:
	TrackWaveView(const CRect &r);
	virtual ~TrackWaveView();

	virtual void draw(CDrawContext *context) override;

	void onTimedUpdate(const Klein &k, const VstTimeInfo *t);
};
