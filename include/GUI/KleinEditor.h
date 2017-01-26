#pragma once

#define DEBUG 0
#include "aeffguieditor.h"
#include "GUI/MasterStrip.h"
#include "GUI/TrackStrip.h"

class Klein;

class KleinEditor : public AEffGUIEditor/*, public CControlListener*/ {
public:
	KleinEditor(Klein *effect);
	virtual ~KleinEditor();

	virtual void setParameter(VstInt32 index, float value) override;
	virtual bool getRect(ERect **ppRect) override;
	virtual bool open(void *ptr) override;
	virtual void idle() override;
//	virtual void draw(ERect *pRect) override;
	virtual void close() override;

#if VST_2_1_EXTENSIONS
	virtual bool onKeyDown(VstKeyCode &keyCode) override;
	virtual bool onKeyUp(VstKeyCode &keyCode) override;
#endif
//	virtual void valueChanged(VSTGUI::CControl* pControl) override;
//	virtual long controlModifierClicked(VSTGUI::CControl* pControl, long button) override { return 0; }	///< return 1 if you want the control to not handle it, otherwise 0
protected:
	Klein &klein;

	CBitmap *buttonBitmap;
};