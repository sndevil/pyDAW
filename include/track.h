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
    short Getsample(int i);
    short* Getbuffer();
    
private:
    double pan = +1,volume = 1;
    SndfileHandle file;
    short* buffer;
    
    
};