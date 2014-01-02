#include "audioeffectx.h"
#define NUM_PARAMS 0

class autorec : public AudioEffectX {
public:
	autorec(audioMasterCallback audioMaster);
	~autorec();

	void processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames);
};
