//
//  PitchDetector.h
//  ViolinMasterTest
//
//  Created by Wu Jian on 10/1/14.
//  Copyright (c) 2014 nnn. All rights reserved.
//

#ifndef ViolinMasterTest_PitchDetector_h
#define ViolinMasterTest_PitchDetector_h

#include "dywapitchtrack.h"
#include <string.h>

#define SAMPLES_COUNT 1024

typedef short INT16;

typedef struct NoteDesc
{
    double pitchValue;
    double deviation;
    const char * pitchName;
    const char * toneName;
} NoteDesc;

class SamplesNode
{
public:
    SamplesNode() {
        previous = NULL;
        next = NULL;
        //memset(samples, 0, sizeof(samples)*SAMPLES_COUNT);
    }
    ~SamplesNode() {}
    
    double          samples[SAMPLES_COUNT];
    int             sampleSize;
    NoteDesc        noteDesc;
    SamplesNode *   next;
    SamplesNode *   previous;
};


class PitchDetector
{
public:
    PitchDetector();
    ~PitchDetector();
    void init();
    void release();
    const double detectFrequency(SamplesNode * pSampleNote);
    const char * notationName(const char * pitchName);
    
private:
    int searchFrequencyInArray(double frequency, int startIdx, int endIdx);
    const char * getPitchName(double frequency, double * pDeviation);
    
    
    
private:
    bool initialized;
    dywapitchtracker _pitchTracker;
    
    const int   SAMPLE_COUNT    = 2048;
    
    //**********************
    // pitch table
    //**********************
    const char pitchNames[108][4] = {
        "C0 ","C0#","D0 ","D0#","E0 ","F0 ","F0#","G0 ","G0#","A0 ","A0#","B0 ",
        "C1 ","C1#","D1 ","D1#","E1 ","F1 ","F1#","G1 ","G1#","A1 ","A1#","B1 ",
        "C2 ","C2#","D2 ","D2#","E2 ","F2 ","F2#","G2 ","G2#","A2 ","A2#","B2 ",
        "C3 ","C3#","D3 ","D3#","E3 ","F3 ","F3#","G3 ","G3#","A3 ","A3#","B3 ",
        "C4 ","C4#","D4 ","D4#","E4 ","F4 ","F4#","G4 ","G4#","A4 ","A4#","B4 ",
        "C5 ","C5#","D5 ","D5#","E5 ","F5 ","F5#","G5 ","G5#","A5 ","A5#","B5 ",
        "C6 ","C6#","D6 ","D6#","E6 ","F6 ","F6#","G6 ","G6#","A6 ","A6#","B6 ",
        "C7 ","C7#","D7 ","D7#","E7 ","F7 ","F7#","G7 ","G7#","A7 ","A7#","B7 ",
        "C8 ","C8#","D8 ","D8#","E8 ","F8 ","F8#","G8 ","G8#","A8 ","A8#","B8 "
    };
    
    const double pitchFrequencies[108] = {
        16.352,17.324,18.354,20,21,22,23,25,26,28,29,31,
        33,35,37,39,41,44,46,49,52,55,58,62,
        65,69,73,78,82,87,93,98,104,110,117,124,
        131,139,147,156,165,175,185,196,208,220,233,247,
        262,278,294,311,330,349,370,392,415,440,466,494,
        523,554,587,622,659,699,740,784,831,880,932,988,
        1047,1109,1175,1245,1319,1397,1475,1568,1661,1760,1865,1976,
        2093,2218,2349,2489,2637,2794,2960,3136,3322,3520,3729,3951,
        4186,4435,4699,4978,5274,5588,5920,6272,6645,7040,7459,7902,
    };
    
    const char notationNames[7][4] = { "La", "Si", "DO", "RE", "Mi", "Fa", "Sol" };
};

#endif
