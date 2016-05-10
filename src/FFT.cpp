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
#define Size 2048
using std::cout;

void fft(double* input, double* outputr,double*outputi , int size);
void fft2(double* input,double*inputi , int size);
void equaliser(double* inputr, double* inputi,int size);
void Reverse_fft(double* inputr, double* inputi, double* out, int size);

int _tmain(int argc, _TCHAR* argv[])
{

	double input[Size],inputi[Size],outputr[Size],outputi[Size], inputreversed[Size];
	char strout[40];
	double error=0;
	//FILE* f;
	FILE* f = fopen("D://out.txt","w");
	//double list[8]={35,150,690,750,900,5000,8100,10000};

	double t = 0,arg=0;
	for (int i = 0; i < Size;i++)
	{
		int counter = 0;
		t = (double)i/48000;
		input[i] = 0;
		for (double f = 200;f < 20050;f+=500)
		{
			arg=sin(2*PI*t*f);
			input[i] += arg;// + sin((double)i/256*PI) + sin((double)i/2*PI);
			counter++;
		}
		input[i]/=counter;
		outputr[i] = input[i];
		inputi[i] = 0;
		outputi[i] = 0;
	}

	fft(input,outputr,outputi,Size);
	equaliser(outputr, outputi,Size);
	//fft2(outputr,outputi,2048);

	Reverse_fft(outputr,outputi,inputreversed,Size);

	for (int i = 0; i < Size/2;i++)
	{
		sprintf(strout,"%d;",i);
		fwrite(strout,sizeof(char),strlen(strout),f);
	}
	for (int i = 0; i < Size/2; i++)
	{
		sprintf(strout,"%f;",sqrt(pow(outputr[i],2)+pow(outputi[i],2)));
		fwrite(strout,sizeof(char),strlen(strout),f);
		error += abs(inputreversed[i] - input[i]);
	}
	error /= Size;
	cout<<error<<"\n";
	fclose(f);
	getchar();
	return 0;
}



void fft(double* input, double* outputr,double*outputi , int size)
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

void Reverse_fft(double* inputr, double* inputi, double* out, int size)
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
		out[i] = r / size;
	}
}


void equaliser(double* inputr, double* inputi,int size)
{
	//Cut between 1khz and 2khz
	double equaliser[Size];
	for (int i = 0 ; i < Size; i ++)
	{
		if (i < 43 || (i > 84 && i < 510) || i > 553)
			equaliser[i] = 1;
		else
			equaliser[i] = 0.1;
	}

	for (int i = 0; i < Size; i++)
	{
		inputr[i] = inputr[i] * equaliser[i];
		inputi[i] = inputi[i] * equaliser[i];
	}

}