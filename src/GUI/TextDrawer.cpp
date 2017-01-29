#include "GUI/TextDrawer.h"
#include "debug.h"
//------------------------------------------------------------------------
// TextDrawer
//------------------------------------------------------------------------
/*! @class TextDrawer
 borrows a pile of stuff from CParamDisplay to provide clean text drawing capability to classes that don't otherwise inherit
*/

TextDrawer::TextDrawer(CColor _fontColor, int _style, CColor _shadowColor)
	: horiTxtAlign(kCenterText), fontColor(_fontColor), shadowColor(_shadowColor), style(_style),
		bAntialias(true), fontSize(0), fontStyle(0)
{
	fontID = kNormalFont;
	fontID->remember();
}

TextDrawer::TextDrawer(const TextDrawer& v)
	: horiTxtAlign(v.horiTxtAlign)
	, style(v.style)
	, fontID(v.fontID)
	, fontColor(v.fontColor)
	, shadowColor(v.shadowColor)
	, bAntialias(v.bAntialias), fontSize(v.fontSize), fontStyle(v.fontStyle)
{
	fontID->remember();
}

void
TextDrawer::drawText(CDrawContext *pContext, const char *string, const CRect& rect)
{

	/*
	setDirty(false);

	if (style & kNoDrawStyle)
	return;

	// draw the background
	if (newBack)
	{
	if (bTransparencyEnabled)
	newBack->drawTransparent(pContext, size, backOffset);
	else
	newBack->draw(pContext, size, backOffset);
	}
	else if (pBackground)
	{
	if (bTransparencyEnabled)
	pBackground->drawTransparent(pContext, size, backOffset);
	else
	pBackground->draw(pContext, size, backOffset);
	}
	else
	{
	if (!bTransparencyEnabled)
	{
	pContext->setFillColor(backColor);
	pContext->drawRect(size, kDrawFilled);

	if (!(style & (k3DIn | k3DOut | kNoFrame)))
	{
	pContext->setLineWidth(1);
	pContext->setFrameColor(frameColor);
	pContext->drawRect(size);
	}
	}
	}
	// draw the frame for the 3D effect
	if (style & (k3DIn | k3DOut))
	{
	CRect r(size);
	r.right--; r.top++;
	pContext->setLineWidth(1);
	if (style & k3DIn)
	pContext->setFrameColor(backColor);
	else
	pContext->setFrameColor(frameColor);
	CPoint p;
	pContext->moveTo(p(r.left, r.bottom));
	pContext->lineTo(p(r.left, r.top));
	pContext->lineTo(p(r.right, r.top));

	if (style & k3DIn)
	pContext->setFrameColor(frameColor);
	else
	pContext->setFrameColor(backColor);
	pContext->moveTo(p(r.right, r.top));
	pContext->lineTo(p(r.right, r.bottom));
	pContext->lineTo(p(r.left, r.bottom));
	}
	*/
	if (!(style & kNoTextStyle) && string && strlen(string)) {
		CRect oldClip;
		pContext->getClipRect(oldClip);
		CRect newClip(rect);
		newClip.bound(oldClip);
		pContext->setClipRect(newClip);
		pContext->setFont(fontID, fontSize, fontStyle);

		// draw darker text (as shadow)
		if (style & kShadowText) {
			CRect newSize(rect);
			newSize.offset(1, 1);
			pContext->setFontColor(shadowColor);
			pContext->drawString(string, newSize, horiTxtAlign, bAntialias);
		}
		pContext->setFontColor(fontColor);
		pContext->drawString(string, rect, horiTxtAlign, bAntialias);
		pContext->setClipRect(oldClip);
	}
}

//------------------------------------------------------------------------
void
TextDrawer::setFont(CFontRef fontID, const long _fontSize, const long _fontStyle)
{
	//	setDirty();
	if (fontID)
		fontID->forget();
	fontID = fontID;
	fontID->remember();
	fontSize = _fontSize;
	fontStyle = _fontStyle;
}

//------------------------------------------------------------------------
void
TextDrawer::setFontColor(CColor color)
{
	// to force the redraw
	if (fontColor != color) {
		//		setDirty();
	}
	fontColor = color;
}
void
TextDrawer::setStyle(long val)
{
	if (style != val) {
		style = val;
		//		setDirty();
	}
}