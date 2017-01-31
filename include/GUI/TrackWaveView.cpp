#include "aeffectx.h"
#include "TrackWaveView.h"

TrackWaveView::TrackWaveView(const CRect & r)
	: CView(r)
{
}

TrackWaveView::~TrackWaveView()
{
}

void TrackWaveView::draw(CDrawContext * context)
{
	context->setFillColor(kWhiteCColor);
	context->drawRect(getViewSize(), kDrawFilled);
}


void TrackWaveView::onTimedUpdate(const Klein & k, const VstTimeInfo * t)
{
}
