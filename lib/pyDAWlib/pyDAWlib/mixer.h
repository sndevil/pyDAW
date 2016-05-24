#include <sndfile.hh>
#include "track.h"
#include <cstdio>

class mixer
{
public:
    mixer();
    void AddTrack(char* filename,sf_count_t position);
    void Process();
    double* Getsample(sf_count_t i);
    double* Getbuffer();
    sf_count_t GetTotalFrames();
    bool eof;
    track* tracks;
private:
    void Readbuffer();
    double pan,volume;
    sf_count_t Totalframes,Currentframe,FrameOffset,FrameOffsetMono;
    int trackcount;
    double* buffer;
    
    
};