//
//  main.cpp
//  DAW
//
//  Created by Mohammadreza Rezaei on 4/29/16.
//  Copyright (c) 2016 Mohammadreza Rezaei. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <cstdio>
#include <sndfile.hh>
#include <cstring>
#include <math.h>
#include "mixer.h"



///// Sndfile reads data in range -32767 to 32767
// buffer[2x] = right
// buffer[2x+1] = left
#define BufferSIZE 2048


using std::cout;

void Bounce_to_file(const char* path, mixer m, sf_count_t start, sf_count_t length);

int main(int argc, const char * argv[]) {
    //cout<<"Started\n";
    const char * fname = "/Users/mohammadreza/Developer/pyDAW/sounds/1.wav" ;
    const char * fread = "/Users/mohammadreza/Developer/pyDAW/sounds/Esoteric.wav";
    mixer m;
    m.AddTrack(fread,0);
    //cout<<"Mixer Created\n";
    Bounce_to_file(fname,m, 0, m.GetTotalFrames());
    return 1;
}

void Bounce_to_file(const char* path, mixer m, sf_count_t start, sf_count_t length)
{
    SndfileHandle fout = SndfileHandle(path,SFM_WRITE,SF_FORMAT_WAV|SF_FORMAT_PCM_16,2,44100);
    sf_count_t counter = start;
    sf_count_t total = length*2;
    //cout<<"Entering While "<< total <<"\n";
    
    while (!m.eof)
    {
        //cout<<"Writing Started\n";
        //fout.writef(
        double* buffer = m.Getbuffer(counter);
        fout.writef(buffer,BufferSIZE);
        //cout<<"Writing done\n";
        counter+=BufferSIZE;
        if (counter >= total -1)
        {
            cout<<"Broke\n";
            break;
        }
        if (counter %100000==0)
        {
            double progress = (double)counter/total*100;
            cout<<progress<<"%\n";
        }
    }
    
    puts ("Done.\n") ;
}
