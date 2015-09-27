/*
 * MusicDetector.cpp
 *
 *  Created on: Sep 27, 2015
 *      Author: wujian
 */
#include <iostream>
#include "MusicDetector.h"

MusicDetector::MusicDetector() : pitchCounter(0), lastDetectedIdx(-1)
{
	// TODO Auto-generated constructor stub

}

MusicDetector::~MusicDetector() {
	// TODO Auto-generated destructor stub
}

pitch_idx_t PitchWindow::insert (pitch_idx_t idx)
{
	ring[pos++] = idx;
	pos %= windowSize;
	return detected();
}

/* get the pitch detected */
pitch_idx_t PitchWindow::detected()
{
	unsigned int i;
	pitch_idx_t result = -1;
	int maxCount = 0;
	int currentCount = 0;

	for (i = 0; i < windowSize; i++) {
		PitchWindowMap[ring[i]] = 0;
	}

	for (i = 0; i < windowSize; i++) {
		if (ring[i] != -1) {
			currentCount = ++PitchWindowMap[ring[i]];
		}
		if (currentCount > maxCount) {
			maxCount = currentCount;
			result = ring[i];
		}
	}

	return result;
}



void MusicDetector::initiaize()
{
    pitchWindow.setWindowSize(10);
    musicNoteListeners.erase(musicNoteListeners.begin(), musicNoteListeners.end());
}

void MusicDetector::registerMusicNoteListener(musicNoteListener_func_t listener)
{
	musicNoteListeners.push_back(listener);
}


void MusicDetector::startDetection(int windowSize)
{
    pitchCounter = 0;
    pitchWindow.setWindowSize(windowSize);
}

void MusicDetector::insert(pitch_freq_t pitchSample)
{
    pitch_idx_t detectedIdx = -1;
    pitch_idx_t insertIdx = -1;
    PitchDictionary pd;

    insertIdx = detectAPitch (pitchSample);
    if (insertIdx<0 || insertIdx>=PITCH_COUNT)
    {
    	    // invalid sample
        goto end;
    }

    detectedIdx = pitchWindow.insert(insertIdx);
//    printf("detectedIdx(%d), lastDetectedIdx(%d), pitchCounter(%d)\n",
//    		detectedIdx, lastDetectedIdx, pitchCounter);
//    std::cout<<"detected pitch: "<<pd.indexToPitchName(detectedIdx)<<std::endl;

    if (detectedIdx != lastDetectedIdx)
    {
		for (musicNoteListener_func_t listener : musicNoteListeners)
		{
			listener(lastDetectedIdx, pitchCounter);
		}
		lastDetectedIdx = detectedIdx;
		pitchCounter = 0;
    }
    else
    {
    	    pitchCounter++;
    }

end:
    return;
}

void MusicDetector::stopDetection()
{
    ;
}

pitch_idx_t MusicDetector::detectAPitch(pitch_freq_t pitchSample)
{
  PitchDictionary pd;
  pitch_freq_t    deviation = 0;
  pitch_idx_t     idx = -1;

  idx = pd.frequencyToIndex(pitchSample, &deviation);
#if 0
  printf ("Detected freq(%lf)\tStandard freq(%lf)\n",
		  pitchSample, pd.indexToFrequency(idx));

  printf ("Pitch Name(%s)\tdeviation(%lf)\taccuracy(%lf\%)\n",
		  pd.indexToPitchName(idx),
		  deviation,
		  deviation/pd.indexToFrequency(idx) * 100.00
		  );
#endif
  return idx;
}
