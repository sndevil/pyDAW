// pyDAWlib.h

#pragma once

#ifdef PYDAWLIB_EXPORTS
#define PYDAWLIB extern "C" __declspec(dllexport) 
#else
#define PYDAWLIB extern "C" __declspec(dllimport) 
#endif


//#include <sndfile.hh>
#include "fftw3.h"

using namespace System;

typedef void* Mixer;

typedef void* Outfile;

PYDAWLIB Mixer NewMixer();

PYDAWLIB void AddTrack(Mixer m,const wchar_t* path, long position);

PYDAWLIB void GetSample(Mixer m, long position, double* out);

PYDAWLIB long GetLength(Mixer m);

PYDAWLIB Outfile CreateOutfile(const wchar_t* path,int bps, int channels, int Samplerate);

PYDAWLIB void WriteOutfile(Outfile f,Mixer m, long position);

PYDAWLIB void CloseOutfile(Outfile f);

PYDAWLIB int test(int a);


