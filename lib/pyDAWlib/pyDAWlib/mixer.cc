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

void mixer::AddTrack(char* filename,sf_count_t position)
{
    tracks[trackcount].init(filename);
    tracks[trackcount].PositionInLine=position;
	//cout<<"initing EQ\n";
	//EQ* eq = new EQ();
	//cout<<"EQ Constructor called\n";
	//double eqbands[6] = {500,500,-1,2000,1000,-1};
	//eq->SaveBands(eqbands,2,tracks[trackcount].SampleRate,BufferSIZE/4+1);
	//cout<<"EQ band saved\n";
	//tracks[trackcount].AddFreqEffect(eq, EQEffect);
	//cout<<"EQ Added\n";
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
        //int posmono = FrameOffsetMono+ i/2;
        //cout<<"position:"<<pos<<"\n";
        buffer[i] = 0;
        buffer[i+1] = 0;
        for (int j = 0; j < trackcount;j++)
        {
            int mul = (tracks[j].channels ==1)?2:2;
            if (pos > tracks[j].GetTotalFrames()*mul + tracks[j].PositionInLine || pos < tracks[j].PositionInLine)
            {
                //cout<<"Ended\n";
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
        //double* input = tracks[0].Getsample(pos);//-tracks[0].PositionInLine);
        //buffer[i] +=input[0];
        //buffer[i+1] +=input[1];
    }
    
    //for (int i = 0; i)
    FrameOffset+= BufferSIZE;
    //FrameOffsetMono+= BufferSIZE/2;
    
}
void mixer::Process()
{
    
}

double* mixer::Getsample(sf_count_t i)
{
    double* out;
    if (i > Totalframes*2)
    {
        cout<<"Eof triggered "<< i <<"\n";
        eof = true;
    }
    else
    {
        //cout<<"entered for i:" << i <<"\n";
        int offset = (int)i;// / BufferSIZE);
        
        int index = i % BufferSIZE;
        //cout<<"Frameoffset: " << FrameOffset << " offset: " <<offset <<"\n";
        if ( FrameOffset - offset == 0)
        {
            //cout<<"Buffer Read for i:" << i <<"\n";
            //FrameOffset = offset;
            //Currentframe = file.seek((int)i/2,SF_SEEK_SET);
            Readbuffer();
            //cout<<"Frameoffset: " <<FrameOffset<<"\n";
        }
        if (eof)
            eof = false;
        out = new double[2];
        out[0] = buffer[index];
        out[1] = buffer[index+1];
    }
    return out;
}
double* mixer::Getbuffer()
{
    return buffer;
}
sf_count_t mixer::GetTotalFrames()
{
    return Totalframes;
}
