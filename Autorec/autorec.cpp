#include "autorec.h"

AudioEffect* createEffectInstance(audioMasterCallback audioMaster) {
	return new autorec(audioMaster);
}

autorec::autorec(audioMasterCallback audioMaster) :
AudioEffectX(audioMaster, 0, NUM_PARAMS) {

}

autorec::~autorec() {

}

void autorec::processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames) {
	//Actual processing
}

void autorec::processDoubleReplacing(double** inputs, double** outputs, VstInt32 sampleFrames){

}

void autorec::setProgramName(char* name){

}

void autorec::getProgramName(char* name){

}

void autorec::setParameter(VstInt32 index, float value){

}

float autorec::getParameter(VstInt32 index){
	return 0;
}

void autorec::getParameterLabel(VstInt32 index, char* label){

}

void autorec::getParameterDisplay(VstInt32 index, char* text){

}

void autorec::getParameterName(VstInt32 index, char* text){

}

bool autorec::getEffectName(char* name){
	return true;
}

bool autorec::getVendorString(char* text){
	return true;
}

bool autorec::getProductString(char* text){
	return true;
}

VstInt32 autorec::getVendorVersion(){
	return 0;
}