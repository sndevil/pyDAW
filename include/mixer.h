#include <sndfile.hh>
#include "track.h"
#include <cstdio>

class mixer
{
public:
    mixer();
    void AddTrack(const char* filename,sf_count_t position, double volume, double pan);
    void Process();
    double* Getsample(sf_count_t i);
    double* Getbuffer(sf_count_t i);
    sf_count_t GetTotalFrames();
    bool eof = false;
        track* tracks;
private:
    void Readbuffer();
    double pan = 0,volume = 1;
    sf_count_t Totalframes=0,Currentframe=0,FrameOffset=0,FrameOffsetMono=0;
    int trackcount = 0;
    double* buffer;
    
    
};
