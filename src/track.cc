#include <sndfile.hh>
#include <cstdio>
#include <iostream>
#include <math.h>
#include "track.h"
#include "functions.h"

using std::cout;
#define BufferSIZE 3000

track::track(const char* filename)
{
    file = SndfileHandle(filename);
    
    this->FrameOffset = 0;
    this->Currentframe = 0;
    this->Totalframes = file.seek(0,SF_SEEK_END);
    this->PositionInLine = 0;
    file.seek(0,SF_SEEK_SET);
    this->buffer = new short[BufferSIZE];
    Readbuffer();
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
    //double Gr,Gl;
    //cout<<"Buffer after process: ";
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
        //cout<<buffer[i]<<" ,";
    }
    
    for (int i = 0; i < BufferSIZE;i++)
        buffer[i] = 32767*sin(2*3.14159265/(double)BufferSIZE);
    fft(buffer,BufferSIZE);
    
    for (int i = 0; i < BufferSIZE;i++)
        cout<<buffer[i]<<" _ ";
    
    
    ////// Insert Effects here
}

short* track::Getsample(sf_count_t i)
{
    short* out;
    if (i > Totalframes*2)
        eof = true;
    else
    {
        //cout<<"entered for i:" << i <<"\n";
        int offset = (int)i/2;// / BufferSIZE);
        
        int index = i % BufferSIZE;
        //cout<<"Frameoffset: " << FrameOffset << " offset: " <<offset <<"\n";
        if ( FrameOffset - offset == 0 )
        {
            //cout<<"Buffer Read for i:" << i <<"\n";
            //FrameOffset = offset;
            //Currentframe = file.seek((int)i/2,SF_SEEK_SET);
            Readbuffer();
        }
        if (eof)
            eof = false;
        out = new short[2];
        out[0] = buffer[index];
        out[1] = buffer[index+1];
    }
    return out;
}

short* track::Getbuffer()
{
    return buffer;
}

sf_count_t track::GetTotalFrames()
{
    return Totalframes;
}

void track::Readbuffer()
{
    sf_count_t read = file.readf(buffer,BufferSIZE/2);
    Process();
    FrameOffset += read;
}