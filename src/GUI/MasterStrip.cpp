#include "aeffectx.h"
#include "GUI/MasterStrip.h"
#include "GUI/LabelledButton.h"
#include "GUI/KleinEditor.h"
#include "GUI/ClockView.h"
#include "GUI/MessageView.h"
#include "Controller.h"

#include "debug.h"

MasterStrip::MasterStrip(Controller &c, CCoord x, CCoord y, CFrame *parent)
	: CViewContainer(CRect(x, y, x + 300, y + 100)), controller(c)
{
#if KLEIN_DEBUG >= 5
	dbf << "MasterStrip ... we're in " << c.guiCmds.size() << " gui elements" << endl;
#endif
	setComponents4(c);
	invalid();
}

MasterStrip::~MasterStrip()
{
	clearComponents();
}

void MasterStrip::clearComponents() {
	removeAll(); 
// i think these have been remmebered ... ie removeAll with default parameters will forget them
//	for (auto it : buttons) {
//		delete it;
//	}
	buttons.clear();
}

void MasterStrip::setComponents4(Controller & c)
{
	CRect buttonSz(0, 0, KleinEditor::buttonBitmap->getWidth(), KleinEditor::buttonBitmap->getHeight() / 2);
	buttonSz.offset(2, 2);
	CCoord pos = 0;
	CCoord wid = getWidth();

#if KLEIN_DEBUG >= 5
	dbf << "MasterStrip:: " << c.guiCmds.size() << " gui elements" << endl;
#endif
	for (CommandGuiMapping &it : c.guiCmds) {
		if (it.type == GuiMapping::BUTTON && it.target != kTargetAllTracks) { // either global (ie selected) or specifi track
#if KLEIN_DEBUG >= 5
			dbf << "MasterStrip:: adding <" << it.command << ", " << it.label << "> at " << buttonSz.left << ", " << buttonSz.top << " wide " << endl;
#endif
			LabelledButton *b = new LabelledButton(it.label, buttonSz, &c, it.tag, KleinEditor::buttonBitmap->getHeight() / 2, KleinEditor::buttonBitmap);
			b->remember();
			addView(b);
			buttons.push_back(b);

			buttonSz.offset(2 + KleinEditor::buttonBitmap->getWidth(), 0);
			if (buttonSz.right > wid) {
				wid = buttonSz.right;
			}
			if (buttonSz.bottom > pos) {
				pos = buttonSz.bottom;
			}
		}
	}
	for (ControlGuiMapping &it : c.guiCtls) {

	}
	for (ScriptGuiMapping &it : c.guiKlfs) {

	}

	CRect clockRect(0, 0, kClockViewWidth, kClockViewHeight);
	clockRect.offset(2, pos + 2);
	clockView = new ClockView(clockRect);
	addView(clockView);
	clockView->setTime(0, 0, 0);
	CRect r = clockView->getViewSize();
	float x = r.right;
	float y = r.top;
	if (r.bottom > pos) {
		pos = r.bottom;
	}
	CRect messageRect(0, 0, kMessageViewWidth, kMessageViewHeight);
	messageRect.offset(x + 10, y);
	messageView = new MessageView(messageRect);
	addView(messageView);
	r = messageView->getViewSize();
	x = r.right;
	if (x > wid) {
		wid = x;
	}
	if (r.bottom > pos) {
		pos = r.bottom;
	}

	CRect sz;
	getViewSize(sz);
	sz.right = sz.left + wid + 2;
	sz.bottom = sz.top + pos + 2;
	setViewSize(sz);
}

void MasterStrip::displayHostClock()
{
}

void MasterStrip::displayMessage(const string & msg)
{
}

void MasterStrip::onTimedUpdate(const VstTimeInfo *t)
{
	clockView->setTime(t);
}

