#include <sndfile.hh>
#include <cstdio>
#include "track.h"

#define BufferSIZE 1024000
track::track(const char* filename)
{
    file = SndfileHandle(filename);
    buffer = new short[BufferSIZE];
    file.read(buffer,BufferSIZE);
}

void track::SetPan(double a)
{
    pan = a;
    Process();
}

void track::SetVolume(double a)
{
    volume = a;
    Process();
}

void track::Process()
{
    for (int i = 0; i < BufferSIZE;i++)
    {
        buffer[i] *= volume;
        if (i%2==0) //rightchannel
        {
            if (pan > 0)
                buffer[i]*=1-pan;
        }
        else //leftchannel
        {
            if (pan<0)
                buffer[i]*=pan+1;
        }
    }
}

short track::Getsample(int i)
{
    return buffer[i];
}

short* track::Getbuffer()
{
    return buffer;
}