#ifndef __PITCHNODE__
#define __PITCHNODE__

#include <string.h>
#include "PitchDictionary.h"

#define MAX(a, b) (a)>(b) ? (a):(b)
#define MIN(a, b) (a)<(b) ? (a):(b)

class Pitch
{
public:
	Pitch();

	Pitch(pitch_idx_t  idx,
				pitch_freq_t freq,
				int counter,
				pitch_freq_t maxPosDev,
				pitch_freq_t maxNegDev,
				pitch_freq_t avgPosDev,
				pitch_freq_t avgNegDev);

	Pitch(Pitch & pitchNode);

	double maxPosDeviationPercentage();
	double maxNegDeviationPercentage();
	double avgPosDeviationPercentage();
	double avgNegDeviationPercentage();

public:
	static const pitch_duration_t DURATION_PER_SAMPLE_IN_MS;
    pitch_idx_t  	_pitchIndex;
    pitch_freq_t 	_frequency;
    int 			_totalPitchCounter;
    pitch_freq_t 	_maxPositiveDeviation;
    pitch_freq_t 	_maxNegativeDeviation;
    pitch_freq_t 	_avgPositiveDeviation;
    pitch_freq_t 	_avgNegativeDeviation;
    pitch_duration_t _duration;
    Pitch * 			_next;
};

class PitchNodeList
{
public:
	PitchNodeList();
	void push(Pitch * pNode);
    void removeHead() { _head = _head->_next; }
	Pitch * head();
	Pitch * last();
private:
	Pitch * _head;
	Pitch * _tail;
};

class Music
{
public:
	Music();
	virtual ~Music();
	void insertAndMergeANote (Pitch * pPitch);
	PitchNodeList & getNodeList();
	void display ();

private:
	PitchNodeList _pitchNodeList;
};

#endif /* #ifdef __PITCHNODE__ */

