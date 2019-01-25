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

void effect::Process(double* buffer, const int size)
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

delay::delay()
{
}

delay::delay(delay* tocopy)
{
	filtersize = tocopy->filtersize;
	filter = new double[filtersize];
	soundmemo = new double[filtersize * 4];

	readindex = 0;
	writeindex = 0;
	readindex1 = 0;
	readindex2 = 0;
	firsttime = true;


	firstgain = tocopy->firstgain;
	secondgain = tocopy->firstgain;
	thirdgain = tocopy->thirdgain;

	for (int i = 0; i < filtersize; i++)
		filter[i] = tocopy->filter[i];
}

delay::delay(double freq, int Samplerate, int channels,double feedback,double gain)
{
	filtersize = (int)((1/freq)*Samplerate)*channels;
	filter = new double[filtersize];
	soundmemo = new double[filtersize * 4];
	readindex = 0;
	writeindex = 0;
	readindex1 = 0;
	readindex2 = 0;
	firsttime = true;

	firstgain = gain;
	secondgain = firstgain * feedback;
	thirdgain = secondgain * feedback;

	for (int i = 0 ; i < filtersize; i++)
		filter[i] = 1;

}

delay::delay(int cyclems, int Samplerate, int channels,double feedback,double gain)
{
	filtersize = (int)((double)cyclems/1000*Samplerate)*channels;
	filter = new double[filtersize];
	soundmemo = new double[filtersize * 4];
	readindex =0 ;
	writeindex = 0;
	readindex1 = 0;
	readindex2 = 0;
	firsttime = true;
	
	firstgain = gain;
	secondgain = firstgain * feedback;
	thirdgain = secondgain * feedback;

	for (int i = 0; i < filtersize;i++)
		filter[i] = 1;
}

void delay::Process(double* buffer, const int size)
{
	double f1=0,f2=0,f3=0;
	for (int i = 0; i < size; i++)
	{
		soundmemo[writeindex++] = buffer[i];
		if (writeindex >= 4*filtersize)
		{
			writeindex = 0;
			firsttime = false;
		}

		if (firsttime)
		{
			f1 = (writeindex > filtersize)? soundmemo[readindex] : 0;
			f2 = (writeindex > 2*filtersize) ? soundmemo[readindex - filtersize] : 0;
			f3 = (writeindex > 3*filtersize) ? soundmemo[readindex - 2*filtersize] : 0;

			if (writeindex >= filtersize)
				readindex++;

			if (readindex >= 4*filtersize)
			{
				readindex = 0;
				firsttime = false;
			}
		}
		else
		{
			f1 = soundmemo[readindex];
			f2 = (readindex > filtersize) ? soundmemo[readindex - filtersize] : soundmemo[readindex + 3*filtersize];
			f3 = (readindex > 2 * filtersize) ? soundmemo[readindex - 2 * filtersize] : soundmemo[readindex + 2*filtersize];

			if (++readindex >= 4*filtersize)
				readindex = 0;
		}

		buffer[i] += (f1 * firstgain + f2* secondgain + f3 *thirdgain) * filter[readindex % size];
	}
}

reverb::reverb()
{
}

reverb::reverb(reverb* tocopy)
{
	filtersize = tocopy->filtersize;
	readindex = tocopy->readindex;
	writeindex = tocopy->writeindex;
	gain = tocopy->gain;
	channels = tocopy->channels;

	memosize = filtersize / channels;
	filter = new double[filtersize];
	soundmemo = new double[filtersize];

	for (int i = 0 ; i < filtersize;i++)
		filter[i] = tocopy->filter[i];
	for (int i = 0; i< memosize; i++)
		soundmemo[i] = tocopy->soundmemo[i];
}

reverb::reverb(int cyclems, int Samplerate, int channels, double gain)
{
	readindex = 0;
	writeindex = 0;
	this->channels = channels;
	this->gain = gain;
	filtersize = (int)(((double)cyclems/1000) * Samplerate) * channels / 10;
	memosize = filtersize / channels;
	filter = new double[filtersize];
	soundmemo = new double[memosize];
	for (int i = 0 ; i < filtersize; i++)
		filter[i] = 1;//- ((double)i/filtersize);
	for (int i = 0 ; i < memosize ; i++)
		soundmemo[i] = 0;
}

void reverb::Process(double* buffer, const int size)
{
	double toadd = 0;
	int idx = 0,count=0;
	int tmpread = 0;
	double max =0,min = 0;
	//std::cout<<"Process Entered\n";
	for (int i = 0; i < size; i++)
	{
		//std::cout<<"Main loop Entered\n";
		soundmemo[writeindex] += buffer[i]/10;

		if (i % 10 == 9)
		{
			
			toadd = 0;
			count = 0;
			for (int j = 0; j < filtersize; j++)
			{
				idx = writeindex - (j/2);
				idx = (idx >=0) ? idx : memosize+idx;
				toadd += soundmemo[idx] * filter[j];
				if (soundmemo[idx] > 50 || soundmemo[idx] < -50)
				{
					std::cout<<"Toadd: " << toadd<<"\n";
					std::cout<<"Filter["<<j<<"] : " << filter[j]<<"\n";
					std::cout<<"Memo["<<idx<<"] : " << soundmemo[idx]<<"\n";
					std::cout<<"Memosize: "<<memosize << "  filtersize: " << filtersize<<"\n";
				}
				count++;
			}

			if (count != 0)
				toadd *= gain / count;
			else
				toadd *= gain;

			if (toadd > max)
				max = toadd;
			if (min>toadd)
				min = toadd;

			for (int j = 0; j < 10;j++)
				buffer[i-j] += toadd;
			
			if (++writeindex >= memosize)
				writeindex = 0;

		}
	}
	//std::cout<<"Countmax: " << countmax<<"\n";
	//std::cout<<"Countmin: " << countmin<<"\n";
	std::cout<<"max: " << max <<"\n";
	std::cout<<"min: " << min << "\n";
}