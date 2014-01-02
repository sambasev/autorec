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