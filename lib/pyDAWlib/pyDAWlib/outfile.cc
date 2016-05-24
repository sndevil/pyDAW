#include "stdafx.h"
#include "outfile.h"
#include <iostream>


outfile::outfile(char* path,int format, int channels, int Samplerate)
{
	file = SndfileHandle(path,SFM_WRITE,format,channels,Samplerate);
}
void outfile::write(double* buffer, int count)
{
	//std::cout<<"output "<<count<<" buffer: "<<buffer[0]<<" , " << buffer[1]<<"\n";
	file.writef(buffer,count);
}

void outfile::close()
{
	file.~SndfileHandle();
}