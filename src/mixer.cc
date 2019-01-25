//
//  main.cpp
//  DAW
//
//  Created by Mohammadreza Rezaei on 4/29/16.
//  Copyright (c) 2016 Mohammadreza Rezaei. All rights reserved.
//
#include "Stdafx.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <sndfile.hh>
#include <cstring>
#include <math.h>
#include "mixer.h"

using std::cout;

#define BufferSIZE 2048
#define Maxfiles 10

mixer::mixer()
{
    buffer = new double[BufferSIZE]; 
    tracks = new track[Maxfiles];

    Totalframes = Currentframe = FrameOffset = FrameOffsetMono=0;
    eof = false;
    pan = 0;
    volume = 1;
    trackcount = 0;
}

void mixer::AddTrack(const char* filename,sf_count_t position, double volume, double pan)
{
    tracks[trackcount].init(filename);
    tracks[trackcount].PositionInLine=position;
    tracks[trackcount].SetVolume(volume);
    tracks[trackcount].SetPan(pan);
    sf_count_t end = position+tracks[trackcount].GetTotalFrames();
    if (Totalframes < end)
        Totalframes = end;
    trackcount++;
}

void mixer::Readbuffer()
{
    
    for (int i = 0;i < BufferSIZE;i+=2)
    {
        int pos = FrameOffset + i;
        buffer[i] = 0;
        buffer[i+1] = 0;
        for (int j = 0; j < trackcount;j++)
        {
            int mul = (tracks[j].channels ==1)?2:1;
            if (pos > tracks[j].GetTotalFrames()*tracks[j].channels + tracks[j].PositionInLine || pos < tracks[j].PositionInLine)
            {
                buffer[i] += 0;
                buffer[i+1] += 0;
            }
            else
            {
                if (tracks[j].channels == 1)
                {
                    double* input = tracks[j].Getsample((pos-tracks[j].PositionInLine)/2);
                    double dr,dl;
                    dl = input[0] * (tracks[j].pan+1)/2;
                    dr = input[0] * (tracks[j].pan-1)/-2;
                    buffer[i] += dr;
                    buffer[i+1] += dl;
                }
                else
                {
                    double* input = tracks[j].Getsample(pos-tracks[j].PositionInLine);
                    buffer[i] += input[0];
                    buffer[i+1] += input[1];
                }
            }
        }
    }
    
    
}
void mixer::Process()
{
    
}

double* mixer::Getsample(sf_count_t i=0)
{
    double* out;
    if (i > Totalframes*2)
    {
        cout<<"Eof triggered "<< i <<"\n";
        out = nullptr;
        eof = true;
    }
    else
    {        
        int offset = (int)i;
        
        int index = i % BufferSIZE;
        if ( FrameOffset - offset == 0)
            Readbuffer();
        if (eof)
            eof = false;
        out = new double[2];
        out[0] = buffer[index];
        out[1] = buffer[index+1];
    }
    return out;
}
double* mixer::Getbuffer(sf_count_t i=0)
{
    double* out;
    if (i > Totalframes*2)
    {
        cout<<"Eof triggered "<< i <<"\n";
        out = nullptr;
        eof = true;
    }
    else
    {
        FrameOffset = i;
        Readbuffer();
        if (eof)
            eof = false;
        out = buffer;
    }
    return out;
}
sf_count_t mixer::GetTotalFrames()
{
    return Totalframes;
}
