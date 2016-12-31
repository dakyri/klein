#include "KleinEditor.h"

KleinEditor::KleinEditor(AudioEffect *effect)
	: AEffGUIEditor(effect)
{
	/*
	short i;
#ifdef DEBUG
	if (!dbf) {
		dbf = fopen("debug", "w");
		setbuf(dbf, NULL);
	}
#endif
	hKnobHandle = 0;
	hKnobBody = 0;
	hOnOffButton = 0;

	for (i = 0; i<NSGDelayTap; i++) {
		tapDelayFader[i] = NULL;
		tapLevelFader[i] = NULL;
		tapPanFader[i] = NULL;
		tapDelayDisplay[i] = NULL;
		tapLevelDisplay[i] = NULL;
		tapPanDisplay[i] = NULL;
		tapChannelSwapButton[i] = NULL;
		tapSendMenu[i] = NULL;
	}

	feedbackFader = NULL;
	feedbackDisplay = NULL;

	for (i = 0; i<NSGDelayFilter; i++) {
		filterFreqFader[i] = NULL;
		filterResFader[i] = NULL;
		filterFreqDisplay[i] = NULL;
		filterResDisplay[i] = NULL;
		filterEnableButton[i] = NULL;
		filterTypeMenu[i] = NULL;
	}

	for (i = 0; i<NSGDelayLFO; i++) {
		lfoRateFader[i] = NULL;
		lfoDepthFader[i] = NULL;
		lfoRateDisplay[i] = NULL;
		lfoDepthDisplay[i] = NULL;
		lfoTypeMenu[i] = NULL;
		lfoTargetMenu[i] = NULL;
	}

	directLevelDisplay = NULL;
	directLevelFader = NULL;
	directPanDisplay = NULL;
	directPanFader = NULL;




	// load the background bitmap
	// we don't need to load all bitmaps, this could be done when open is called
	hBackground = new CBitmap(kBackgroundId);

	// init the size of the plugin
	rect.left = 0;
	rect.top = 0;
	rect.right = (short)hBackground->getWidth();
	rect.bottom = (short)hBackground->getHeight();
	*/
}

//-----------------------------------------------------------------------------
KleinEditor::~KleinEditor()
{
	/*
	// free background bitmap
	if (hBackground)
		hBackground->forget();
	hBackground = 0;
	*/
}


//-----------------------------------------------------------------------------
long KleinEditor::open(void *ptr)
{
	// !!! always call this !!!
	AEffGUIEditor::open(ptr);

	/*
	// load some bitmaps

	if (!hKnobHandle)
		hKnobHandle = new CBitmap(kKnobHandleId);

	if (!hKnobBody)
		hKnobBody = new CBitmap(kKnobBodyId);

	if (!hOnOffButton)
		hOnOffButton = new CBitmap(kOnOffButtonId);

	//--init background frame-----------------------------------------------
	CRect size(0, 0, hBackground->getWidth(), hBackground->getHeight());
	frame = new CFrame(size, ptr, this);
	frame->setBackground(hBackground);


	//--init the faders------------------------------------------------
	CPoint point(0, 0);
	CPoint offset(1, 0);
	CPoint	offs(0, 0);
	//	size (kFaderX, kFaderY,
	//          kFaderX + hFaderBody->getWidth (), kFaderY + hFaderBody->getHeight ());

	*/
	return true;
}


//-----------------------------------------------------------------------------
void KleinEditor::setParameter(long index, float value)
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
	if (updateFlag) { 
		updateFlag = 0; update();
	} 
}

long
KleinEditor::onKeyDown(VstKeyCode &keyCode) {
	keyCode = keyCode; return -1;
}

long
KleinEditor::onKeyUp(VstKeyCode &keyCode) { 
	keyCode = keyCode;
	return -1; 
}

//-----------------------------------------------------------------------------
void KleinEditor::valueChanged(CDrawContext* context, CControl* control)
{
	long tag = control->getTag();
	/*
	switch (tag)
	{

	case kTap1Delay:
		effect->setParameterAutomated(tag, control->getValue());
		tapDelayFader[0]->update(context);
		tapDelayDisplay[0]->update(context);
		break;
	case kTap1Level:
		effect->setParameterAutomated(tag, control->getValue());
		tapLevelFader[0]->update(context);
		tapLevelDisplay[0]->update(context);
		break;
	}*/
}

long 
KleinEditor::getRect(ERect **ppRect) {
	*ppRect = 0;
	return 0;
}


void KleinEditor::close()
{
	delete frame;
	frame = 0;

	// free some bitmaps
	/*

	if (hOnOffButton)
		hOnOffButton->forget();
	hOnOffButton = 0;

	if (hKnobHandle)
		hKnobHandle->forget();
	hKnobHandle = 0;

	if (hKnobBody)
		hKnobBody->forget();
	hKnobBody = 0;
	*/
}