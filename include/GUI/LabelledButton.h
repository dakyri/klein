#pragma once
#include <string>
#include "controls/cbuttons.h"
#include "GUI/TextDrawer.h"

class LabelledButton : public CKickButton, public TextDrawer {
public:
	//@{
	LabelledButton(const std::string& _label, const CRect& size, IControlListener* listener,
		long tag, CBitmap* background, const CPoint& offset = CPoint(0, 0));
	LabelledButton(const std::string& _label, const CRect& size, IControlListener* listener,
		long tag, CCoord heightOfOneImage, CBitmap* background, const CPoint& offset = CPoint(0, 0));
	LabelledButton(const LabelledButton& labelledButton);
	//@}

	virtual void draw(CDrawContext*);

protected:
	std::string label;
};