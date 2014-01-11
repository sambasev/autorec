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

	buf = new audiobuffer(MAX_REC_TIME * sampleRate, channels);
}

autorec::~autorec() {
	//__asm int 3;	//assembly interrupt (break when destructor is called)
	if (buf) {
		delete buf;
	}
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
		while (--sampleFrames >= 0) 
		{
			audiosample_t bufsample = buf->getNextSample();
			(*out1++) = bufsample.left;
			(*out2++) = bufsample.right;
		}
	}
	else {
		while (--sampleFrames >= 0)
		{
			audiosample_t fsample;
			fsample.left = *in1;
			fsample.right = *in2;
			buf->insertSample(&fsample);
			(*out1++) = (*in1++);
			(*out2++) = (*in2++);
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
			if (value == 1)
			 {
				play = true;
				if (!saved) {
					//writeWAVData("C:\\Users\\samba_000\\Desktop\\myFile.wav", buffer.data(), buffer.size()*sizeof(float), 44100, 2);
					saved = true;
				}
			 }
			else if (value == 0)
			{
				play = false;		
			}
		}; break;
		case kBufferLength:
		{
			if (value == 0) {
				  bufferLen = k5s;
				  buf->resize(bufferLen*sampleRate);
			}
			else if (value == 1) {
				  bufferLen = k10s;
				  buf->resize(bufferLen*sampleRate);
			}
		}; break;	
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
			if (bufferLen==k5s)
				return 0; //Temporary. param to float conversion?
			else
				return 1;
		};  break;
		default: 
			return 0;
	}
}

void autorec::getParameterLabel(VstInt32 index, char* label){
	switch (index) {
		case kPlay:			vst_strncpy(label, "", kVstMaxParamStrLen); break;
		case kBufferLength: vst_strncpy(label, "sec", kVstMaxParamStrLen); break;
		default:			vst_strncpy(label, "units", kVstMaxParamStrLen); break;
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
			if (bufferLen == k10s) 
				vst_strncpy(text, "10", kVstMaxParamStrLen); 
			else
				vst_strncpy(text, "5", kVstMaxParamStrLen); 
		}; break;
		default:vst_strncpy(text, "Default", kVstMaxParamStrLen); break;
	}
}

void autorec::getParameterName(VstInt32 index, char* text){
	switch (index){
		case kPlay: 			vst_strncpy(text, "Play", kVstMaxProgNameLen); break;
		case kBufferLength: 	vst_strncpy(text, "Buffer:", kVstMaxProgNameLen); break;
		default: 				vst_strncpy(text, "default", kVstMaxProgNameLen); break;
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

int autorec::sec2samples(int seconds) {
	return seconds*sampleRate*channels;
}


// audiobuffer methods

audiobuffer::audiobuffer(unsigned int size, unsigned int channels) {
	cursor = 0; last = 0; buffersize = MAX_BUFFER_SIZE;
	if (size && channels && (size <= MAX_BUFFER_SIZE) && (channels <= CHANNELS)) {
		audiosample_t temp;
		memset(&temp, 0, sizeof(audiosample_t));
		sample.assign(size, temp);
		buffersize = size;
	}
};

//insertSample(float **fsample) - inserts sample and advances cursor. 
//Wraps around cursor if necessary.
void audiobuffer::insertSample(audiosample_t *fsample) {
	int x = sample.size();
	if (fsample) {
		last = cursor = cursor % buffersize;			//Loop around
		sample[cursor].left = fsample->left;
		sample[cursor].right = fsample->right;
		cursor++;
	}
}

audiosample_t audiobuffer::getSample(unsigned int index) {
	return sample[index % buffersize];
}

//Returns the next sample (oldest)
audiosample_t audiobuffer::getNextSample() {
	audiosample_t nextsample = sample[last%buffersize];
	last = ++last % buffersize;
	return nextsample;
}

//cursor points to oldest sample
//TODO: Critical test - resize from 5s to 10s to 5s multiple times quickly (crashes)
//Specific test - PLAY set to ON, then change buffersize rapidly, then PLAY set to OFF
// sample.resize() - A race condition ? before resizing is done and play toggling?
int audiobuffer::resize(unsigned int newsize) {
	//buffersize = this->sample.size();
	if (newsize == buffersize) {			// newsize == oldsize 
		return 0;						
	}
	if (newsize > MAX_BUFFER_SIZE) {		// Error
		return -1;
	}
	if (newsize > buffersize) {				// Expand
		if (this->sample.size() == newsize) {		// Optimization
			unsigned int pos = cursor, x = 0, y = buffersize;
			unsigned int diff = buffersize - cursor;
			unsigned int newpos = newsize - diff;
			audiosample_t silence;
			silence.left = silence.right = 0;
			while (y % newsize) {
				sample[y++] = silence;		// init new samples
			}
			while (newpos % newsize) {
				sample[newpos++] = sample[pos];
				sample[pos++] = silence;
			}
			buffersize = newsize;
			return 1;
		}
		vector<audiosample_t> newbuf;
		audiosample_t temp;
		memset(&temp, 0, sizeof(audiosample_t));
		newbuf.assign(newsize, temp);
		unsigned int pos = cursor, x = 0, y = 0; 
		while (pos % buffersize){		// copy oldest samples to new buffer
			newbuf[x++] = sample[pos++];
		}
		while (y < cursor) {			// append the rest to new buffer
			newbuf[x++] = sample[y++];
		}
		cursor = x;						// update cursor (used to record) and last (used to play)
		last = 0;						// oldest sample is at the start of the new buffer
		buffersize = newsize;
		sample.swap(newbuf);			
		return 1;
	}
	if (newsize < buffersize) {			// Shrink (fake)
		//sample.resize(newsize);
		if (cursor == newsize) {
			last = cursor = 0;
			buffersize = newsize;
			return 1;
		}
		if (cursor > newsize) {
			unsigned int diff = cursor - newsize, start =  0, x = cursor;
			while (start % newsize) {
				sample[start++] = sample[diff++];		//bug
			}
			last = cursor = 0;
			buffersize = newsize;
			return 1;
		}
		if (cursor < newsize) {
			unsigned int remaining = newsize - cursor, x = cursor;
			unsigned int diff = buffersize - remaining; //buffersize == oldsize
			while (x % newsize) {
				sample[x++] = sample[diff++];
			}
			last = cursor;
			buffersize = newsize;						// cursor already at oldest
			return 1;
		}
	}
	return 0;	
}

unsigned int audiobuffer::size() {
	return this->buffersize;
}