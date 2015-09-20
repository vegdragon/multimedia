//
//  Queue.cpp
//  SpeakHere
//
//  Created by Wu Jian on 10/6/14.
//
//

#include "SamplesQueue.h"
#include <stdlib.h>
#include <string.h>

#define CLEANUP_ON_ERROR(condition) if((condition)) goto cleanUp;

int SamplesQueue::init()
{
    return 0;
}

int SamplesQueue::release()
{
    SamplesNode * pNode = NULL;
    while (0==dequeue(&pNode) && pNode!=NULL)
    {
        delete pNode;
    }

    return 0;
}

int SamplesQueue::enqueue(short * samples, int size)
{
    int result = -1;
    SamplesNode * pNode = NULL;
    int i = 0;
    int j = 0;
    
    CLEANUP_ON_ERROR(NULL==samples);
    CLEANUP_ON_ERROR(size<=0);  // size should be 1024 usually
    
    // divide the node to 1024 package
    for (i=0; i<size; i++) {
        pNode = new SamplesNode();
        CLEANUP_ON_ERROR(NULL==pNode);
        
        // copy samples to sample node.
        for (j=0;j<SAMPLES_COUNT && j<i;j++,i++) {
            pNode->samples[j] = (float)samples[i];
        }
        
        pNode->noteDesc.deviation = -1;
        pNode->noteDesc.pitchName = "";
        pNode->noteDesc.pitchValue = -1;
        pNode->noteDesc.toneName = "";
        pNode->sampleSize = j;
        
        
        if (NULL==_head) {
            _head = pNode;
            _head->next = NULL;
            _head->previous = NULL;
            _tail = _head;
            result = 0;
        }
        else {
            _tail->next = pNode;
            pNode->previous = _tail;
            _tail = pNode;
            _tail->next = NULL;
            result = 0;
        }
    }
cleanUp:
    return result;
}

int SamplesQueue::dequeue(SamplesNode ** ppNode)
{
    int result = -1;
    if (_tail != NULL) {
        *ppNode = _tail;
        
        // remove node from linked list
        if (_tail==_head) {
            _head = _tail = NULL;
        }
        else
        {
            _tail = _tail->previous;
            _tail->next = NULL;
        }
        
        
        result = 0;
    }
    
    return result;
}