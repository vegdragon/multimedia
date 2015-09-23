#ifndef __PITCH_DICTIONARY__
#define __PITCH_DICTIONARY__

#define PITCH_COUNT     108
typedef int             pitch_idx_t;
typedef int             pitch_freq_t;

class PitchDictionary
{
public:
  const char * indexToPitchName (pitch_idx_t idx);
  const char * indexToSylableName (pitch_idx_t idx);
  pitch_idx_t frequencyToIndex (pitch_freq_t frequency, int *pDeviation);
  const pitch_freq_t * getFrequencies();

protected:
  // search algorithm
  pitch_freq_t searchFrequencyInArray(pitch_freq_t frequency, pitch_idx_t startIdx, pitch_idx_t endIdx);

private:
  static const char sylableName[7][4];


  //**********************
  // pitch table
  //**********************
  static const char pitchNames[PITCH_COUNT][4];

  /* frequency table */
  static const pitch_freq_t frequencies[PITCH_COUNT];
};

#endif /* __PITCH_DICTIONARY__ */
