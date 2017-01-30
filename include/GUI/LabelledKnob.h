#pragma once
#include "controls/cknob.h"
#include "GUI/TextDrawer.h"
#include <string>

#define _ASVIEWCONTAINER_
#ifdef _ASVIEWCONTAINER_


class LabelledKnob : public CViewContainer {
public:
	//@{
	LabelledKnob(const std::string& _label, const CRect& size, IControlListener* const listener,
		const long tag, float radius, const CColor &handleC, const CColor & shadowC,
		const CColor &fontC, const CColor & fontShadowC, const float _startTheta, const float _rangeTheta);
	LabelledKnob(const std::string& _label, const CRect& size, IControlListener* const listener,
		const long tag, float radius, const CColor &handleC, const CColor & shadowC,
		const CColor &fontC, const CColor & fontShadowC);
	LabelledKnob(const std::string& _label, const CRect& size, IControlListener* const listener,
		const long tag, CBitmap *knob, const CColor &handleC, const CColor & shadowC,
		const CColor &fontC, const CColor & fontShadowC, const float _startTheta, const float _rangeTheta);
	LabelledKnob(const std::string& _label, const CRect& size, IControlListener* const listener,
		const long tag, CBitmap *knob, const CColor &handleC, const CColor & shadowC,
		const CColor &fontC, const CColor & fontShadowC);
	LabelledKnob(const LabelledKnob& labelledKnob);
	//@}


protected:
	CKnob *vKnob;
	CTextLabel *vLabel;
	std::string label;
};


#else

class LabelledKnob : public CKnob, public TextDrawer {
public:
	//@{
	LabelledKnob(const std::string& _label, const CRect& size, IControlListener* const listener,
		const long tag, float radius, const CColor &handleC, const CColor & shadowC,
		const CColor &fontC, const CColor & fontShadowC, const float _startTheta, const float _rangeTheta);
	LabelledKnob(const std::string& _label, const CRect& size, IControlListener* const listener,
		const long tag, float radius, const CColor &handleC, const CColor & shadowC,
		const CColor &fontC, const CColor & fontShadowC);
	LabelledKnob(const LabelledKnob& labelledKnob);
	//@}

	virtual void draw(CDrawContext*) override;
	virtual void valueToPoint(CPoint &point) const override;
	virtual float valueFromPoint(CPoint& point) const override;

protected:
	CRect knobRect;
	CRect labelRect;
	std::string label;
};

#endif