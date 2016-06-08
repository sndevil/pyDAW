#include "Stdafx.h"
#include <sndfile.hh>
#include <cstdio>
#include <iostream>
#include <math.h>
#include "fftw3.h"
#include "track.h"
#include "functions.h"

using std::cout;
#define BufferSIZE 4096

track::track(char* filename)
{
    //cout<<"Entered Constructor\n";
	eof = mute = solo = false;
	Totalframes=Currentframe=FrameOffset = PositionInLine=0;
	pan = 0;
	volume = 1;
    file = SndfileHandle(filename);
    //cout<<"Opened File\n";
    this->channels = file.channels();
    this->FrameOffset = 0;
    this->Currentframe = 0;
    this->Totalframes = file.seek(0,SF_SEEK_END);
    this->PositionInLine = 0;
    file.seek(0,SF_SEEK_SET);
    this->buffer = new double[BufferSIZE];
    //cout<<"Reading buffer\n";
    Readbuffer();
    //cout<<"Buffer read\n";
}
track::track()
{
    this->channels = 0;
    this->FrameOffset = 0;
    this->Currentframe = 0;
    this->Totalframes = 0;
    this->PositionInLine = 0;
    //this->buffer = new double[BufferSIZE];
}

void track::init(char* filename)
{
	//cout<<filename<<"\n";
    file = SndfileHandle(filename);
    //cout<<"Opened File\n";
    this->channels = file.channels();
    this->FrameOffset = 0;
    this->Currentframe = 0;
    this->Totalframes = file.seek(0,SF_SEEK_END);
    this->PositionInLine = 0;
	this->pan = 0;
	this->volume = 1;
    file.seek(0,SF_SEEK_SET);
    this->buffer = new double[BufferSIZE];
	timeeffects = new effect*[10];
	freqeffects = new effect*[10];
	this->freqeffectscount = 0;
	timeeffectscount = 0;
	this->SampleRate = file.samplerate();
    //cout<<"Reading buffer\n";
    Readbuffer();
}

void track::SetPan(double a)
{
    pan = a;
    Process();
}

void track::SetVolume(double a)
{
    volume = a;
    Process();
}

