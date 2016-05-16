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
#include <fftw3.h>

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
            double temp = tPi * (double)i*j/size;
            r += input[j] * cos(temp);
            im -= input[j] * sin(temp);
        }
        outputr[i] = r;
        outputi[i] = im;
    }
}

void fft_stereo(double* input, double* outputr,double*outputi , int size)
{
    double r;
    double im;
    double tPi = PI*4 / size;
    int ci=0,cj=0;
    for (int i = 0; i < size; i+=2)
    {
        r = 0;
        im = 0;
        
        for (int j = 0 ; j < size ; j+=2)
        {
            double temp = tPi * (double)ci*cj;
            r += input[j] * cos(temp);
            im -= input[j] * sin(temp);
            cj++;
        }
        outputr[i] = r;
        outputi[i] = im;
        ci++;
    }
    ci = cj = 0;
    for (int i = 1; i < size; i+=2)
    {
        r = 0;
        im = 0;
        
        for (int j = 1 ; j < size ; j+=2)
        {
            double temp = tPi * (double)ci*cj;
            r += input[j] * cos(temp);
            im -= input[j] * sin(temp);
            cj++;
        }
        outputr[i] = r;
        outputi[i] = im;
        ci++;
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
        out[i] = r / size * 2;
    }
}

void Reverse_fft_stereo(double* inputr, double* inputi, double* out, int size)
{
    
    double tPi = PI * 4/size;
    double r;
    int ci=0,cj =0;
    for (int i = 0; i < size; i+=2)
    {
        r = 0;
        for (int j = 0; j < size; j+=2)
        {
            double temp = tPi * (double)ci*cj;
            r += inputr[j]*cos(temp) - inputi[j]*sin(temp);
            cj++;
        }
        out[i] = r / size * 2;
        ci++;
    }
    ci = cj = 0;
    for (int i = 1; i < size; i+=2)
    {
        r = 0;
        for (int j = 1; j < size; j+=2)
        {
            double temp = tPi * (double)ci*cj;
            r += inputr[j]*cos(temp) - inputi[j]*sin(temp);
            //std::cout<<"i: "<<i<<"j: "<<j<<"\n";
            cj++;
        }
        out[i] = r / size * 2;
        ci++;
    }
}

