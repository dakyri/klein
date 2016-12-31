#pragma once

#define DEBUG 0
#include "aeffguieditor.h"

class KleinEditor : public AEffGUIEditor, public CControlListener {
public:
	KleinEditor(AudioEffect *effect);
	virtual ~KleinEditor();

	virtual void setParameter(long index, float value) override; 
	virtual long getRect(ERect **ppRect) override;
	virtual long open(void *ptr) override;
	virtual void idle() override;
//	virtual void draw(ERect *pRect) override;
	virtual void close() override;

#if VST_2_1_EXTENSIONS
	virtual long onKeyDown(VstKeyCode &keyCode) override;
	virtual long onKeyUp(VstKeyCode &keyCode) override;
#endif
	virtual void valueChanged(CDrawContext *pContext, CControl *pControl) override;
	virtual long controlModifierClicked(CDrawContext *pContext, CControl *pControl, long button) override {
		return 0;		// return 1 if you want the control to not handle it, otherwise 0
	}	
};