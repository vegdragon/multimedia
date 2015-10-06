/*
 * MusicDetector.cpp
 *
 *  Created on: Sep 27, 2015
 *      Author: wujian
 */
#include <iostream>
#include "MusicDetector.h"

MusicDetector::MusicDetector() : detectedPitchCounter(0), lastDetectedIdx(-1)
{
	;
}

MusicDetector::~MusicDetector()
{
	;
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
    detectedPitchCounter = 0;
    pitchWindow.setWindowSize(windowSize);
}

void MusicDetector::insert(pitch_freq_t pitchSample)
{
    pitch_idx_t detectedIdx = -1;
    pitch_idx_t insertIdx = -1;
    pitch_freq_t deviation = -1;
    static pitch_freq_t maxPositiveDeviation = 0;
    static pitch_freq_t maxNegativeDeviation = 0;
    static pitch_freq_t acuPositiveDeviation = 0;
    static pitch_freq_t acuNegativeDeviation = 0;
    static int totalPitchCounter = 0;
    PitchDictionary pd;

    insertIdx = detectAPitch (pitchSample, deviation);
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
    		musicNoteListener_func_t listener;
		for (std::vector<musicNoteListener_func_t>::iterator it = musicNoteListeners.begin(); it != musicNoteListeners.end(); ++it)
		{
			Pitch pitchNode(
					lastDetectedIdx,
					pd.indexToFrequency(lastDetectedIdx),
					totalPitchCounter,
					maxNegativeDeviation,
					maxPositiveDeviation,
					totalPitchCounter!=0 ? acuNegativeDeviation/totalPitchCounter:-0xfff,
					totalPitchCounter!=0 ? acuPositiveDeviation/totalPitchCounter:0xfff
					);
			listener = *it;
			listener(lastDetectedIdx, detectedPitchCounter, pitchNode);
		}
		lastDetectedIdx = detectedIdx;
		detectedPitchCounter = 0;
		maxPositiveDeviation = 0;
		maxNegativeDeviation = 0;
		acuPositiveDeviation = 0;
		acuNegativeDeviation = 0;
		totalPitchCounter = 0;
    }
    else
    {
		detectedPitchCounter++;

		// calculate deviation
		if (insertIdx == detectedIdx)
		{
			if (deviation > 0)
			{
				maxPositiveDeviation = deviation>maxPositiveDeviation ? deviation:maxPositiveDeviation;
				acuPositiveDeviation += deviation;
			}
			else
			{
				maxNegativeDeviation = deviation<maxNegativeDeviation ? deviation:maxNegativeDeviation;
				acuNegativeDeviation += deviation;
			}
			totalPitchCounter++;
		}
    }

end:
    return;
}

void MusicDetector::stopDetection()
{
    ;
}

pitch_idx_t MusicDetector::detectAPitch(pitch_freq_t pitchSample, pitch_freq_t & deviation)
{
  PitchDictionary pd;
  pitch_idx_t     idx = -1;

  idx = pd.frequencyToIndex(pitchSample, deviation);
#if 0
  printf ("Detected freq(%lf)\tStandard freq(%lf)\n",
		  pitchSample, pd.indexToFrequency(idx));

  printf ("Pitch Name(%s)\tdeviation(%lf)\taccuracy(%lf\%)\n",
		  pd.indexToPitchName(idx),
		  deviation,
		  deviation/pd.indexToFrequency(idx) * 100.00
		  );
#endif

  // TODO. record & calculate accuracy rate.

  return idx;
}
