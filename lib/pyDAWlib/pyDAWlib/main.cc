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


using std::cout;

int main(int argc, const char * argv[]) {
    const char * fname = "/Users/mohammadrezarezaei/1.wav" ;
    const char * fread = "/Users/mohammadrezarezaei/Documents/Projects/DAW/guitar1.wav";
    const char * fread1 = "/Users/mohammadrezarezaei/Documents/Projects/DAW/guitar2.wav";
    const char * fread2 = "/Users/mohammadrezarezaei/Documents/Projects/DAW/Kick.wav";
    const char * fread3 = "/Users/mohammadrezarezaei/Documents/Projects/DAW/Snare.wav";
    const char * fread4 = "/Users/mohammadrezarezaei/Documents/Projects/DAW/Overhead.wav";
    mixer m;
    
    m.AddTrack(fread,150000);
    m.AddTrack(fread1,150000);//100000);
    m.AddTrack(fread2,0);
    m.AddTrack(fread3,0);
    m.AddTrack(fread4,0);
    m.tracks[0].pan = -1;
    m.tracks[1].pan = 1;
    //cout<<"Constructing t\n";
    //track t(fread);
    
    //cout<<"Constructing fout\n";
    SndfileHandle fout = SndfileHandle(fname,SFM_WRITE,SF_FORMAT_WAV|SF_FORMAT_PCM_16,2,44100);
    sf_count_t counter = 0;
    sf_count_t total = m.GetTotalFrames()*2;
    //cout<<"Entering While "<< total <<"\n";
    
    while (!m.eof)
    {
        //cout<<"Writing Started\n";
        fout.writef(m.Getsample(counter),1);
        //cout<<"Writing done\n";
        counter+=2;
        //cout<<"channels:"<<t.channels<<"\n";
        if (counter >= total -1)
        {
            cout<<"Broke\n";
            break;
        }
        if (counter %100000==0)
        {
            double progress = (double)counter/total*100;
            cout<<progress<<"%\n";
            //if (progress>10)
            //    break;
        }
    }

    puts ("Done.\n") ;
    return 0;
}
