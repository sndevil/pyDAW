#include "fftw3.h"

enum EffectType
{
	EQEffect,
	HIGHPASSEffect,
	LOWPASSEffect,
	LIMITEREffect,
	DELAYEffect
};

class effect
{
public:
	effect();
	virtual void Process(fftw_complex* buffer,const int size);
	virtual void Process(double* buffer, const int size);
	virtual void init(effect* eff);
	//virtual void init(Highpass* eff);
};


class EQ : public effect
{
public:
	EQ();
	EQ(EQ* temp,const int size);
	void Process(fftw_complex* buffer,const int size);
	void SaveBands(double* bands,int bandcount,int SampleRate,const int size);
	void init(EQ* eff);
	double* tempbandsEQ;
	int ftempEQ,gtempEQ,bwtempEQ;
	int bandcountEQ;
	bool EQSaved;
	double* tempfilterEQ;
};

class Highpass : public effect
{
public:
	Highpass();
	Highpass(Highpass* temp, const int size);
	void Process(fftw_complex* buffer,const int size);
	void SaveBand(int freq,int bw ,int gain,int SampleRate,const int size);
	void init(Highpass* eff);
	int ftemp,gtemp,bwtemp;
    bool HPSaved;
    double* tempfilter;
};



class delay : public effect
{
public:
	delay();
	delay(delay* tocopy);
	delay(double freq,int Samplerate, int channels, double feedback, double gain);
	delay(int cyclems,int Samplerate, int channels, double feedback, double gain);
	void init();
	virtual void Process(double* buffer, const int size);

	double* soundmemo;
	double* filter;

	int writeindex;
	int readindex;
	int readindex1;
	int readindex2;

	int filtersize;
	int timecounter;
	double firstgain;
	double secondgain;
	double thirdgain;

	bool firsttime;

private:

};