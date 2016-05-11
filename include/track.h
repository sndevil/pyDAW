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
    double* Getsample(sf_count_t i);
    double* Getbuffer();
    sf_count_t GetTotalFrames();
    bool eof = false;
    
private:
    void Readbuffer();
    double pan = 0,volume = 1;
    sf_count_t Totalframes=0,Currentframe=0,FrameOffset=0;
    sf_count_t PositionInLine=0;
    SndfileHandle file;
    double* buffer;
    
    
};