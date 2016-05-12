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



///// Sndfile reads data in range -32767 to 32767
// buffer[2x] = right
// buffer[2x+1] = left


using std::cout;

int main(int argc, const char * argv[]) {
    const char * fname = "/Users/mohammadrezarezaei/1.wav" ;
    const char * fread = "/Users/mohammadrezarezaei/Documents/Projects/DAW/Test.wav";
    
    //cout<<"Constructing t\n";
    track t(fread);
    
    //cout<<"Constructing fout\n";
    SndfileHandle fout = SndfileHandle(fname,SFM_WRITE,SF_FORMAT_WAV|SF_FORMAT_PCM_16,2,44100);
    sf_count_t counter = 0;
    sf_count_t total = t.GetTotalFrames()*2;
    //cout<<"Entering While\n";
    
    while (!t.eof)
    {
        //cout<<"Writing Started\n";
        fout.writef(t.Getsample(counter),1);
        //cout<<"Writing done\n";
        counter+=2;
        if (counter >= total -2)
            break;
        if (counter %100000==0)
        {
            double progress = (double)counter/total*100;
            cout<<progress<<"%\n";
        //    if (progress>1)
        //        break;
        }
    }

    puts ("Done.\n") ;
    return 0;
}
