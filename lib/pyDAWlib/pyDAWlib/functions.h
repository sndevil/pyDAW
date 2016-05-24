//
//  functions.h
//  
//
//  Created by Mohammadreza Rezaei on 5/9/16.
//
//

#include <stdio.h>
#include "fftw3.h"

void fft(short* input, short* outputr,short*outputi , int size);
void fft_stereo(double* input, double* outputr,double*outputi , int size);
void fft2(double* input,double*inputi , const int size);
void equaliser(double* bands,int bandcount,int SampleRate,fftw_complex* buffer,const int size);
void Reverse_fft(short* inputr, short* inputi, short* out, int size);
void Reverse_fft_stereo(double* inputr, double* inputi, double* out, int size);
void Highpass(int freq, int gain,int bw,int SampleRate,fftw_complex* buffer,const int size);
void Lowpass(int freq, int gain,int bw,int SampleRate,double* inputr, double* inputi,const int size);
void Limiter(double cut, double gain,double* input, const int size);