#include "GUI\LabelledKnob.h"

#include "debug.h"

const float kPi = 3.14159265358979323846;

#ifdef _ASVIEWCONTAINER_
LabelledKnob::LabelledKnob(const std::string & _label, const CRect & size, IControlListener * const listener,
	const long tag, float _radius, const CColor & _handleColor, const CColor & _shadowColor,
	const CColor &fontC, const CColor & fontShadowC, const float _startAngle, const float _rangeAngle)
	: LabelledKnob(_label, size, listener, tag, _radius, _handleColor, _shadowColor, fontC, fontShadowC)
{
	vKnob->setStartAngle(_startAngle);
	vKnob->setRangeAngle(_rangeAngle);
}

LabelledKnob::LabelledKnob(const std::string & _label, const CRect & size, IControlListener * const listener,
	const long tag, float _radius, const CColor & _handleColor, const CColor & _shadowColor,
	const CColor &fontC, const CColor & fontShadowC)
	: CViewContainer(size), label(_label)
{
	float c = size.getWidth() / 2;
	CRect knobRect(2, c - _radius, 2 + 2 * _radius, c + _radius);
	CRect labelRect(2, knobRect.bottom + 2, size.getWidth()-2, size.getHeight()-knobRect.bottom);

	vKnob = new CKnob(knobRect, listener, tag, nullptr, nullptr);
	addView(vKnob);
	vKnob->setColorShadowHandle(_shadowColor);
	vKnob->setColorHandle(_handleColor);

	vLabel = new CTextLabel(labelRect);
	addView(vLabel);
	vLabel->setText(label.c_str());
	vLabel->setStyle(kNoFrame | kShadowText);
	vLabel->setFontColor(fontC);
	vLabel->setShadowColor(fontShadowC);
}

LabelledKnob::LabelledKnob(const LabelledKnob & v)
	: CViewContainer(v), label(v.label), vKnob(v.vKnob), vLabel(v.vLabel) 
{
}

#else
LabelledKnob::LabelledKnob(const std::string & _label, const CRect & size, IControlListener * const listener,
		const long tag, float _radius, const CColor & _handleColor, const CColor & _shadowColor,
		const CColor &fontC, const CColor & fontShadowC, const float _startAngle, const float _rangeAngle)
	: LabelledKnob(_label, size, listener, tag, _radius, _handleColor, _shadowColor, fontC, fontShadowC)
{
	setStartAngle(_startAngle);
	setRangeAngle(_rangeAngle);
}

LabelledKnob::LabelledKnob(const std::string & _label, const CRect & size, IControlListener * const listener,
	const long tag, float _radius, const CColor & _handleColor, const CColor & _shadowColor,
	const CColor &fontC, const CColor & fontShadowC)
	: CKnob(size, listener, tag, nullptr, nullptr), TextDrawer(fontC, kShadowText, fontShadowC), label(_label)
{
	float c = (size.left + size.right) / 2;
	knobRect.top = size.top;
	knobRect.left = c - _radius;
	knobRect.right = c + _radius;
	knobRect.bottom = size.top + 2 * _radius;
	radius = _radius;
	labelRect = size;
	labelRect.top = knobRect.bottom + 2;
	
	setColorShadowHandle(_shadowColor);
	setColorHandle(_handleColor);
}

LabelledKnob::LabelledKnob(const LabelledKnob & v)
	: CKnob(v), TextDrawer(v), label(v.label), labelRect(v.labelRect), knobRect(v.knobRect)
{
}

void LabelledKnob::draw(CDrawContext *pContext)
{
	CRect kr = knobRect;
	CPoint where;
	valueToPoint(where);
	pContext->setLineWidth(2);
	pContext->setFrameColor(colorHandle);
	if (pBackground) {
		if (bTransparencyEnabled) {
			pBackground->drawTransparent(pContext, kr, offset);
		} else {
			pBackground->draw(pContext, kr, offset);
		}
	} else {
		pContext->drawEllipse(CRect(kr.left+1, kr.top+1, kr.right-1, kr.bottom-1));
	}
	if (pHandle) {
		CCoord width = pHandle->getWidth();
		CCoord height = pHandle->getHeight();
		where.offset(kr.left - width / 2, kr.top - height / 2);

		CRect handleSize(0, 0, width, height);
		handleSize.offset(where.x, where.y);
		pHandle->drawTransparent(pContext, handleSize);
	} else {
		CPoint origin(kr.width() / 2, kr.height() / 2);
		origin.offset(kr.left, kr.top);
		where.offset(kr.left, kr.top);

		pContext->setFrameColor(colorShadowHandle);
		pContext->drawLine(CPoint(where.x-1, where.y), CPoint(origin.x-1, origin.y));

		pContext->setFrameColor(colorHandle);
		pContext->drawLine(where, origin);
		/*

		float toAngle = (startAngle + (value * rangeAngle)) * 360.0 / kPi;
		float fromAngle = startAngle * 360.0 / kPi;
		pContext->setLineWidth(3);
		dbf << "draw " << value << ", " << fromAngle << ", " << toAngle<< std::endl;
		CRect shadowRect = knobRect.offset(-1, 0);
		pContext->setFrameColor (colorShadowHandle);
		pContext->drawArc(shadowRect, fromAngle, toAngle);

		pContext->setFrameColor (colorHandle);
		pContext->drawArc(knobRect, fromAngle, toAngle);
		*/
	}
	drawText(pContext, label.c_str(), labelRect);
	setDirty(false);
}

//------------------------------------------------------------------------
void LabelledKnob::valueToPoint(CPoint &point) const
{
	float alpha = (value - bCoef) / aCoef;
	point.x = (CCoord)(radius + cosf(alpha) * (radius - inset) + 0.5f);
	point.y = (CCoord)(radius - sinf(alpha) * (radius - inset) + 0.5f);
//	point.x -= knobRect.left;
}

float
LabelledKnob::valueFromPoint(CPoint& point) const {
	point.x -= (knobRect.left-size.left);
	return CKnob::valueFromPoint(point);
}

#endif