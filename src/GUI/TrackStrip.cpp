
#include "vstgui.h"
#include "vstcontrols.h"
#include "KleinTrack.h"
#include "GUI\TrackStrip.h"

TrackStrip::TrackStrip(KleinTrack * t, CCoord x, CCoord y)
	: CView(CRect(0, 0, 300, 100))
{
}

TrackStrip::~TrackStrip()
{
}
