#include "audioeffectx.h"
#include <iostream>
#include <vector>
#include "writewav.h"

#define NUM_PARAMS 2	//Update this whenever adding visible parameters
#define MAX_REC_TIME 10 //Max record time the plugin supports (in seconds)
#define CHANNELS 2		//stereo plugin
#define SAMPLERATE 44100	//For now its fixed. Should get it from host
#define MAX_BUFFER_SIZE MAX_REC_TIME*SAMPLERATE //Each sample goes into the buffer

using namespace std;

void writeWAVData(const char* outFile, float* buf, size_t bufSize, int sampleRate, short channels);

class audiobuffer;

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
	//void resizeBuffer();
	char programName[kVstMaxProgNameLen + 1];
	enum parameterName {
		kPlay,
		kBufferLength
	};
	bool play = false, done = false, saved = false;
	//float bufferLen;
	enum eBufferLen {
		k5s = 5,
		k10s = 10
	} bufferLen;
	void resizeBuffer(vector<float> buf, int *cursor, eBufferLen newlen);
	int sec2samples(int seconds);
	// Buffer
	int sampleRate = 44100;
	int seconds = MAX_REC_TIME;
	int channels = CHANNELS;
	//float* buffer;
	vector<float> buffer;
	int bufsize = sampleRate* seconds;
	int cursor = 0, playCursor = 0;
	audiobuffer *buf;
};

//Each audiosample can have n channels (typically n=2)
struct audiosample_t {
	//vector<float> channel; // horrible CPU usage
	float left;				// vastly better
	float right;
};

//------------------------------------------------------------------------
// Takes in size (total numFrames), creates an audio buffer with methods:
// insertSample(void *sample)
// getSample(index), getNextSample() - loop
// resize()
// cannot automatically resize - application has to call resize, otherwise index out of bounds
// freeze() - will resize using only play cursor, not the actual buffer. 
class audiobuffer
{
	//friend class autorec;	//To allow autorec to access private methods
public:
	audiobuffer(unsigned int size, unsigned int channels);
	~audiobuffer() {}
	void insertSample(audiosample_t *fsample);
	audiosample_t getSample(unsigned int index);
	audiosample_t getNextSample();
	int resize(unsigned int newsize);
	unsigned int size();
private:
	unsigned int cursor, last;
	unsigned int buffersize;
	vector<audiosample_t> sample;
};

