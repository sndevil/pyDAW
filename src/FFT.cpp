// FFT.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
//#include <time.h>
//#include <sys\utime.h>

#define PI 3.14159265
#define Size 4096

#define SampleRate 48000

using std::cout;

void fft(short* input, short* outputr,short*outputi , int size);
void fft_stereo(double* input, double* outputr,double*outputi , int size);
void fft2(double* input,double*inputi , const int size);
void equaliser(double* bands,int bandcount,double* inputr, double* inputi,const int size);
void Reverse_fft(short* inputr, short* inputi, short* out, int size);
void Reverse_fft_stereo(double* inputr, double* inputi, double* out, int size);
void Highpass(int freq, int gain,int bw,double* inputr, double* inputi,const int size);
void Lowpass(int freq, int gain,int bw,double* inputr, double* inputi,const int size);
void Limiter(double cut, double gain,double* input, const int size);


int _tmain(int argc, _TCHAR* argv[])
{

	double input[Size], inputreversed[Size];
	double inputi[Size],outputr[Size],outputi[Size];
	char strout[40];
	double error=0;
	//FILE* f;
	FILE* f = fopen("D://out.txt","w");
	FILE* f2 = fopen("D://out2.txt","w");
	//double list[8]={35,150,690,750,900,5000,8100,10000};

	double t = 0,arg=0;
	for (int i = 0; i < Size;i+=2)
	{
		int counter = 0;
		t = (double)i/SampleRate;
		input[i] = input[i+1] = 0;
		for (double f = 1000;f < 4050;f+=500)
		{
			arg=sin(2*PI*t*f);//* 32767;
			input[i] += arg;// + sin((double)i/256*PI) + sin((double)i/2*PI);
			arg=sin(2*PI*t*f*5+10);//* 32767;
			input[i+1] += arg;
			counter++;

		}
		input[i]/=counter;
		input[i+1]/=counter;
		inputi[i] = 0;
		outputi[i] = 0;
	}

	fft_stereo(input,outputr,outputi,Size);
	double eqbands[6] = {20000,200,-0.5};
	equaliser(eqbands,1, outputr, outputi,Size);
	//bands { CenterFreq , BW , gain }
	Highpass(2000,1,2000,outputr,outputi,4096);
	//fft2(outputr,outputi,2048);

	Reverse_fft_stereo(outputr,outputi,inputreversed,Size);

	Limiter(0.99, 1,inputreversed, Size);

	for (int i = 0; i < Size/2;i++)
	{
		sprintf(strout,"%d;",i);
		fwrite(strout,sizeof(char),strlen(strout),f);
		fwrite(strout,sizeof(char),strlen(strout),f2);
	}
	for (int i = 0; i < Size; i++)
	{
		sprintf(strout,"%f;",sqrt(pow(outputr[i],2)+pow(outputi[i],2)));
		fwrite(strout,sizeof(char),strlen(strout),((i%2)==0)?f:f2);
		error += abs(inputreversed[i] - input[i]);
	}
	error /= Size;
	cout<<error<<"\n";
	fclose(f);
	fclose(f2);
	getchar();
	return 0;
}



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
        outputr[i] = r;//outputr[size-i-1] = r;
        outputi[i] = im;
        //outputi[size-i-1] = -im;
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
            r += input[j] * cos(temp);///32767
            im -= input[j] * sin(temp);
			cj++;
        }
        outputr[i] = r;//outputr[size-i-1] = r;
        outputi[i] = im;
		ci++;
        //outputi[size-i-1] = -im;
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
        outputr[i] = r;//outputr[size-i-1] = r;
        outputi[i] = im;
		ci++;
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
        out[i] = r / size * 2;// * 32767;
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
            r += inputr[j]*cos(temp) - inputi[j]*sin(temp);//*32767
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
			cj++;
        }
        out[i] = r / size * 2;
		ci++;
    }
}


void equaliser(double* bands,int bandcount,double* inputr, double* inputi,const int size)
{
	//bands { CenterFreq , BW , gain }

	//  gain * exp -(x - centerfreq)^2 / BW^2

    //Cut between 1khz and 2khz
    double equaliser[Size];
	double S = size / 2;
	double deltaf = SampleRate / S;
	double CF,BW,Gain;
    for (int i = 0 ; i < S; i +=2)
    {
            equaliser[i] = 1;			
			for (int j = 0; j < bandcount;j++)
			{
				CF = bands[3*j];
				BW = bands[3*j+1];
				Gain = bands[3*j+2];
				equaliser[i] += Gain * exp(-(pow(i/2*deltaf - CF,2)/pow(BW,2)));
			}
    }
    
    for (int i = 0; i < size/2; i+=2)
    {
        inputr[i] = inputr[i] * equaliser[i];
		inputr[size-i] = inputr[size-i] * equaliser[i];
        inputr[i+1] = inputr[i+1]*equaliser[i];
		inputr[size-i-1] = inputr[size-i-1] * equaliser[i];
        inputi[i] = inputi[i] * equaliser[i];
		inputi[size - i] = inputi[size - i] * equaliser[i];
        inputi[i+1] = inputi[i+1] * equaliser[i];
		inputi[size - i - 1] = inputi[size - i - 1] * equaliser[i];
    }
    
}

void Highpass(int freq, int gain,int bw,double* inputr, double* inputi,const int size)
{
	const int S = 2048;
    double filter[S];
    double alpha = bw;// / 4.605;
    double deltaf = SampleRate / S;
    for (int i = 1 ; i <= S; i ++)
    {
        if (i*deltaf < freq - 2*bw)
            filter[i-1] = 0;
        else
            filter[i-1] = gain * (3.4142 - exp(-((i * deltaf)-freq) / alpha))/3.4142;
        
        if (filter[i-1] < 0)
            filter[i-1] = 0;
    }
    // x-1 / x = 0.707
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
	}
}