void track::Process()
{
    //double Gr,Gl;
    //cout<<"Buffer after process: ";

    double buffer2[BufferSIZE];
    double samplerate = file.samplerate();
    double *in1, *in2,*outr1,*outr2;
    in1 = (double*)fftw_malloc(sizeof(double)*BufferSIZE/2);
    outr1 = (double*)fftw_malloc(sizeof(double)*BufferSIZE/2);
        outr2 = (double*)fftw_malloc(sizeof(double)*BufferSIZE/2);
        in2 = (double*)fftw_malloc(sizeof(double)*BufferSIZE/2);
    
    double deltaf = samplerate / (BufferSIZE/channels);
    for (int i = 0; i < BufferSIZE;i++)
    {
        //double t = (double)i / samplerate;
        buffer[i] *= volume;
            if (i%2==0) //rightchannel
            {
                if (pan > 0)
                    buffer[i]*=1-pan;
                in1[i/2]= buffer[i];
            }
            else //leftchannel
            {
                if (pan<0)
                    buffer[i]*=pan+1;
                in2[i/2] = buffer[i];
            }
    }

    fftw_complex *out1;
    fftw_complex *out2;
    fftw_plan p1,p2,pi1,pi2;
    
    out1 = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * (BufferSIZE/channels/2+1));
    out2 = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * (BufferSIZE/channels/2+1));
    p2 = fftw_plan_dft_r2c_1d(BufferSIZE/channels, in2, out2,FFTW_ESTIMATE);
    pi2 = fftw_plan_dft_c2r_1d(BufferSIZE/channels, out2, outr2,FFTW_ESTIMATE);
    fftw_execute(p2);
    
    p1 = fftw_plan_dft_r2c_1d(BufferSIZE/channels, in1, out1,FFTW_ESTIMATE);
    pi1 = fftw_plan_dft_c2r_1d(BufferSIZE/channels, out1, outr1,FFTW_ESTIMATE);
    fftw_execute(p1);
    
	//cout<<"Going to call effects. Count: "<< freqeffectscount << "\n";

	for (int i = 0; i < freqeffectscount;i++)
	{
		freqeffects[i]->Process(out1,BufferSIZE/channels/2+1);
		freqeffects[i]->Process(out2,BufferSIZE/channels/2+1);
	}

	//cout<<"Effect call finished\n";
    //double eqbands[6] = {200,100,-1,2000,1000,1};
    //equaliser(eqbands,2,samplerate,out1,BufferSIZE/4+1);
    //equaliser(eqbands,2,samplerate,out2,BufferSIZE/4+1);
    //Highpass(1000, 1,500,samplerate,out1,BufferSIZE/channels/2+1);
    //Highpass(1000, 1,500,samplerate,out2,BufferSIZE/channels/2+1);
    //for (int i = 0 ; i < BufferSIZE/4+1;i++)
    //{
    //    double mag =sqrt(pow(out1[i][0],2)+pow(out1[i][1],2));
        //if (mag > 0)
    //    cout<<i*deltaf<<" : " <<mag<<"\n";
    //}


    fftw_execute(pi1);
    fftw_execute(pi2);

	//cout<<"iFFT\n";

    for (int i = 0;i < BufferSIZE/channels;i+=channels)
    {
        buffer[i*channels] = outr1[i]/(double)BufferSIZE * channels;
        buffer[i*channels+1] = outr2[i]/(double)BufferSIZE*channels;
    }
	//cout<<"1\n";
    fftw_destroy_plan(p1); fftw_destroy_plan(pi1);
	//cout<<"2\n";
    fftw_free(in1); fftw_free(out1);
	//cout<<"3\n";
    fftw_destroy_plan(p2);
	//cout<<"4\n";
    fftw_free(out2);
    fftw_free(in2);
    fftw_destroy_plan(pi2);
    

	for (int i = 0; i < timeeffectscount;i++)
		timeeffects[i]->Process(buffer,BufferSIZE);


    //Limiter(1, 0.7,buffer, BufferSIZE);
    //fft_stereo(buffer, fftr,ffti, BufferSIZE);
    //for (int i = 0; i < BufferSIZE;i++)
    //{
    //    cout<<i<<" : " <<sqrt(pow(fftr[i],2)+pow(ffti[i],2))<<"\n";
    //}
    //

    
    /*for (int i = 0; i < BufferSIZE;i++)
    {
        //cout<<i<<" : "<<buffer[i]<<"\n";
        if (buffer[i]>1||buffer[i]<-1)
            cout<<i<<":"<<buffer[i]<<"\n";
    }*/
    
}

