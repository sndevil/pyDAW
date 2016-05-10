//
//  functions.cpp
//  
//
//  Created by Mohammadreza Rezaei on 5/9/16.
//
//
#include <math.h>
#include "functions.h"
#include <stdio.h>
#include <iostream>
#include <string.h>

#define PI 3.14159265


void fft(short* input, short* outputr,short*outputi , int size)
{
    double r;
    double im;
    double tPi = PI*2;
    for (int i = 0; i < size; i++)
    {
        r = 0;
        im = 0;
        
        for (int j = 0 ; j < size ; j++)
        {
            short temp = tPi * (double)i*j/size;
            r += input[j] * cos(temp);
            im -= input[j] * sin(temp);
        }
        outputr[i] = r;//outputr[size-i-1] = r;
        outputi[i] = im;
        //outputi[size-i-1] = -im;
    }
}

void fft_stereo(short* input, double* outputr,double*outputi , int size)
{
    double r;
    double im;
    double tPi = PI*2 / size;
    for (int i = 0; i < size; i+=2)
    {
        r = 0;
        im = 0;
        
        for (int j = 0 ; j < size ; j+=2)
        {
            short temp = tPi * (double)i*j;
            r += (double)input[j]/32767 * cos(temp);
            im -= (double)input[j]/32767 * sin(temp);
        }
        outputr[i] = r;//outputr[size-i-1] = r;
        outputi[i] = im;
        //outputi[size-i-1] = -im;
    }
    for (int i = 1; i < size; i+=2)
    {
        r = 0;
        im = 0;
        
        for (int j = 1 ; j < size ; j+=2)
        {
            short temp = tPi * (double)i*j;
            r += (double)input[j]/32767 * cos(temp);
            im -= (double)input[j]/32767 * sin(temp);
        }
        outputr[i] = r;//outputr[size-i-1] = r;
        outputi[i] = im;
        //outputi[size-i-1] = -im;
    }
}

void fft2(double* input ,double* inputi, int size)
{
    int N = size/2;
    if (size > 2)
    {
        fft2(input,inputi,N) ;
        fft2(input+N ,inputi+N,N);
    }
    double wtemp,tempr,tempi,wr,wi,wpr,wpi;
    wtemp = sin(PI/N);
    wpr = -2.0*wtemp*wtemp;
    wpi = -sin(2*PI/N);
    wr = 1.0;
    wi = 0.0;
    for (unsigned i=0; i<N; i++) {
        tempr = input[i+N]*wr - inputi[i+N]*wi;
        tempi = input[i+N]*wi + inputi[i+N]*wr;
        input[i+N] = input[i]-tempr;
        inputi[i+N] = inputi[i]-tempi;
        input[i] += tempr;
        inputi[i] += tempi;
        
        wtemp = wr;
        wr += wr*wpr - wi*wpi;
        wi += wi*wpr + wtemp*wpi;
    }
}

void Reverse_fft(short* inputr, short* inputi, short* out, int size)
{
    double tPi = PI * 2;
    double r;
    for (int i = 0; i < size; i++)
    {
        r = 0;
        for (int j = 0; j < size; j++)
        {
            double temp = tPi * (double)i*j/size;
            r += inputr[j]*cos(temp) - inputi[j]*sin(temp);
        }
        out[i] = r / size * 32767;
    }
}

void Reverse_fft_stereo(double* inputr, double* inputi, short* out, int size)
{
    
    double tPi = PI * 2/size;
    double r;
    for (int i = 0; i < size; i+=2)
    {
        r = 0;
        for (int j = 0; j < size; j+=2)
        {
            double temp = tPi * (double)i*j;
            r += inputr[j]*cos(temp) - inputi[j]*sin(temp);
        }
        out[i] = r / size * 32767;
    }
    for (int i = 1; i < size; i+=2)
    {
        r = 0;
        for (int j = 1; j < size; j+=2)
        {
            double temp = tPi * (double)i*j;
            r += inputr[j]*cos(temp) - inputi[j]*sin(temp);
        }
        out[i] = r / size * 32767;
    }
}


void equaliser(short* inputr, short* inputi,const int size)
{
    //Cut between 1khz and 2khz
    double equaliser[size];
    for (int i = 0 ; i < size; i ++)
    {
        if (i < 43 || (i > 84 && i < 510) || i > 553)
            equaliser[i] = 1;
        else
            equaliser[i] = 0.1;
    }
    
    for (int i = 0; i < size; i++)
    {
        inputr[i] = inputr[i] * equaliser[i];
        inputi[i] = inputi[i] * equaliser[i];
    }
    
}

void Highpass(int freq, int gain,int bw,int SampleRate,double* inputr, double* inputi,const int size)
{
    double filter[size];
    double alpha = bw / 4.605;
    double deltaf = SampleRate / size;
    for (int i = 1 ; i <= size; i ++)
    {
        if (i*deltaf < freq - 2*bw)
            filter[i-1] = 0;
        else
            filter[i-1] = gain * (1 - exp(-(i * deltaf) / alpha));
        
        if (filter[i-1] < 0)
            filter[i-1] = 0;
    }
    
    for (int i = 0; i < size; i+=2)
    {
        inputr[i] = inputr[i] * filter[i];
        inputr[i+1] = inputr[i+1]*filter[i];
        inputi[i] = inputi[i] * filter[i];
        inputi[i+1] = inputi[i+1] * filter[i];
    }
    
    
}