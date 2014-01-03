#include "autorec.h"

AudioEffect* createEffectInstance(audioMasterCallback audioMaster) {
	return new autorec(audioMaster);
}

autorec::autorec(audioMasterCallback audioMaster) :
AudioEffectX(audioMaster, 0, NUM_PARAMS) {
	setNumInputs(2);		// stereo in
	setNumOutputs(2);		// stereo out
	setUniqueID('arec');	// identify
	canProcessReplacing();	// supports replacing output
//	canDoubleReplacing();	// supports double precision processing

	//Buffer for recording
	buffer = new float[bufsize+2];
}

autorec::~autorec() {
	if (buffer)
		delete[] buffer;
}

void autorec::processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames) {
	/*
	Init	 - Create a global buffer to store X seconds of audio (# of samples = X * samplerate * channels)
	Process  - None. output = input. Just copy input to a circular buffer (similar to filters). 
	When 'RETRIEVE' button is hit, copy/overwrite it into another buffer (store). Maybe prompt for storage.
	When 'PLAY' is hit, play from buffer. Hitting RETRIEVE will overwrite play buffer with last X seconds of audio.
	*/ 
	float* in1 = inputs[0];
	float* in2 = inputs[1];
	float* out1 = outputs[0];
	float* out2 = outputs[1];

	while (--sampleFrames >= 0)
	{
		buffer[cursor++] = (*in1);
		buffer[cursor++] = (*in2);
		(*out1++) = (*in1++);
		(*out2++) = (*in2++);
		if (cursor >= bufsize)
		{
			cursor = 0;		//Wrap around
		}
	}
}

void autorec::setProgramName(char* name){
	vst_strncpy(programName, name, kVstMaxProgNameLen);
}

void autorec::getProgramName(char* name){
	vst_strncpy(name, programName, kVstMaxProgNameLen);
}

void autorec::setParameter(VstInt32 index, float value){
	switch (index) {
		case kRec: {
		 if (value)
			 rec = true;
		 else
			 rec = false;
		}; break;
		case kBufferLength: 
			bufferLen = value; break;
	}
}

float autorec::getParameter(VstInt32 index){
	switch (index) {
		case kRec: 	{
			if (rec) 
				return 1;
			else 
				return 0;
		}; break;
		case kBufferLength: 
			return bufferLen; //Temporary. param to float conversion?
			break;
		default: 
			return 0;
	}
}

void autorec::getParameterLabel(VstInt32 index, char* label){
	switch (index) {
		case kRec: vst_strncpy(label, "", kVstMaxParamStrLen); break;
		case kBufferLength: vst_strncpy(label, "sec", kVstMaxParamStrLen); break;
		default: vst_strncpy(label, "units", kVstMaxParamStrLen); break;
	}
}

void autorec::getParameterDisplay(VstInt32 index, char* text){
	switch (index){
		case kRec: {
			if (rec) 
				vst_strncpy(text, "ON", kVstMaxParamStrLen);
			else 
				vst_strncpy(text, "OFF", kVstMaxParamStrLen);
		}; break; 	// Temp
		case kBufferLength: {
			if(bufferLen <=0.5) 
				vst_strncpy(text, "5", kVstMaxParamStrLen); 
			else
				vst_strncpy(text, "10", kVstMaxParamStrLen); 
		}; break;
		default: vst_strncpy(text, "Default", kVstMaxParamStrLen); break;
	}
}

void autorec::getParameterName(VstInt32 index, char* text){
	switch (index){
		case kRec: 	vst_strncpy(text, "Rec", kVstMaxProgNameLen); break;
		case kBufferLength: 	vst_strncpy(text, "Buffer:", kVstMaxProgNameLen); break;
		default: 	vst_strncpy(text, "default", kVstMaxProgNameLen); break;
	}
}

bool autorec::getEffectName(char* name){
	vst_strncpy(name, programName, kVstMaxProgNameLen);
	return true;
}

bool autorec::getVendorString(char* text){
	vst_strncpy(text, "Vantage Audio", kVstMaxProgNameLen);
	return true;
}

bool autorec::getProductString(char* text){
	vst_strncpy(text, "AutoRec", kVstMaxProgNameLen);
	return true;
}

VstInt32 autorec::getVendorVersion(){
	return 1;
}