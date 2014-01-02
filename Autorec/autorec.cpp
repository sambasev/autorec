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
	canDoubleReplacing();	// supports double precision processing
}

autorec::~autorec() {

}

void autorec::processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames) {
	//Actual processing
	float* in1 = inputs[0];
	float* in2 = inputs[1];
	float* out1 = outputs[0];
	float* out2 = outputs[1];

	while (--sampleFrames >= 0)
	{
		(*out1++) = (*in1++);
		(*out2++) = (*in2++);
	}
}

void autorec::processDoubleReplacing(double** inputs, double** outputs, VstInt32 sampleFrames){
	double* in1 = inputs[0];
	double* in2 = inputs[1];
	double* out1 = outputs[0];
	double* out2 = outputs[1];

	while (--sampleFrames >= 0)
	{
		(*out1++) = (*in1++);
		(*out2++) = (*in2++);
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

}

void autorec::getParameterDisplay(VstInt32 index, char* text){
	switch (index){
		case kRec: dB2string(1.0, text, kVstMaxParamStrLen); break;	// Temp
		case kBufferLength: dB2string(bufferLen, text, kVstMaxParamStrLen); break;
	}
}

void autorec::getParameterName(VstInt32 index, char* text){
	switch (index){
		case kRec: 	vst_strncpy(text, "Rec", kVstMaxProgNameLen);
		case kBufferLength: 	vst_strncpy(text, "Buffer:", kVstMaxProgNameLen);
		default: 	vst_strncpy(text, "default", kVstMaxProgNameLen);
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