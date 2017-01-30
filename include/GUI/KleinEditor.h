#pragma once

#include <string>
#include <vector>

#include "plugin-bindings/aeffguieditor.h"
#include "GUI/MasterStrip.h"
#include "GUI/TrackStrip.h"

class Klein;

/**
 * interface encapsulating non vst-able info that we'd like the editor to show
 */
class KleinEditorI {
public:
	virtual void displayHostClock(VstTimeInfo *t) = 0;
	virtual void displayTrackStatus(int trackId) = 0;
	virtual void displaySampleData(int trackId) = 0;
	virtual void displaySelectedTrack(int trackId) = 0;
	virtual void displaySelectedLoop(int trackId) = 0;
	virtual void displayCurrentLayer(int trackId) = 0;
	virtual void displayMessage(const string &msg) = 0;
};

class KleinEditor : public AEffGUIEditor, public KleinEditorI {
public:
	KleinEditor(Klein *effect);
	virtual ~KleinEditor();

	virtual void setParameter(VstInt32 index, float value) override;
	virtual bool getRect(ERect **ppRect) override;
	virtual bool open(void *ptr) override;
	virtual void idle() override;
//	virtual void draw(ERect *pRect) override;
	virtual void close() override;

#if VST_2_1_EXTENSIONS
	virtual bool onKeyDown(VstKeyCode &keyCode) override;
	virtual bool onKeyUp(VstKeyCode &keyCode) override;
#endif
// hooks from KleinEditorI
	virtual void displayHostClock(VstTimeInfo *t);
	virtual void displayTrackStatus(int trackId);
	virtual void displaySampleData(int trackId);
	virtual void displaySelectedTrack(int trackId);
	virtual void displaySelectedLoop(int trackId);
	virtual void displayCurrentLayer(int trackId);
	virtual void displayMessage(const string &msg);

	static CBitmap *buttonBitmap;
	static CBitmap *knob20Bitmap;
protected:
	Klein &klein;

	MasterStrip *masterStrip;
	vector<TrackStrip*> trackStrip;
};