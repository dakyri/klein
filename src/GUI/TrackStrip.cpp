
#include <string>
#include "vstgui.h"
#include "vstcontrols.h"
#include "KleinTrack.h"
#include "Controller.h"
#include "GUI\TrackStrip.h"
#include "GUI\LabelledKnob.h"

#include "debug.h"

using namespace std;

TrackStrip::TrackStrip(Controller &c, KleinTrack * t, CCoord x, CCoord y, CFrame *parent)
	: CViewContainer(CRect(x, y, x+300, y+100), parent), controller(c), track(t)
{
	setBackgroundColor(kBlackCColor);
	setComponents4(c, t);
}

TrackStrip::~TrackStrip()
{
	clearComponents();
}

void TrackStrip::clearComponents() {
	removeAll();
	// i think these have been remmebered ... ie removeAll with default parameters will forget them
	//	for (auto it : buttons) {
	//		delete it;
	//	}
//	buttons.clear();
}

void TrackStrip::setComponents4(Controller & c, KleinTrack * t) {

	string s = std::to_string(t->getId());
	CTextLabel *tl = new CTextLabel(CRect(2, 2, getHeight() - 2, 40), s.c_str(), nullptr, kShadowText | kNoFrame);
	tl->setShadowColor(kGreyCColor);
	tl->setFontColor(kWhiteCColor);
	tl->setBackColor(kBlackCColor);
	tl->setHoriAlign(CHoriTxtAlign::kLeftText);
	addView(tl);
	CCoord wid = 0;
	CCoord height = 64;

	LabelledKnob *kg = new LabelledKnob("gain", CRect(60, 2, 118, 62), &c, 0, 20, kWhiteCColor, kGreyCColor, kWhiteCColor, kGreyCColor);
	addView(kg);
	LabelledKnob *kp = new LabelledKnob("pan", CRect(120, 2, 178, 62), &c, 0, 20, kWhiteCColor, kGreyCColor, kWhiteCColor, kGreyCColor);
	addView(kp);
	wid = 180;

	CRect sz;
	getViewSize(sz);
	sz.right = sz.left + wid;
	sz.bottom = sz.top + height;
	setViewSize(sz);

}