void track::Process_Mono()
{
    //double Gr,Gl;
    //cout<<"Buffer after process: ";
    
    double samplerate = file.samplerate();
    double *in1, *outr1;
    in1 = (double*)fftw_malloc(sizeof(double)*BufferSIZE);
    outr1 = (double*)fftw_malloc(sizeof(double)*BufferSIZE);
    
    double deltaf = samplerate / (BufferSIZE/channels);
    for (int i = 0; i < BufferSIZE;i++)
    {
        //double t = (double)i / samplerate;
        buffer[i] *= volume;

            in1[i] = buffer[i];
    }
    fftw_complex *out1;
    fftw_plan p1,pi1;
    
    out1 = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * (BufferSIZE/channels/2+1));
    
    p1 = fftw_plan_dft_r2c_1d(BufferSIZE/channels, in1, out1,FFTW_ESTIMATE);
    pi1 = fftw_plan_dft_c2r_1d(BufferSIZE/channels, out1, outr1,FFTW_ESTIMATE);
    fftw_execute(p1);
    
    //double eqbands[6] = {200,100,-1,2000,1000,1};
    //equaliser(eqbands,2,samplerate,out1,BufferSIZE/4+1);
    //equaliser(eqbands,2,samplerate,out2,BufferSIZE/4+1);
    //Highpass(1000, 1,500,samplerate,out1,BufferSIZE/channels/2+1);
    
    
    fftw_execute(pi1);

    
    for (int i = 0;i < BufferSIZE;i++)
    {
        buffer[i] = outr1[i]/(double)BufferSIZE;
    }
    
    fftw_destroy_plan(p1); fftw_destroy_plan(pi1);
    fftw_free(in1); fftw_free(out1);
    
    Limiter(1, 0.9,buffer, BufferSIZE);
    //fft_stereo(buffer, fftr,ffti, BufferSIZE);
    //for (int i = 0; i < BufferSIZE;i++)
    //{
    //    cout<<i<<" : " <<sqrt(pow(fftr[i],2)+pow(ffti[i],2))<<"\n";
    //}
    //
    
    
    /*for (int i = 0; i < BufferSIZE;i++)
     {
     //cout<<i<<" : "<<buffer[i]<<"\n";
     if (buffer[i]>1||buffer[i]<-1)
     cout<<i<<":"<<buffer[i]<<"\n";
     }*/
    
}

double* track::Getsample(sf_count_t i)
{
    double* out;
    if (i > Totalframes*channels)
        eof = true;
    else
    {
        //cout<<"entered for i:" << i <<"\n";
        int offset = (int)i/channels;// / BufferSIZE);
        
        int index = i % BufferSIZE;
        //cout<<"Frameoffset: " << FrameOffset << " offset: " <<offset <<"\n";
        if ( FrameOffset - offset == 0 )
        {
            //cout<<"Buffer Read for i:" << i <<"\n";
            //FrameOffset = offset;
            //Currentframe = file.seek((int)i/2,SF_SEEK_SET);
            Readbuffer();
        }
        if (eof)
            eof = false;
        out = new double[channels];
        for (int j = 0; j < channels;j++)
            out[j] = buffer[index + j];
    }
    return out;
}

double* track::Getbuffer()
{
    return buffer;
}

sf_count_t track::GetTotalFrames()
{
    return Totalframes;
}

void track::Readbuffer()
{
    //cout<<"Entered Reading buffer\n";
    sf_count_t read = file.readf(buffer,BufferSIZE/channels);
    //cout<<"Processing\n";
    if (channels > 1)
        Process();
    else
        Process_Mono();
    //cout<<"Process done\n";
    //cout<<"increasing frameoffset\n";
    //cout<<"read:"<< read<<" Frameoffset: " <<FrameOffset<<" Processing\n";
    FrameOffset += read;
    //cout<<"Frameoffset: " <<FrameOffset<<"\n";
}

void track::AddFreqEffect(effect* e, EffectType t)
{

	switch (t)
	{
	case EQEffect:
		freqeffects[freqeffectscount] = new EQ((EQ*)e,BufferSIZE/channels/2 + 1);//->init(temp);
		freqeffectscount++;

		break;
	case HIGHPASSEffect:
		freqeffects[freqeffectscount]= new Highpass((Highpass*)e,BufferSIZE/channels/2 +1);//->init(temp);
		freqeffectscount++;
		break;
	}
	if (channels > 1)
        Process();
    else
        Process_Mono();
}

void track::AddTimeEffect(effect* e, EffectType t)
{
	switch(t)
	{
	case DELAYEffect:
		timeeffects[timeeffectscount] = new delay((delay*)e);
		timeeffectscount++;
		break;
	case REVERBEffect:
		timeeffects[timeeffectscount] = new reverb((reverb*)e);
		timeeffectscount++;
	}

	if (channels > 1)
		Process();
	else
		Process_Mono();
}