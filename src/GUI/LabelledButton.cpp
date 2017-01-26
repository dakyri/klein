#include "GUI/LabelledButton.h"

#include "debug.h"

//------------------------------------------------------------------------
// LabelledButton
//------------------------------------------------------------------------
/*! @class LabelledButton
Quick override of CKickButton
*/
//------------------------------------------------------------------------
/**
* LabelledButton constructor.
* @param size the size of this view
* @param listener the listener
* @param tag the control tag
* @param background the bitmap
* @param offset unused
*/
//------------------------------------------------------------------------
LabelledButton::LabelledButton(const std::string & _label, const CRect& size, CControlListener* listener,
		long tag, CBitmap* background, const CPoint& offset)
	: CKickButton(size, listener, tag, background, offset), label(_label)
	, TextDrawer()
{
}

//------------------------------------------------------------------------
/**
* LabelledButton constructor.
* @param size the size of this view
* @param listener the listener
* @param tag the control tag
* @param heightOfOneImage height of one sub bitmap in background
* @param background the bitmap
* @param offset of background
*/
//------------------------------------------------------------------------
LabelledButton::LabelledButton(const std::string & _label, const CRect& size, CControlListener* listener,
		long tag, CCoord heightOfOneImage, CBitmap* background, const CPoint& offset)
	: CKickButton(size, listener, tag, heightOfOneImage, background, offset), label(_label)
	, TextDrawer()
{
}

//------------------------------------------------------------------------
LabelledButton::LabelledButton(const LabelledButton& v)
	: CKickButton(v), TextDrawer(v), label(v.label)
{

}

//------------------------------------------------------------------------
void LabelledButton::draw(CDrawContext *pContext)
{
	CKickButton::draw(pContext);
	drawText(pContext, label.c_str(), size);
//	dbf << "label " << label.c_str() << " drawn into " << size.getWidth() << ", " << size.getHeight() << std::endl;
}
