#include "fftw3.h"

enum EffectType
{
	EQEffect,
	HIGHPASSEffect,
	LOWPASSEffect,
	LIMITEREffect
};

class effect
{
public:
	effect();
	virtual void Process(fftw_complex* buffer,const int size);
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