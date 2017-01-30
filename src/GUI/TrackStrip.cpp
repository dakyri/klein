
#include <string>
#include "aeffectx.h"
#include "vstgui.h"
#include "KleinTrack.h"
#include "Controller.h"
#include "GUI\TrackStrip.h"
#include "GUI\LabelledKnob.h"
#include "GUI\KleinEditor.h"

#include "debug.h"

using namespace std;

TrackStrip::TrackStrip(Controller &c, KleinTrack * t, CCoord x, CCoord y, CFrame *parent)
	: CViewContainer(CRect(x, y, x+300, y+100)), controller(c), track(t)
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
	knobs.clear();
}

void TrackStrip::setComponents4(Controller & c, KleinTrack * t) {

	string s = std::to_string(t->getId());
	CTextLabel *tl = new CTextLabel(CRect(2, 2, getHeight() - 2, 40), s.c_str(), nullptr, kShadowText | kNoFrame);
	tl->setShadowColor(kGreyCColor);
	tl->setFontColor(kWhiteCColor);
	tl->setBackColor(kBlackCColor);
	tl->setHoriAlign(CHoriTxtAlign::kLeftText);
	addView(tl);
	CCoord wid = 42;
	CCoord height = 64;

	CRect knobSz(0, 0, 60, 63);
	knobSz.offset(60, 2);
	for (CommandGuiMapping &it : c.guiCmds) {
		dbf << "TrackStrip:: checking command <" << it.command << ", " << it.label << "> at " << knobSz.left << ", " << knobSz.top << " wide " << endl;
	}
	for (ControlGuiMapping &it : c.guiCtls) {
		dbf << "TrackStrip:: checking control <" << it.control << ", " << it.label << "> at " << knobSz.left << ", " << knobSz.top << " wide " << endl;
		if (it.type == GuiMapping::KNOB && (it.target == kTargetAllTracks || it.target == track->getId())) {
#if KLEIN_DEBUG >= 5
			dbf << "TrackStrip:: " << track->getId() << " adding <" << it.control << ", " << it.label << "> for " << hex << it.tag << dec <<" at " << knobSz.left << ", " << knobSz.top << " wide " << endl;
#endif
			long tag = it.tag;
			if (it.target == kTargetAllTracks) {
				setTagTargetId(tag, track->getId());
				dbf << " setting target to " << track->getId() << hex << " from " << it.tag << " to " << tag << dec << endl;
			}
			LabelledKnob *kg = new LabelledKnob(it.label, knobSz, &c, tag, KleinEditor::knob20Bitmap, kBlackCColor, kGreyCColor, kWhiteCColor, kGreyCColor);
			kg->remember();
			addView(kg);
			knobs.push_back(kg);

			knobSz.offset(2 + 60, 0);
			if (knobSz.right > wid) {
				wid = knobSz.right;
			}
			if (knobSz.bottom > height) {
				height = knobSz.bottom;
			}
		}
	}
	for (ScriptGuiMapping &it : c.guiKlfs) {
		dbf << "TrackStrip:: checking scruot <" << it.script << ", " << it.label << "> at " << knobSz.left << ", " << knobSz.top << " wide " << endl;
	}
	/*
	LabelledKnob *kg = new LabelledKnob("gain", CRect(60, 2, 118, 62), &c, 0, 20, kWhiteCColor, kGreyCColor, kWhiteCColor, kGreyCColor);
	addView(kg);
	LabelledKnob *kp = new LabelledKnob("pan", CRect(120, 2, 178, 62), &c, 0, 20, kWhiteCColor, kGreyCColor, kWhiteCColor, kGreyCColor);
	addView(kp);
	wid = 180;
	*/

	CRect sz;
	getViewSize(sz);
	sz.right = sz.left + wid;
	sz.bottom = sz.top + height;
	setViewSize(sz);

}

void TrackStrip::displayTrackStatus()
{
}

void TrackStrip::displaySampleData()
{
}

void TrackStrip::displaySelectedTrack()
{
}

void TrackStrip::displaySelectedLoop()
{
}

void TrackStrip::displayCurrentLayer()
{
}
