//
//  PitchDetector.cpp
//  ViolinMasterTest
//
//  Created by Wu Jian on 10/1/14.
//  Copyright (c) 2014 nnn. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "PitchDetector.h"


PitchDetector::PitchDetector()
{
    ;
}

PitchDetector::~PitchDetector()
{
    if (initialized) {
        release();
    };
}

void PitchDetector::init()
{
    if (!initialized) {
        dywapitch_inittracking(&_pitchTracker);
    }
    
    return;
}

void PitchDetector::release()
{
    ;
}

//const double PitchDetector::detectFrequency(INT16 * samples, NoteDesc * noteDesc, double * pDeviation)
const double PitchDetector::detectFrequency(SamplesNode * pSamplesNode)
{
    double      freqDetected   = -1.0;
    
    if (NULL == pSamplesNode) {
        return -1.0;
    }

    /* todo, sample_count vs. 4096 */
    freqDetected = dywapitch_computepitch(&_pitchTracker,
                                           pSamplesNode->samples,
                                           0,
                                           pSamplesNode->sampleSize);
    freqDetected = freqDetected * 2;
    if (freqDetected >= 16.352 && freqDetected <= 7902) {
        pSamplesNode->noteDesc.pitchName = getPitchName(freqDetected,
                                           &pSamplesNode->noteDesc.deviation);
        pSamplesNode->noteDesc.pitchValue = freqDetected;
        pSamplesNode->noteDesc.toneName = notationNames[pSamplesNode->noteDesc.pitchName[0]-'A'];
    }
    
    return freqDetected;
}


// search algorithm
int PitchDetector::searchFrequencyInArray(double frequency, int startIdx, int endIdx)
{
    if (abs(endIdx-startIdx)<=1)
    {
        return endIdx;
    }
    else if (frequency < pitchFrequencies[startIdx + ((endIdx-startIdx)>>1)])
    {
        return searchFrequencyInArray(frequency, startIdx, startIdx + ((endIdx-startIdx)>>1));
    }
    else
    {
        return searchFrequencyInArray(frequency, startIdx + ((endIdx-startIdx)>>1), endIdx);
    }
}

// return pitch name according to frequency
const char * PitchDetector::getPitchName(double frequency, double * pDeviation)
{
    int idx = -1;
    const int countOfFrequencies = sizeof(pitchFrequencies)/sizeof(double);
    int a = 0, b = 0, c = 0;
    unsigned int absA = 0, absB = 0, absC = 0;
    
    if (frequency>PitchDetector::pitchFrequencies[countOfFrequencies-1]
        || frequency<PitchDetector::pitchFrequencies[0])
    {
        return NULL;
    }
    
    idx = searchFrequencyInArray(frequency, 0, countOfFrequencies-1);
    a = frequency - PitchDetector::pitchFrequencies[idx];
    absA = fabs(a);
    b = b = idx>=countOfFrequencies-1 ? 0xffff:(frequency - pitchFrequencies[idx+1]);
    absB = idx>=countOfFrequencies-1 ? 0xffff:fabs(frequency - pitchFrequencies[idx+1]);
    c = idx<=0 ? 0xffff:(frequency - pitchFrequencies[idx-1]);
    absC = idx<=0 ? 0xffff:fabs(frequency - pitchFrequencies[idx-1]);
    
    if (absA>absB)
    {
        if (b>c) {
            idx = idx - 1;
            *pDeviation = c;
        }
        else {
            idx = idx + 1;
            *pDeviation = b;
        }
    }
    else
    {
        if (absA>absC) {
            idx = idx - 1;
            *pDeviation = c;
        }
        else
        {
            idx = idx;
            *pDeviation = a;
        }
    }
    
    return idx==-1? "[nil]":pitchNames[idx];
}

const char * PitchDetector::notationName(const char * pitchName)
{
    if (pitchName != NULL
        && pitchName[0] >= 'A'
        && pitchName[0] <= 'G')
    {
        return notationNames[pitchName[0] - 'A'];
    }
    else
    {
        return "";
    }
}