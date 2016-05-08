#include <sndfile.hh>
#include <cstdio>

#define Maxfiles 10

class track
{
public:
    track(const char* filename);
    void SetPan(double a);
    void SetVolume(double a);
    void Process();
    short* Getsample(sf_count_t i);
    short* Getbuffer();
    sf_count_t GetTotalFrames();
    bool eof = false;
    
private:
    void Readbuffer();
    double pan = 0,volume = 1;
    sf_count_t Totalframes,Currentframe,FrameOffset;
    SndfileHandle file;
    short* buffer;
    
    
};