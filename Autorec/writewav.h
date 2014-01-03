/*

Source taken from Josh Parnell at http://joshparnell.com/blog/2013/03/21/how-to-write-a-wav-file-in-c/
Link: http://ltheory.com/blog/writeWAV.txt
Excerpt from his blog:
My code formatting in this blog is horrible, hence linking it as a separate file.Now all you do is call writeWAVData like this:

writeWAVData("mySound.wav", mySampleBuffer, mySampleBufferSize, 44100, 1);

Which would write a 44.1khz mono wav("CD quality").mySampleBuffer should be an array of signed shorts for 16 - bit sound, floats for 32 - bit(I think ? never tried writing 32 - bit audio), unsigned chars for 8 - bit.Since the function is templatized it automatically detects the format and takes care of the relevant fields in the WAV header.

Oh, and this only works on a little - endian machine, since WAV is expected to be little...but that probably doesn't matter to anyone these days...you're all running little endian...

*/

#include <fstream>

template <typename T>
void write(std::ofstream& stream, const T& t) {
	stream.write((const char*)&t, sizeof(T));
}

void writeWAVData(const char* outFile, float* buf, size_t bufSize,
	int sampleRate, short channels)
{
	std::ofstream stream(outFile, std::ios::binary);
	stream.write("RIFF", 4);
	write<int>(stream, 36 + bufSize);
	stream.write("WAVE", 4);
	stream.write("fmt ", 4);
	write<int>(stream, 16);
	write<short>(stream, 1);                                        // Format (1 = PCM)
	write<short>(stream, channels);                                 // Channels
	write<int>(stream, sampleRate);                                 // Sample Rate
	write<int>(stream, sampleRate * channels * sizeof(float)); // Byterate
	write<short>(stream, channels * sizeof(float));            // Frame size
	write<short>(stream, 8 * sizeof(float));                   // Bits per sample
	stream.write("data", 4);
	stream.write((const char*)&bufSize, 4);
	stream.write((const char*)buf, bufSize);
	stream.close();
}