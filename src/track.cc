#include <sndfile.hh>
#include <cstdio>
#include <iostream>
#include <math.h>
#include "track.h"
#include "functions.h"

using std::cout;
#define BufferSIZE 4096

track::track(const char* filename)
{
    //cout<<"Entered Constructor\n";
    file = SndfileHandle(filename);
    //cout<<"Opened File\n";
    this->FrameOffset = 0;
    this->Currentframe = 0;
    this->Totalframes = file.seek(0,SF_SEEK_END);
    this->PositionInLine = 0;
    file.seek(0,SF_SEEK_SET);
    this->buffer = new double[BufferSIZE];
    //cout<<"Reading buffer\n";
    Readbuffer();
    //cout<<"Buffer read\n";
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
    
    double fftr[BufferSIZE], ffti[BufferSIZE];
    double buffer2[BufferSIZE];
    double samplerate = file.samplerate();
    for (int i = 0; i < BufferSIZE;i++)
    {
        double t = (double)i / samplerate;
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

        //buffer[i] = 32767 * sin(2*3.14159265* 200 * t);
        //cout<<buffer[i]<<" ,";
    }
    
    
    fft_stereo(buffer, fftr,ffti, BufferSIZE);
    //for (int i = 0; i < BufferSIZE;i++)
    //{
    //    cout<<i<<" : " <<sqrt(pow(fftr[i],2)+pow(ffti[i],2))<<"\n";
    //}
    Highpass(10000, 1,100,samplerate,fftr, ffti,BufferSIZE);
    //double eqbands[6] = {2000,1000,-0.8,10000,2000,0.6};
    //equaliser(eqbands,2,file.samplerate(),fftr , ffti,BufferSIZE);


    //cout<<"EQ ended\n";
    Reverse_fft_stereo(fftr, ffti, buffer, BufferSIZE);
    //cout<<"ReverseFFT ended\n";
    //std::cout<<" eq done\n";
    Limiter(1, 0.99,buffer, BufferSIZE);
    //cout<<"Limiter ended\n";
    //double error= 0;
    //for (int i = 0; i < BufferSIZE;i++)
    //    error += abs(buffer2[i] - buffer[i]);
    //error/= BufferSIZE;
    //cout<<"Error: " << error<<"\n";
    //for (int i = 0; i < BufferSIZE;i++)
    //    buffer[i] = 32767*sin(2*3.14159265/(double)BufferSIZE);
    //fft(buffer,BufferSIZE);
    
    /*for (int i = 0; i < BufferSIZE;i++)
    {
        //cout<<i<<" : "<<buffer[i]<<"\n";
        if (buffer[i]>1||buffer[i]<-1)
            cout<<i<<":"<<buffer[i]<<"\n";
    }*/
    
}

double* track::Getsample(sf_count_t i)
{
    double* out;
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
        out = new double[2];
        out[0] = buffer[index];
        out[1] = buffer[index+1];
    }
    return out;
}

double* track::Getbuffer()
{
    return buffer;
}

sf_count_t track::GetTotalFrames()
{
    return Totalframes;
}

void track::Readbuffer()
{
    //cout<<"Entered Reading buffer\n";
    sf_count_t read = file.readf(buffer,BufferSIZE/2);
    Process();
    //cout<<"Process done\n";
    //cout<<"increasing frameoffset\n";
    //    cout<<"read:"<< read<<" Frameoffset: " <<FrameOffset<<" Processing\n";
    FrameOffset += read;
    //cout<<"Frameoffset: " <<FrameOffset<<"\n";
}