//
//  MusicAnalyzer.h
//  SpeakHere
//
//  Created by Wu Jian on 10/5/14.
//
//

#ifndef __SpeakHere__MusicAnalyzer__
#define __SpeakHere__MusicAnalyzer__

#include <stdio.h>
#include <pthread.h>
#include "SamplesQueue.h"
#include "PitchDetector.h"

class MusicAnalyzer
{
public:
    MusicAnalyzer() : _bContinue(true), _sampleQueue(NULL), _pitchDetector(NULL)
    {
        ;
    }
    ~MusicAnalyzer()
    {
        ;
    }
    int init();
    int release();
    
    int startAnalyzer();
    int stopAnalyzer();
    int enqueueSamples(short * samples, int size);
    
private:
    static void * AnalyzerThread(void * data);
    
private:
    volatile bool           _bContinue;
    pthread_t               _analyzerThreadId;
    pthread_mutex_t         _mutex;
    pthread_cond_t          _cond;
    SamplesQueue *          _sampleQueue;
    PitchDetector *         _pitchDetector;
};
#endif /* defined(__SpeakHere__MusicAnalyzer__) */
