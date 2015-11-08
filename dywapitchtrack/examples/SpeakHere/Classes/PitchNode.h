#ifndef __PITCHNODE__
#define __PITCHNODE__

#include <string.h>
#include "PitchDictionary.hpp"

class PitchNode
{
public:
    PitchNode(pitch_idx_t  idx,
              pitch_freq_t freq,
              int counter,
              pitch_freq_t maxPosDev,
              pitch_freq_t maxNegDev,
              pitch_freq_t avgPosDev,
              pitch_freq_t avgNegDev)
    {
        pitchIndex = idx;
        frequency = freq;
        totalPitchCounter = counter;
        maxNegativeDeviation = maxPosDev;
        maxPositiveDeviation = maxNegDev;
        avgNegativeDeviation = avgPosDev;
        avgPositiveDeviation = avgNegDev;
    }
    
    double maxPosDeviationPercentage()
    {
        PitchDictionary pd;
        return maxPositiveDeviation * 100.00 / pd.indexToFrequency(pitchIndex);
    }
    double maxNegDeviationPercentage()
    {
        PitchDictionary pd;
        return maxNegativeDeviation * 100.00 / pd.indexToFrequency(pitchIndex);
    }
    
    double avgPosDeviationPercentage()
    {
        PitchDictionary pd;
        return avgPositiveDeviation * 100.00 / pd.indexToFrequency(pitchIndex);
    }
    double avgNegDeviationPercentage()
    {
        PitchDictionary pd;
        return avgNegativeDeviation * 100.00 / pd.indexToFrequency(pitchIndex);
    }
    
public:
    pitch_idx_t  pitchIndex;
    pitch_freq_t frequency;
    int totalPitchCounter;
    pitch_freq_t maxPositiveDeviation;
    pitch_freq_t maxNegativeDeviation;
    pitch_freq_t avgPositiveDeviation;
    pitch_freq_t avgNegativeDeviation;
};

#endif /* #ifdef __PITCHNODE__ */

