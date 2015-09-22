#ifndef __PITCHNODE__
#define __PITCHNODE__

#define PITCH_COUNT     108

#include <math.h>
#include <string.h>
#include <stdlib.h>

typedef int             pitch_idx_t;
typedef unsigned int    pitch_freq_t;


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
  PitchWindow():pos(0) { memset (ring, 0, sizeof(pitch_idx_t)*WINDOW_SIZE); }
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
      currentCount = ++PitchWindowMap[ring[i]];
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


class PitchDictionary
{
public:
  const char * indexToPitchName (pitch_idx_t idx)
  {
    return pitchNames[idx];
  }
  
  const char * indexToSylableName (pitch_idx_t idx)
  {
    return sylableName[pitchNames[idx][0] - 'A'];
  }

  pitch_idx_t frequencyToIndex (pitch_freq_t frequency, int *pDeviation)
  {
    int idx = -1;
    const int countOfFrequencies = sizeof(frequencies)/sizeof(double);
    int a = 0, b = 0, c = 0;
    unsigned int absA = 0, absB = 0, absC = 0;

    if (frequency>frequencies[countOfFrequencies-1]
        || frequency<frequencies[0])
    {
        *pDeviation = -1;
        return -1;
    }

    idx = searchFrequencyInArray(frequency, 0, countOfFrequencies-1);
    a = frequency - frequencies[idx];
    absA = fabs(a);
    b = b = idx>=countOfFrequencies-1 ? 0xffff:(frequency - frequencies[idx+1]);
    absB = idx>=countOfFrequencies-1 ? 0xffff:fabs(frequency - frequencies[idx+1]);
    c = idx<=0 ? 0xffff:(frequency - frequencies[idx-1]);
    absC = idx<=0 ? 0xffff:fabs(frequency - frequencies[idx-1]);

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

    return idx;
  }


  const pitch_freq_t * getFrequencies() { return frequencies; }

protected:
  // search algorithm
  pitch_freq_t searchFrequencyInArray(pitch_freq_t frequency, pitch_idx_t startIdx, pitch_idx_t endIdx)
  {
    if (abs(endIdx-startIdx)<=1)
    {
      return endIdx;
    }
    else if (frequency < frequencies[startIdx + ((endIdx-startIdx)>>1)])
    {
      return searchFrequencyInArray(frequency, startIdx, startIdx + ((endIdx-startIdx)>>1));
    }
    else
    {
      return searchFrequencyInArray(frequency, startIdx + ((endIdx-startIdx)>>1), endIdx);
    }
  }


private:
  const char sylableName[7][4] = { "la ", "si ", "do ", "re ", "mi ", "fa ", "sol" };


  //**********************
  // pitch table
  //**********************
  const char pitchNames[PITCH_COUNT][4] = {
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

  const pitch_freq_t frequencies[PITCH_COUNT] = {
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
 
};

#endif /* #ifdef __PITCHNODE__ */

