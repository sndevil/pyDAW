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

#define		BUFFER_LEN		1024000
using std::cout;

static void
create_file (const char * fname, int format)
{	static short buffer [BUFFER_LEN] ;
    
    SndfileHandle file ;
    int channels = 2 ;
    int srate = 48000 ;
    
    printf ("Creating file named '%s'\n", fname) ;
    
    file = SndfileHandle (fname, SFM_WRITE, format, channels, srate) ;
    
    memset (buffer, 0, sizeof (buffer)) ;
    
    file.write (buffer, BUFFER_LEN) ;
    
    puts ("") ;
    /*
     **	The SndfileHandle object will automatically close the file and
     **	release all allocated memory when the object goes out of scope.
     **	This is the Resource Acquisition Is Initailization idom.
     **	See : http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization
     */
} /* create_file */

static void
read_file (const char * fname)
{	static short buffer [BUFFER_LEN] ;
    
    SndfileHandle file ;
    
    file = SndfileHandle (fname) ;
    
    printf ("Opened file '%s'\n", fname) ;
    printf ("    Sample rate : %d\n", file.samplerate ()) ;
    printf ("    Channels    : %d\n", file.channels ()) ;
    
    file.read (buffer, BUFFER_LEN) ;
    
    puts ("") ;
    
    /* RAII takes care of destroying SndfileHandle object. */
} /* read_file */


int main(int argc, const char * argv[]) {
    const char * fname = "/Users/mohammadrezarezaei/1.wav" ;
    const char * fread = "/Users/mohammadrezarezaei/Music/Logic/Padway/Bounces/Jadde Mix 3.wav";
    
    SndfileHandle fin = SndfileHandle(fread);
    static short buffer [BUFFER_LEN] ;
    fin.read(buffer,BUFFER_LEN);
    
    for (int i = 10000; i < 12000;i++)
        cout<<buffer[i]<<" _ ";
    
    SndfileHandle fout = SndfileHandle(fname,SFM_WRITE,SF_FORMAT_WAV|SF_FORMAT_PCM_24,2,44100);
    fout.write(buffer,BUFFER_LEN);
    
    
    puts ("Done.\n") ;
    return 0;
}
