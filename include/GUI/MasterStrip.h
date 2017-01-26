#pragma once
#include "vstgui.h"

#include <vector>

class Controller;

using namespace std;
class LabelledButton;

class MasterStrip : public CViewContainer {
public:
	MasterStrip(Controller &c, CCoord x, CCoord y, CFrame *parent, CBitmap *_buttonBitmap);
	virtual ~MasterStrip();

	void setComponents4(Controller &c);
	void clearComponents();

protected:
	Controller &controller;
	vector<LabelledButton*> buttons;
	CBitmap *buttonBitmap;
};