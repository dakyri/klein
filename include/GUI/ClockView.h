#pragma once

#include <string>
#include <sstream>
#include "aeffectx.h"
#include "vstgui.h"
#include "GUI/TextDrawer.h"

using namespace std;

class ClockView : public CView, public TextDrawer {
public:
	ClockView(const CRect &rect)
		: CView(rect), TextDrawer(), beats(0), bars(0), quants(0) {
	}

	~ClockView() {
	}

	virtual void draw(CDrawContext *context) override {
		string time;
		stringstream ts(time);
		ts << bars << ":" << beats << "." << quants;
		drawText(context, time.c_str(), size);
	}

	void setTime(VstTimeInfo *t) {
		if (t != nullptr) {
			theTime = *t;
			float bbf = theTime.ppqPos - theTime.barStartPos;
			beats = int(floor(bbf));
			quants = (bbf - beats) * 1000;// theTime.sampleRate;
			bars = int(floor(theTime.barStartPos)) / theTime.timeSigNumerator;
			setDirty();
		}
	}
protected:
	VstTimeInfo theTime;
	int beats;
	int quants;
	int bars;
};