#include "audioeffectx.h"

#define NUM_PARAMS 2	//Update this whenever adding visible parameters

class autorec : public AudioEffectX {
public:
	autorec(audioMasterCallback audioMaster);
	~autorec();

	void processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames);
//	virtual void processDoubleReplacing(double** inputs, double** outputs, VstInt32 sampleFrames);
	
	// Program
	virtual void setProgramName(char* name);
	virtual void getProgramName(char* name);

	// Parameters
	virtual void setParameter(VstInt32 index, float value);
	virtual float getParameter(VstInt32 index);
	virtual void getParameterLabel(VstInt32 index, char* label);
	virtual void getParameterDisplay(VstInt32 index, char* text);
	virtual void getParameterName(VstInt32 index, char* text);

	virtual bool getEffectName(char* name);
	virtual bool getVendorString(char* text);
	virtual bool getProductString(char* text);
	virtual VstInt32 getVendorVersion();

	//Local variables
private:
	char programName[kVstMaxProgNameLen + 1];
	enum parameterName {
		kRec,
		kBufferLength
	};
	bool rec, done=false;
	float bufferLen;
	enum eBufferLen {
		k5s,
		k10s,
		k30s
	};
	// Buffer
	int sampleRate = 44100;
	int seconds = 10;
	int channels = 2;
	float* buffer;
	int bufsize = sampleRate* seconds * channels;
	int cursor = 0, playCursor = 0;
};
