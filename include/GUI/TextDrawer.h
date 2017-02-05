#pragma once
#include "vstgui.h"


class TextDrawer {
public:
	TextDrawer(CColor _fontColor = kBlackCColor, int _style = kShadowText, CColor _shadowColor = kWhiteCColor, CHoriTxtAlign _align = kCenterText);
	TextDrawer(const TextDrawer& v);

	void setFont(CFontRef fontID, const long _fontSize=0, const long _fontStyle=0);
	void setFontColor(CColor color);
	void setShadowColor(CColor color);
	void setStyle(long val);
	void setAlign(CHoriTxtAlign val);

protected:
	void drawText(CDrawContext *pContext, const char *string, const CRect& rect);

	CFontRef fontID;
	CColor fontColor;
	CColor shadowColor;
	bool bAntialias;
	CHoriTxtAlign horiTxtAlign;
	long style;
	long fontSize;
	long fontStyle;
};

