//
//  MusicAnalyzer.cpp
//  SpeakHere
//
//  Created by Wu Jian on 10/5/14.
//
//

#include <stdlib.h>
#include <assert.h>
#include "MusicAnalyzer.h"


#define CLEANUP_ON_ERROR(condition) if((condition)) goto cleanUp;


int MusicAnalyzer::init()
{
    _sampleQueue = new SamplesQueue();
    
    pthread_mutex_init(&_mutex, NULL);
    pthread_cond_init(&_cond, NULL);
    
    _pitchDetector = new PitchDetector();
    _pitchDetector->init();
    
    return 0;
}

int MusicAnalyzer::release()
{
    int ret = -1;
    
    _bContinue = false;
    if (_sampleQueue!=NULL) {
        delete _sampleQueue;
    }
    if (_pitchDetector != NULL) {
        _pitchDetector->release();
        delete _pitchDetector;
    }
    
    ret = pthread_mutex_destroy(&_mutex);
    // assert(ret);
    ret = pthread_cond_destroy(&_cond);
    // assert(ret);

    return ret;
}

void * MusicAnalyzer::AnalyzerThread(void * data)
{
    MusicAnalyzer * pContext = (MusicAnalyzer*)data;
    SamplesNode * pSampleNode = NULL;
    int ret = -1;
    
    while (pContext->_bContinue) {
        pthread_mutex_lock(&(pContext->_mutex));
        // dequeue a sample set
        ret = pContext->_sampleQueue->dequeue(&pSampleNode);
        if(ret != 0 || pSampleNode == NULL) {
            // wait if no data in queue
            pthread_cond_wait(&pContext->_cond, &pContext->_mutex);
        }
        else {
            double pitchDetected = -1;
            // decode pitch info from samples
            pitchDetected = pContext->_pitchDetector->detectPitch(pSampleNode);
            if (pitchDetected>0)
            {
                printf("\n[%s(%s) - %02f]\t[%02f]",
                   pSampleNode->noteDesc.pitchName,
                   pSampleNode->noteDesc.toneName,
                   pSampleNode->noteDesc.pitchValue,
                   pSampleNode->noteDesc.deviation);
            }
            // delete sample node.
            delete pSampleNode;
        }
        pthread_mutex_unlock(&pContext->_mutex);
    }
    
    return NULL;
}

int MusicAnalyzer::enqueueSamples(short * samples, int size)
{
    int result = -1;
    
    // enqueue samples
    _sampleQueue->enqueue(samples, size);
    
    // condition changed, notify another thread
    pthread_cond_signal(&_cond);
    
    return result;
}

int MusicAnalyzer::startAnalyzer()
{
    int result = -1;
    
    // Create the thread using POSIX routines.
    pthread_attr_t  attr;
    int             returnVal;
    
    returnVal = pthread_attr_init(&attr);
    assert(!returnVal);
    returnVal = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    assert(!returnVal);
    
    int threadError = pthread_create(&_analyzerThreadId,
                                     &attr,
                                     &MusicAnalyzer::AnalyzerThread,
                                     this);
    
    returnVal = pthread_attr_destroy(&attr);
    assert(!returnVal);
    if (threadError != 0)
    {
        // Report an error.
    }

    return result;
}

int MusicAnalyzer::stopAnalyzer()
{
    int result = 0;
    
    _bContinue = false;
    
    // condition changed, notify another thread
    pthread_cond_signal(&_cond);
    pthread_join(_analyzerThreadId, NULL);
    
    return result;
}



