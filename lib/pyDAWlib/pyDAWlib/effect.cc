#include "Stdafx.h"
#include "effect.h"
#include <math.h>
#include <iostream>

effect::effect()
{
}

void effect::init(effect* eff)
{
}

void effect::Process(fftw_complex* buffer,const int size)
{
}

void EQ::init(EQ* eff)
{
	EQSaved = eff->EQSaved;
//	tempfilterEQ = new double[size];
//	for (int i = 0; i <= size;i++)
//		tempfilterEQ[i] = eff->tempfilterEQ[i];
}
void Highpass::init(Highpass* eff)
{
//	this->Highpass(eff);
}


EQ::EQ()
{
	bandcountEQ = 0;
	EQSaved = false;
}

EQ::EQ(EQ* temp,const int size)
{
	EQSaved = temp->EQSaved;
	tempfilterEQ = new double[size];
	for (int i = 0; i <= size;i++)
		tempfilterEQ[i] = temp->tempfilterEQ[i];
}

void EQ::Process(fftw_complex* buffer,const int size)
{
	//std::cout<<"Entered EQ process\n";
	if (EQSaved)
	{
		for (int i = 0; i <= size; i++)
		{
			//std::cout<<i<<" : "<<tempfilterEQ[i]<<"\n";
			buffer[i][0] *= tempfilterEQ[i];
			buffer[i][1] *= tempfilterEQ[i];
		}
	}
	//std::cout<<"EQ Process Done\n";
}

void EQ::SaveBands(double* bands,int bandcount,int SampleRate,const int size)
{

	double deltaf = SampleRate / size / 2;

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
                    tempfilterEQ[i] += Gain * exp(-(pow(i*deltaf - CF,2)/pow((double)BW,2)));
                }
				if (tempfilterEQ[i]<0)
					tempfilterEQ[i] = 0;
				else if (tempfilterEQ[i] > 2)
					tempfilterEQ[i] = 2;
            }
            else
                tempfilterEQ[i] = tempfilterEQ[i-1];
        }
		tempfilterEQ[size] = 1;
        EQSaved = true;
    }
}


Highpass::Highpass()
{
	HPSaved = false;
}

Highpass::Highpass(Highpass* temp, const int size)
{
	HPSaved = temp->HPSaved;
	tempfilter = new double[size];
	ftemp = temp->ftemp;
	gtemp = temp->gtemp;
	bwtemp = temp->bwtemp;

	for (int i = 0; i <= size;i++)
		tempfilter[i] = temp->tempfilter[i];
}
void Highpass::Process(fftw_complex* buffer,const int size)
{
	for (int i = 0; i <= size; i++)
    {
        //std::cout<<i<<" : "<<tempfilter[i]<<"\n";
        buffer[i][0] *= tempfilter[i];
        buffer[i][1] *= tempfilter[i];
    }
}

void Highpass::SaveBand(int freq,int bw ,int gain,int SampleRate,const int size)
{
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
        tempfilter[size] = 1;
        HPSaved = true;
    }
}