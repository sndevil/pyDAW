//
//  functions.h
//  
//
//  Created by Mohammadreza Rezaei on 5/9/16.
//
//

#include <stdio.h>

void fft(short* input, short* outputr,short*outputi , int size);
void fft_stereo(short* input, double* outputr,double*outputi , int size);
void fft2(double* input ,double* inputi, int size);
void Reverse_fft(short* inputr, short* inputi, short* out, int size);
void Reverse_fft_stereo(double* inputr, double* inputi, short* out, int size);
void equaliser(short* inputr, short* inputi,int size);
void Highpass(int freq, int gain,int bw,int SampleRate,double* inputr, double* inputi,const int size);