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
    sf_count_t Totalframes,Currentframe,FrameOffset;
    sf_count_t PositionInLine;
    SndfileHandle file;
    double* buffer;
    
    
};