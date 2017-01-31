#pragma once

#include "aeffectx.h"
#include "vstgui.h"

class KleinTrack;
class Klein;

class TrackStatusView : public CView {
public:
	TrackStatusView(const CRect &r, const KleinTrack *t);
	virtual ~TrackStatusView();

	virtual void draw(CDrawContext *context) override;
	void onTimedUpdate(const Klein &k, const VstTimeInfo *t);

protected:
	const KleinTrack *track;
	float radius;
	CPoint center;
	CRect markerRect;
	float currentArcDeg;
};
