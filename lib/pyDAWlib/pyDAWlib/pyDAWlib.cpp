// This is the main DLL file.

#include "stdafx.h"

#include "pyDAWlib.h"
#include "mixer.h"
#include "outfile.h"
#include <iostream>

int test(int a)
{
	return a;
}

Mixer NewMixer()
{
	return reinterpret_cast<void*>(new mixer());
}

void AddTrack(Mixer m,const wchar_t* path, long position)
{
	int i = 0;
	char w[200];
	mixer* mix = reinterpret_cast<mixer*>(m);
	for (i;i < 200;i++)
	{
		w[i] = (char)path[i];
		if (w[i] == '\0')
			break;
	}
	sf_count_t pos = position;
	mix->AddTrack(w,pos);
}

void GetSample(Mixer m, long position, double * out)
{
	sf_count_t counter = position - (position %2);
	mixer* mix = reinterpret_cast<mixer*>(m);
	double* outt = mix->Getsample(counter);
	out[0] = outt[0];
	out[1] = outt[1];
}

long GetLength(Mixer m)
{
	mixer* mix = reinterpret_cast<mixer*>(m);
	return (long)(mix->GetTotalFrames());
}


Outfile CreateOutfile(const wchar_t* path,int bps, int channels, int Samplerate)
{
	char w[200];
	int format=0,i=0;
	for (i;i < 200;i++)
	{
		w[i] = (char)path[i];
		if (w[i] == '\0')
			break;
	}

	switch(bps)
	{
	case 8:
		format = SF_FORMAT_WAV | SF_FORMAT_PCM_S8;
		break;
	case 16:
		format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
		break;
	case 24:
		format = SF_FORMAT_WAV | SF_FORMAT_PCM_24;
		break;
	case 32:
		format = SF_FORMAT_WAV | SF_FORMAT_PCM_32;
		break;
	}
	//std::cout<<"Format: " <<format<<" Channels: " << channels<< " SR: " << Samplerate<<"\n";

	return reinterpret_cast<void*>(new outfile(w,format,channels,Samplerate));
}

void WriteOutfile(Outfile f,Mixer m, long position)
{
	mixer* mix = reinterpret_cast<mixer*>(m);
	outfile* file = reinterpret_cast<outfile*>(f);
	double* read;
	//std::cout<<"calling getsample()\n";
	read = mix->Getsample((sf_count_t)position);
	//long length = mix->GetTotalFrames();
	//if (position > length)
	//std::cout<<"read "<<position<<" buffer: "<<read[0]<<" , " << read[1]<<"\n";
	file->write(read,1);
	//std::cout<<"wrote " << position<<"\n";
	//file.writef(mix->Getsample((sf_count_t)position),1);
}


void CloseOutfile(Outfile f)
{
	outfile* file = reinterpret_cast<outfile*>(f);
	file->close();
}

void AddEQ(Mixer m, int trackindex, double* bands, int bandcount)
{
	//std::cout<<"Add EQ called count: "<< bandcount <<"\n";
	mixer* mix = reinterpret_cast<mixer*>(m);
	EQ* eq = new EQ();
	//for (int i = 0 ; i < bandcount*3;i++)
	//	std::cout<<i<<" : "<<bands[i]<<"\n";
	eq->SaveBands(bands,bandcount,mix->tracks[trackindex].SampleRate,BufferSIZE/4+1);
	//std::cout<<"EQ saved\n";
	mix->tracks[trackindex].AddFreqEffect(eq, EQEffect);
	//std::cout<<"EQ added to track"<<trackindex<<"\n";
}

void AddHighpass(Mixer m, int trackindex, int freq, int gain, int bw)
{
	//std::cout<<"Add HP called\n";
	mixer* mix = reinterpret_cast<mixer*>(m);
	Highpass* hp = new Highpass();
	//std::cout<<"HP constructor called\n";
	hp->SaveBand(freq,bw,gain,mix->tracks[trackindex].SampleRate,BufferSIZE/4+1);
	//std::cout<<"HP band saved\n";
	mix->tracks[trackindex].AddFreqEffect(hp, HIGHPASSEffect);
	//std::cout<<"HP Added to track\n";
}

void AddDelay(Mixer m, int trackindex, int cyclems, double feedback, double gain)
{
	mixer* mix = reinterpret_cast<mixer*>(m);
	delay* del = new delay(cyclems, mix->tracks[trackindex].SampleRate
							,mix->tracks[trackindex].channels,feedback, gain);
	mix->tracks[trackindex].AddTimeEffect(del, DELAYEffect);
}
