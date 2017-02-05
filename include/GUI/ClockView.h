#pragma once

#include <string>
#include <sstream>
#include "aeffectx.h"
#include "vstgui.h"
#include "GUI/TextDrawer.h"

#include "debug.h"

using namespace std;

class ClockView : public CView, public TextDrawer {
public:
	ClockView(const CRect &rect)
		: CView(rect), TextDrawer(), beats(0), bars(0), quants(0) {
		setFontColor(kCyanCColor);
		setShadowColor(kGreyCColor);
		setAlign(kLeftText);
	}

	~ClockView() {
	}

	virtual void draw(CDrawContext *context) override {
		stringstream ts;
		ts << bars << ":" << beats << "." << quants;
		string time = ts.str();
		drawText(context, time.c_str(), size);
	}

	void setTime(int ba, int be, int q) {
		bars = ba;
		beats = be;
		quants = q;
		setDirty(true);
	}

	void setTime(const VstTimeInfo *t) {
		if (t != nullptr) {
			theTime = *t;
			float bbf = theTime.ppqPos - theTime.barStartPos;
			beats = int(floor(bbf));
			quants = (bbf - beats) * 1000;// theTime.sampleRate;
			bars = int(floor(theTime.barStartPos)) / theTime.timeSigNumerator;
			bars += beats / theTime.timeSigNumerator;
			beats = beats %  theTime.timeSigNumerator;
//			dbf << bars << ":" << beats << "." << quants << endl;
			setDirty(true);
			invalidRect(size); // because dirty isn't dirty enough it seems ??? TODO FIXME XXXX
		}
	}
protected:
	VstTimeInfo theTime;
	int beats;
	int quants;
	int bars;
};