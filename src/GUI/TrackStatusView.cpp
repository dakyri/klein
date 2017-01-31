#include "GUI/TrackStatusView.h"
#include "KleinTrack.h"

#include "debug.h"

TrackStatusView::TrackStatusView(const CRect & r, const KleinTrack *t)
	: CView(r), track(t), currentArcDeg(360)
{
	float d = min(r.getWidth() - 4, r.getHeight() - 4);
	radius = d / 2;
	center.x = r.left + 2 + radius;
	center.y = r.top + 2 + radius;
	markerRect = CRect(center.x-radius, center.y - radius, center.x + radius, center.y + radius);
	setVisible(true);
	setTransparency(false);
	setDirty();
}

TrackStatusView::~TrackStatusView()
{
}

void TrackStatusView::draw(CDrawContext * context)
{
	context->setDrawMode(kDrawFilled);
	CColor fillColor = kGreyCColor;
	float arc = currentArcDeg;
	switch (track->getMode()) {
	case TRAK_PEND:
		fillColor = kYellowCColor;
		break;
	case TRAK_STOP:
	case TRAK_PAUSE:
		fillColor = kGreyCColor;
		break;
	case TRAK_PLAY:
		if (track->isMute()) {
			fillColor = kBlueCColor;
		} else {
			fillColor = kGreenCColor;
		}
		break;
	case TRAK_REC:
	case TRAK_DUB:
	case TRAK_INSERT:
	case TRAK_DELETE:
		fillColor = kRedCColor;
		break;

	case TRAK_DEAD:
	default:
		break;
	}
	context->setFillColor(fillColor);
	context->drawEllipse(markerRect, kDrawFilled);
}

void TrackStatusView::onTimedUpdate(const Klein & k, const VstTimeInfo * t)
{
	currentArcDeg = 360;
}
