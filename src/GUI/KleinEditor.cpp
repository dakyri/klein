#include "Klein.h"
#include "GUI/KleinEditor.h"
#include "debug.h"

CBitmap *KleinEditor::buttonBitmap = nullptr;
CBitmap *KleinEditor::knob20Bitmap = nullptr;

KleinEditor::KleinEditor(Klein *_klein)
	: AEffGUIEditor(_klein), klein(*_klein), timeUpdateTimer(nullptr), masterStrip(nullptr)
{
}

//-----------------------------------------------------------------------------
KleinEditor::~KleinEditor()
{
	// free background bitmap
	
	if (buttonBitmap) {
		buttonBitmap->forget();
	}
	buttonBitmap = nullptr;
	if (knob20Bitmap) {
		knob20Bitmap->forget();
	}
	knob20Bitmap = nullptr;
	if (timeUpdateTimer) {
		timeUpdateTimer->forget();
	}
	timeUpdateTimer = nullptr;
}


/**
 * open the window. main vst hook
 * ownership of the master strip, track strips etc. belong to the frame
 */
bool KleinEditor::open(void *ptr)
{
#if KLEIN_DEBUG >= 5
	dbf << "openning editor with " << /*klein.track.size() << */" tracks" << endl;
#endif
	// !!! always call this !!!
//	AEffGUIEditor::open(ptr);

	if (!buttonBitmap) {
		buttonBitmap = new CBitmap("Button.png");
#if KLEIN_DEBUG >= 5
		dbf << "loading bitmap " << buttonBitmap->isLoaded() << ", wide " << buttonBitmap->getWidth() << ", high " << buttonBitmap->getHeight() << endl;
#endif
	}
	if (!knob20Bitmap) {
		knob20Bitmap = new CBitmap("Knob20.png");
#if KLEIN_DEBUG >= 5
		dbf << "loading bitmap " << knob20Bitmap->isLoaded() << ", wide " << knob20Bitmap->getWidth() << ", high " << knob20Bitmap->getHeight() << endl;
#endif
	}

	CRect frameSize(0, 0, 300, 300);
	CFrame* newFrame = new CFrame(frameSize, this);
	newFrame->open(ptr);
	newFrame->setBackgroundColor(kGreenCColor);

	CCoord pos = 0;
	CCoord wid = 0;
	CRect ssz;
#if KLEIN_DEBUG >= 5
	dbf << "now scanning tracks ... still got " << klein.track.size() << " tracks" << endl;
#endif

	masterStrip = new MasterStrip(klein.controller, 0, pos, newFrame);
	newFrame->addView(masterStrip);
	masterStrip->getViewSize(ssz);
	if (ssz.getWidth() > wid) {
		wid = ssz.getWidth();
	}
	pos += ssz.getHeight();

	for (unique_ptr<KleinTrack> &it: klein.track) {
		KleinTrack *t = it.get();

		TrackStrip *ts = new TrackStrip(klein.controller, t, 0, pos, newFrame);
		newFrame->addView(ts);
		trackStrip.push_back(ts);
		ts->getViewSize(ssz);

		if (ssz.getWidth() > wid) {
			wid = ssz.getWidth();
		}
		pos += ssz.getHeight();
#if KLEIN_DEBUG >= 5
		dbf << "adding track strip " << pos << ", " << wid << ", id " << t->getId() << endl;
#endif
	}
	newFrame->setSize(wid, pos);


	frame = newFrame;
#if KLEIN_DEBUG >= 5
	dbf << "main frame ... is built " << endl;
#endif
	if (!timeUpdateTimer) {
		timeUpdateTimer = new CVSTGUITimer([this](CVSTGUITimer *t){
			VstTimeInfo vti = klein.getCurrentVSTTime();
			masterStrip->onTimedUpdate(&vti);
			for (auto it : trackStrip) {
				it->onTimedUpdate(klein, &vti);
			}
		}, 50);
	}
	return true;
}


//-----------------------------------------------------------------------------
void KleinEditor::setParameter(VstInt32 index, float value)
{
	if (!frame)
		return;

	// called from ADelayEdit
	/*
	static float i = 0;
	switch (index)
	{
	case kTap1Delay:
		if (tapDelayFader[0])
			tapDelayFader[0]->setValue(effect->getParameter(index));
		if (tapDelayDisplay[0])
			tapDelayDisplay[0]->setValue(effect->getParameter(index));
		break;
	case kTap1Level:
		if (tapLevelFader[0])
			tapLevelFader[0]->setValue(effect->getParameter(index));
		if (tapLevelDisplay[0])
			tapLevelDisplay[0]->setValue(effect->getParameter(index));
		break;
	}*/
}

void
KleinEditor::idle() { 
//	if (updateFlag) { 
//		updateFlag = 0; update();
//	} 
}

bool
KleinEditor::onKeyDown(VstKeyCode &keyCode) {
	keyCode = keyCode; return false;
}

bool
KleinEditor::onKeyUp(VstKeyCode &keyCode) { 
	keyCode = keyCode;
	return false; 
}



bool 
KleinEditor::getRect(ERect **ppRect) {
	*ppRect = 0;
	return false;
}


void KleinEditor::close()
{
	CFrame* oldFrame = frame;
	frame = nullptr;
	masterStrip = nullptr;
	trackStrip.clear();
	if (oldFrame) {
		oldFrame->forget();
	}
}


void KleinEditor::displayHostClock(VstTimeInfo *t)
{
	if (masterStrip) {
		masterStrip->displayHostClock();
	}

}

void KleinEditor::displayTrackStatus(int trackId)
{
	if (trackId >= 0 && trackId < trackStrip.size()) {
		trackStrip[trackId]->displayTrackStatus();
	}
}

void KleinEditor::displaySampleData(int trackId)
{
	if (trackId >= 0 && trackId < trackStrip.size()) {
		trackStrip[trackId]->displaySampleData();
	}
}

void KleinEditor::displaySelectedTrack(int trackId)
{
	if (trackId >= 0 && trackId < trackStrip.size()) {
		trackStrip[trackId]->displaySelectedTrack();
	}
}

void KleinEditor::displaySelectedLoop(int trackId)
{
	if (trackId >= 0 && trackId < trackStrip.size()) {
		trackStrip[trackId]->displaySelectedLoop();
	}
}

void KleinEditor::displayCurrentLayer(int trackId)
{
	if (trackId >= 0 && trackId < trackStrip.size()) {
		trackStrip[trackId]->displayCurrentLayer();
	}
}

void KleinEditor::displayMessage(const string & msg)
{
	if (masterStrip) {
		masterStrip->displayMessage(msg);
	}
}
