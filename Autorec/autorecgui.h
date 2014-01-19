//-------------------------------------------------------------------------------------------------------
// VST Plug-Ins SDK
// Version 2.4		$Date: 2006/11/13 09:08:28 $
//
// Category     : VST 2.x SDK Samples
// Filename     : sdeditor.h
// Created by   : Steinberg Media Technologies
// Description  : Simple Surround Delay plugin with Editor using VSTGUI
//
// © 2006, Steinberg Media Technologies, All Rights Reserved
//-------------------------------------------------------------------------------------------------------

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
