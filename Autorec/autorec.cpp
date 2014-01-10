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
	//buffer.assign(bufsize, 0.0);	//Init bufsize samples with 0.0
	//bufferLen = k5s;		// Default buffer is 5 seconds
	//playCursor = cursor = (MAX_REC_TIME - bufferLen) * sampleRate * channels;

	buf = new audiobuffer(MAX_REC_TIME * sampleRate, channels);
}

autorec::~autorec() {
	//__asm int 3;	//assembly interrupt (break when destructor is called)
	//buffer.clear();	
	//buffer.resize(0);  //Necessary?
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
			//(*out1++) = buffer[playCursor++];
			//(*out2++) = buffer[playCursor++];
			playCursor++;
			audiosample_t bufsample = buf->getNextSample();
			(*out1++) = bufsample.left;
			(*out2++) = bufsample.right;
			//(*out1++) = (*in1++);
			//(*out2++) = (*in2++);
		}
	}
	else {
		done = false; saved = false;		//New material has been recorded. Refresh the play cursor to reflect this state
		while (--sampleFrames >= 0)
		{
			//buffer[cursor++] = (*in1);
			//buffer[cursor++] = (*in2);
			cursor++;
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
			if (value == 0) 
				bufferLen = k5s;
			else if (value == 1)
				bufferLen = k10s; 
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
			if (bufferLen == k5s) 
				vst_strncpy(text, "5", kVstMaxParamStrLen); 
			else
				vst_strncpy(text, "10", kVstMaxParamStrLen); 
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

//Reads current buffer setting (updated by host/user)
//Updates buffer, and bufsize
//TODO:Make it modulr - give old size and new size?

//void autorec::resizeBuffer(){
//	int oldBufsize = bufsize;
//	int newBufsize;
//	switch (bufferLen)
//	{
//		case k5s: seconds = 5; break;
//		case k10s: seconds = 10; break;
//	}
//	newBufsize = sampleRate * seconds * channels;
//	if (oldBufsize == newBufsize) return;
//	bufsize = newBufsize;
//	if (newBufsize > oldBufsize)			//Expand
//	{
//		buffer.resize(newBufsize, 0.0);	
//		return;
//	}
//	if (newBufsize < oldBufsize)			//Shrink
//	{
//		buffer.resize(newBufsize, 0.0);
//		if (playCursor > oldBufsize)
//			playCursor -= oldBufsize;
//		if (cursor > oldBufsize)
//			cursor -= oldBufsize;
//	}
//}


//Copies oldBuffer to newBuffer, modifies cursor and newBuffer.
//void autorec::resizeBuffer(oldBuffer, newBuffer, cursor)

//Buffer(largest) and manipulating cursor alone
//enum type should take care of non-enum values during compilation

// Test cases (c - cursor):
//Grow
//  old - c 1 2 3 4 5    // new - 1 2 3 4 5 c 0 0 0 0   
//  old - 1 2 3 4 c 5    // new - 5 1 2 3 4 c 0 0 0 0
//Shrink:
//  old - 1 2 3 4 5 6 7 c 8 // new - 4 5 6 7 c
//  old - 1 c 2 3 4 5 6 7 8 // new - 6 7 8 1 c

void autorec::resizeBuffer(vector<float> buf, int *cursor, eBufferLen newlen){
	int x, c, newcursor = 0;
	x = c = *cursor;
	if (bufferLen == newlen) {
		return;
	}
	//Grow
	if (newlen > bufferLen) {
		//copy cursor to end of old buffer to start of new buffer
		for (; x < buf.size(); x++) {
			buf[newcursor++] = buf[x];
		}
		//append start to cursor to new buffer
		for (int y = sec2samples(k10s); y < c; y++) {
			buf[newcursor++] = buf[y];
		}
		//clear new buffer end and update cursor
		for (int y = newcursor; y < buf.size(); y++) {
			buf[y] = 0;
		}
		*cursor = newcursor;
	}
	//Shrink
	if (newlen < bufferLen) {
		//copy end-remaining to start of new buffer
		//append start to cursor to end of new buffer
	}
}

int autorec::sec2samples(int seconds) {
	return seconds*sampleRate*channels;
}

audiobuffer::audiobuffer(unsigned int size, unsigned int channels) {
	cursor = 0; buffersize = MAX_BUFFER_SIZE;
	if (size && channels && (size <= MAX_BUFFER_SIZE) && (channels <= CHANNELS)) {
		audiosample_t temp;
		temp.left = temp.right = 0;
		sample.assign(size, temp);
		buffersize = size;
	}
};

//insertSample(float **fsample) - inserts sample and advances cursor. 
//Wraps around cursor if necessary.
void audiobuffer::insertSample(audiosample_t *fsample) {
	int x = sample.size();
	if (fsample) {
		cursor = cursor % buffersize;			//Loop around
		sample[cursor].left = fsample->left;
		sample[cursor].right = fsample->right;
		cursor++;
	}
}

audiosample_t audiobuffer::getSample(unsigned int index) {
	if (index < buffersize) {
		return sample[index];
	}
	return sample[0];
}

audiosample_t audiobuffer::getNextSample() {
	cursor = ++cursor % buffersize;
	return sample[cursor];
}

int audiobuffer::resize(unsigned int newsize) {
	if (newsize == buffersize) {		// Newsize = oldsize
		return 0;						
	}
	if (newsize > MAX_BUFFER_SIZE) {	// Error
		return -1;
	}
	if (newsize > buffersize) {			// Expand
		for (int i = buffersize; i < newsize; i++) {
			sample[i].left = 0;	
			sample[i].right = 0;
		}								// Init new samples
		buffersize = newsize;
	}
	if (newsize < buffersize) {			// Shrink
		buffersize = newsize;
	}
	return 0;	// SUCCESS
}