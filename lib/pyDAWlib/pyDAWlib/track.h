#include <sndfile.hh>
#include <cstdio>
#include "effect.h"

#define Maxfiles 10

class track
{
public:
    track(char* filename);
    track();
    void init(char* filename);
    void SetPan(double a);
    void SetVolume(double a);
    void Process();
    void Process_Mono();
	void AddFreqEffect(effect* e, EffectType t);
	void AddTimeEffect(effect* e, EffectType t);

    double* Getsample(sf_count_t i);
    double* Getbuffer();
    sf_count_t GetTotalFrames();
    bool eof;
    bool mute;
    bool solo;
    int channels;
    double pan,volume;
	int SampleRate;
    sf_count_t PositionInLine;
private:
    void Readbuffer();
    sf_count_t Totalframes,Currentframe,FrameOffset;
    SndfileHandle file;
    double* buffer;
    effect** timeeffects;
	effect** freqeffects;
	int freqeffectscount;
    
};