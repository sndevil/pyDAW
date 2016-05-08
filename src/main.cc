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
#include "track.h"

#define		BUFFER_LEN		1024000



///// Sndfile reads data in range -32767 to 32767
// buffer[2x] = right
// buffer[2x+1] = left


using std::cout;

int main(int argc, const char * argv[]) {
    const char * fname = "/Users/mohammadrezarezaei/1.wav" ;
    const char * fread = "/Users/mohammadrezarezaei/Documents/Projects/DAW/Test.wav";
    
    /*
    SndfileHandle fin = SndfileHandle(fread);
    static short buffer [BUFFER_LEN] ;
    fin.read(buffer,BUFFER_LEN);
    short min=0, max=0;
    
    double pan = +1;
    
    
    
    for (int i = 0; i < 1024000;i++)
    {
        pan = sin(((double)i/20480)*3.14159265);
        if (i%2 == 0)
            buffer[i] *= double(pan + 1) / 2;//sin(((double)i/204800) * 3.14159265);
        else
            buffer[i] *= double(pan - 1) / -2;

    }
    */
    
    track t(fread);
    t.SetPan(-1);
    SndfileHandle fout = SndfileHandle(fname,SFM_WRITE,SF_FORMAT_WAV|SF_FORMAT_PCM_24,2,44100);
    fout.write(t.Getbuffer(),BUFFER_LEN);
    puts ("Done.\n") ;
    return 0;
}
