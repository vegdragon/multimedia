#ifndef __PITCHNODE__
#define __PITCHNODE__

#include <string.h>
#include "PitchDictionary.h"


class PitchNode
{

private:
  pitch_freq_t   frequency;
  pitch_idx_t    pitchIndex;
  PitchNode *    next;
};

class PitchWindow
{
public:
  PitchWindow():pos(0) { memset (ring, -1, sizeof(pitch_idx_t)*WINDOW_SIZE); }
  void shift (pitch_idx_t idx) 
  {
    ring[pos++] = idx;
    pos %= WINDOW_SIZE;
  }

  /* get the pitch detected */
  pitch_idx_t detected()
  {
    int         i;
    pitch_idx_t result       = -1;
    int         maxCount     = 0;
    int         currentCount = 0;

    for (i=0;i<WINDOW_SIZE;i++)
    {
      PitchWindowMap[ring[i]] = 0;
    }

    for (i=0;i<WINDOW_SIZE;i++)
    {
      if (ring[i] != -1)
      {
        currentCount = ++PitchWindowMap[ring[i]];
      }
      if (currentCount > maxCount) 
      {
        maxCount = currentCount;
        result = ring[i];
      }
    }

    return result;
  }

private:
  static const unsigned int  WINDOW_SIZE = 5;        // size of the window
  pitch_idx_t                ring[WINDOW_SIZE];      // circular ring of pitch window
  unsigned int               pos;                    // position of next incoming pitch in the window

  int                        PitchWindowMap[PITCH_COUNT];    // counter map of the pitch window.
};




#endif /* #ifdef __PITCHNODE__ */

