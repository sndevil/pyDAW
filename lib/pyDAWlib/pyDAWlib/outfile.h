#include <sndfile.hh>


class outfile
{
public:
	outfile(char* path,int format, int channels, int Samplerate);
	void write(double* buffer, int count);
	void close();

private:
	SndfileHandle file;

};