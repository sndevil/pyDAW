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
#include <ctime>
#include <chrono>



///// Sndfile reads data in range -32767 to 32767
// buffer[2x] = right
// buffer[2x+1] = left
#define BufferSIZE 2048


using std::cout;

void Bounce_to_file(const char* path, mixer m, sf_count_t start, sf_count_t length);

int main(int argc, const char * argv[]) {
    cout<<"Started\n";
    const char * fname = "/Users/mohammadreza/Developer/pyDAW/sounds/1.wav" ;
    const char * fread = "/Users/mohammadreza/Developer/pyDAW/sounds/ES.wav";
    mixer m;
    cout<<"here\n";
    m.AddTrack(fread,0);
    cout<<"Mixer Created\n";
    Bounce_to_file(fname,m, 0, m.GetTotalFrames());
    return 1;
}

void Bounce_to_file(const char* path, mixer m, sf_count_t start, sf_count_t length)
{
    using namespace std::chrono;
    
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    int channels = 2;
    SndfileHandle fout = SndfileHandle(path,SFM_WRITE,SF_FORMAT_WAV|SF_FORMAT_PCM_16,channels,44100);
    sf_count_t counter = start;
    sf_count_t total = length*channels;
    while (!m.eof)
    {
        double* buffer = m.Getbuffer(counter);
        fout.writef(buffer,BufferSIZE/channels);
        counter+=BufferSIZE;
        if (counter >= total -1)
        {
            cout<<"Broke\n";
            break;
        }
        if (counter %50000==0)
        {
            double progress = (double)counter/total*100;
            cout<<progress<<"%\n";
        }
    }
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    
    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
    
    cout << "It took me " << time_span.count() << " seconds.";
    cout << std::endl;
    
}
