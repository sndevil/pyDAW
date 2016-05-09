//
//  functions.cpp
//  
//
//  Created by Mohammadreza Rezaei on 5/9/16.
//
//
#include <math.h>
#include "functions.h"


//template<typename T>
void fft(short* data, int size) {
    
    if (size > 1)
    {
        fft(data,size/2);
        fft(data+size,size/2);
    }
        short wtemp,tempr,tempi,wr,wi,wpr,wpi;
        wtemp = sin(M_PI/size);
        wpr = -2.0*wtemp*wtemp;
        wpi = -sin(2*M_PI/size);
        wr = 1.0;
        wi = 0.0;
        for (unsigned i=0; i<size; i+=2) {
            tempr = data[i+size]*wr - data[i+size+1]*wi;
            tempi = data[i+size]*wi + data[i+size+1]*wr;
            data[i+size] = data[i]-tempr;
            data[i+size+1] = data[i+1]-tempi;
            data[i] += tempr;
            data[i+1] += tempi;
            
            wtemp = wr;
            wr += wr*wpr - wi*wpi;
            wi += wi*wpr + wtemp*wpi;
        }
}