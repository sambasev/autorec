// Autorec GUI/Editor

#ifndef __autorecgui__
#define __autorecgui__


// include VSTGUI
#ifndef __vstgui__
#include "vstgui.sf/vstgui/vstgui.h"
#endif


//-----------------------------------------------------------------------------
class autorecgui : public AEffGUIEditor, public CControlListener
{
public:
	autorecgui(AudioEffect* effect);
	virtual ~autorecgui();

public:
	virtual bool open(void* ptr);
	virtual void close();

	virtual void setParameter(VstInt32 index, float value);
	virtual void valueChanged(CDrawContext* context, CControl* control);

private:
	// Controls
	CVerticalSlider* delayFader;
	CVerticalSlider* feedbackFader;
	CVerticalSlider* volumeFader;

	CParamDisplay* delayDisplay;
	CParamDisplay* feedbackDisplay;
	CParamDisplay* volumeDisplay;

	// Bitmap
	CBitmap* hBackground;
};

#endif