static double* tempbandsEQ;
static int bandcountEQ=0;
static bool EQSaved = false;
static double* tempfilterEQ;
void equaliser(double* bands,int bandcount,int SampleRate,fftw_complex* buffer,const int size)
{
    //bands { CenterFreq , BW , gain }
    
    //  gain * exp -(x - centerfreq)^2 / BW^2
    
    //double equaliser[size];
    double deltaf = SampleRate / size / 2;
    static int ftempEQ,gtempEQ,bwtempEQ;
    static bool EQSaved;
    static double* tempfilterEQ;
    static double* tempbandsEQ;
  
    //std::cout<<bandcount<<" , " <<bandcountEQ<<"\n";
    if (bandcount==bandcountEQ)
    {
        if (EQSaved)
        {
            if (tempbandsEQ != NULL)
            {
                for (int i = 0; i < bandcount * 3;i++)
                    if (tempbandsEQ[i] != bands[i])
                        EQSaved = false;
            }
            else
                EQSaved = false;
        }
    }
    else
        EQSaved = false;
    
    if (!EQSaved)
    {
        //std::cout<<"Saving EQ\n";
        int CF,BW;
        float Gain;
//    for (int i = 0; i < bandcount;i++)
        tempbandsEQ = new double[3*bandcount];
        tempfilterEQ = new double[size];
        bandcountEQ = bandcount;
        for (int i = 0 ; i <= size; i ++)
        {
            
            tempfilterEQ[i] = 1;
            if (i%2 ==0)
            {
                for (int j = 0; j < bandcount;j++)
                {
                    CF = tempbandsEQ[3*j]= bands[3*j];
                    BW = tempbandsEQ[3*j+1] = bands[3*j+1];
                    Gain = tempbandsEQ[3*j+2] = bands[3*j+2];
                    tempfilterEQ[i] += Gain * exp(-(pow(i*deltaf - CF,2)/pow(BW,2)));
                }
            }
            else
                tempfilterEQ[i] = tempfilterEQ[i-1];
            //std::cout<<i<<" : "<<tempfilterEQ[i]<< "\n";
        }
        EQSaved = true;
    }
   // std::cout<<"EQ run \n";
    for (int i = 0; i <= size; i++)
    {
        //std::cout<<i<<" : "<<tempfilter[i]<<"\n";
        buffer[i][0] *= tempfilterEQ[i];
        buffer[i][1] *= tempfilterEQ[i];
    }
    //std::cout<<"EQ Done\n";
}
//static int ftempHP=0,gtempHP=0,bwtempHP=0;
static bool HPSaved = false;
static double* tempfilterHP;
void Highpass(int freq, int gain,int bw,int SampleRate,fftw_complex* buffer,const int size)
{
    //const int S = size/2;
    static int ftemp,gtemp,bwtemp;
    static bool HPSaved;
    static double* tempfilter;
    if (freq!=ftemp || gain!=gtemp || bwtemp!=bw)
        HPSaved = false;
    if (!HPSaved)
    {
        //double filter[S];
        //double alpha = bw;
        bwtemp = bw;
        gtemp = gain;
        ftemp = freq;
        double deltaf = SampleRate / size / 2;
        tempfilter = new double[size+1];
        for (int i = 1 ; i <= size; i ++)
        {
            double tempval;
            if (i*deltaf < freq - 2*bw)
                tempval = 0;
            else
                tempval = gain * (3.4142 - exp(-((i * deltaf)-ftemp) / bwtemp))/3.4142;
        
            if (tempval < 0)
                tempval = 0;
            tempfilter[i-1] = tempval;
        }
        tempfilter[size] = tempfilter[size-1];
        HPSaved = true;
    }

    for (int i = 0; i <= size; i++)
    {
        //std::cout<<i<<" : "<<tempfilter[i]<<"\n";
        buffer[i][0] *= tempfilter[i];
        buffer[i][1] *= tempfilter[i];
    }
    //for (int i = 0; i <size;i++)
    //    std::cout<<i<<":"<<inputi[i]<<"\n";
    //std::cout<<"for escaped\n";
}

void Lowpass(int freq, int gain,int bw,int SampleRate,double* inputr, double* inputi,const int size)
{
    const int S = 2048;
    double filter[S];
    double alpha = bw;
    double deltaf = SampleRate / S / 2;
    for (int i = 1 ; i <= S; i ++)
    {
        if (i*deltaf > freq + 2*bw)
            filter[i-1] = 0;
        else
            filter[i-1] = gain * (3.4142 - exp(-(freq - (i * deltaf)) / alpha))/3.4142;
        
        if (filter[i-1] < 0)
            filter[i-1] = 0;
    }
    
    for (int i = 0; i < size/2; i+=2)
    {
        inputr[i] = inputr[i] * filter[i];
        inputr[size-i] = inputr[size-i] * filter[i];
        inputr[i+1] = inputr[i+1]*filter[i];
        inputr[size-i-1] = inputr[size-i-1] * filter[i];
        inputi[i] = inputi[i] * filter[i];
        inputi[size - i] = inputi[size - i] * filter[i];
        inputi[i+1] = inputi[i+1] * filter[i];
        inputi[size - i - 1] = inputi[size - i - 1] * filter[i];
    }
}


void Limiter(double cut, double gain,double* input, const int size)
{
    for (int i = 0; i < size;i++)
    {
        if (gain != 0)		
            input[i] *= gain;
        
        if (input[i] > cut)
            input[i] = cut;
        
        if (input[i] < -cut)
            input[i] = -cut;
    }
}