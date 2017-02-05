#pragma once

#include <list>
#include <string>

#include "vstgui.h"
#include "GUI/TextDrawer.h"

using namespace std;

class MessageView : public CView, public TextDrawer {
public:
	MessageView(const CRect &rect)
			: CView(rect), TextDrawer() {
		setFontColor(kBlackCColor);
	}

	~MessageView() {
	}

	virtual void draw(CDrawContext *context) override {
		context->setFillColor(kGreyCColor);
		context->drawRect(size, kDrawFilled);
		if (messages.size() > 0) {
			drawText(context, messages.back().c_str(), size);
		}
	}

	void addMessage(string msg) {
		messages.push_back(msg);
		setMaxMessages(maxMessages);
	}
	void clearMessages() {
		messages.clear();
		setDirty();
	}
	void setMaxMessages(int m) {
		maxMessages = m;
		while (messages.size() > maxMessages) {
			messages.pop_front();
		}
		setDirty();
	}

protected:
	list<string> messages;
	int maxMessages;
};
