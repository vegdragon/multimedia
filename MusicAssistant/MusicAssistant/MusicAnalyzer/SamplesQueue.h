//
//  Queue.h
//  SpeakHere
//
//  Created by Wu Jian on 10/6/14.
//
//

#ifndef __SpeakHere__Queue__
#define __SpeakHere__Queue__

#include <stdio.h>
#include "PitchDetector.h"



class SamplesQueue
{
public:
    SamplesQueue() : _head(NULL), _tail(NULL)
    {
        ;
    }
    ~SamplesQueue()
    {
        ;
    }
    int init();
    int release();
    
    int enqueue(short * samples, int size);     // T - in param
    int dequeue(SamplesNode ** ppNode);         // T - out param
    
private:
    SamplesNode * _head;
    SamplesNode * _tail;
};
#endif /* defined(__SpeakHere__Queue__) */
