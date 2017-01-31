#pragma once
#include "vstgui.h"

#include <vector>

class Controller;

using namespace std;
class LabelledButton;
class MessageView;
class ClockView;

class MasterStrip : public CViewContainer {
public:
	MasterStrip(Controller &c, CCoord x, CCoord y, CFrame *parent);
	virtual ~MasterStrip();

	void setComponents4(Controller &c);
	void clearComponents();

	void displayHostClock();
	void displayMessage(const string &msg);

	void onTimedUpdate(const VstTimeInfo *);

protected:
	const int kMessageViewWidth = 200;
	const int kMessageViewHeight = 25;
	const int kClockViewWidth = 60;
	const int kClockViewHeight = 25;

	Controller &controller;

	vector<LabelledButton*> buttons;
	MessageView *messageView;
	ClockView *clockView;

};