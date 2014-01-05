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

	//Buffer for recording
	buffer.assign(bufsize, 0.0);	//Init bufsize samples with 0.0
	bufferLen = k5s;
}

autorec::~autorec() {
	//__asm int 3;	//assembly interrupt (break when destructor is called)
	buffer.clear();	
	buffer.resize(0);  //Necessary?
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
	if (play) {
		if (!done && (playCursor < cursor)) {
			playCursor = cursor;
			done = true;	// Refresh cursor only once else its refreshed whenever processReplacing() is called
		}
		while (--sampleFrames >= 0) 
		{
			if (playCursor >= bufsize)
			{
				playCursor = 0;		//Wrap around
			}
			(*out1++) = buffer[playCursor++];
			(*out2++) = buffer[playCursor++];
		}
	}
	else {
		done = false; saved = false;		//New material has been recorded. Refresh the play cursor to reflect this state
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
}

void autorec::setProgramName(char* name){
	vst_strncpy(programName, name, kVstMaxProgNameLen);
}

void autorec::getProgramName(char* name){
	vst_strncpy(name, programName, kVstMaxProgNameLen);
}

void autorec::setParameter(VstInt32 index, float value){
	
	switch (index) {
		case kPlay: 
		{
			if (value <= 0.5)
			 {
				play = false;
			 }
			else
			{
				play = true;
				if (!saved) {
					writeWAVData("C:\\Users\\samba_000\\Desktop\\myFile.wav", buffer.data(), buffer.size()*sizeof(float), 44100, 2);
					saved = true;
				}

			}
		}; break;
		case kBufferLength:
		{
			if (value <= 0.5) 
				bufferLen = k5s;
			else 
				bufferLen = k10s; 
		}; resizeBuffer(); break;
			
	}
}

float autorec::getParameter(VstInt32 index){
	switch (index) {
		case kPlay: 	
		{
			if (play)
				return 1;
			else
				return 0;
		}; break;
		case kBufferLength:
		{
			if (bufferLen)
				return 1; //Temporary. param to float conversion?
			else
				return 0;
		};  break;
		default: 
			return 0;
	}
}

void autorec::getParameterLabel(VstInt32 index, char* label){
	switch (index) {
		case kPlay: vst_strncpy(label, "", kVstMaxParamStrLen); break;
		case kBufferLength: vst_strncpy(label, "sec", kVstMaxParamStrLen); break;
		default: vst_strncpy(label, "units", kVstMaxParamStrLen); break;
	}
}

void autorec::getParameterDisplay(VstInt32 index, char* text){
	switch (index){
		case kPlay: {
			if (play) 
				vst_strncpy(text, "ON", kVstMaxParamStrLen);
			else 
				vst_strncpy(text, "OFF", kVstMaxParamStrLen);
		}; break; 	// Temp
		case kBufferLength: {
			if(bufferLen == k5s) 
				vst_strncpy(text, "5", kVstMaxParamStrLen); 
			else
				vst_strncpy(text, "10", kVstMaxParamStrLen); 
		}; break;
		default: vst_strncpy(text, "Default", kVstMaxParamStrLen); break;
	}
}

void autorec::getParameterName(VstInt32 index, char* text){
	switch (index){
		case kPlay: 	vst_strncpy(text, "Play", kVstMaxProgNameLen); break;
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

//Reads current buffer setting (updated by host/user)
//Updates buffer, and bufsize
//TODO:Make it modulr - give old size and new size?
void autorec::resizeBuffer(){
	int oldBufsize = bufsize;
	int newBufsize;
	switch (bufferLen)
	{
		case k5s: seconds = 5; break;
		case k10s: seconds = 10; break;
	}
	newBufsize = sampleRate * seconds * channels;
	if (oldBufsize == newBufsize) return;
	bufsize = newBufsize;
	if (newBufsize > oldBufsize)			//Expand
	{
		buffer.resize(newBufsize, 0.0);	
		return;
	}
	if (newBufsize < oldBufsize)			//Shrink
	{
		buffer.resize(newBufsize, 0.0);
		if (playCursor > oldBufsize)
			playCursor -= oldBufsize;
		if (cursor > oldBufsize)
			cursor -= oldBufsize;
	}
}
