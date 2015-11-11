/*
 * Pitch.cpp
 *
 *  Created on: Oct 6, 2015
 *      Author: wujian
 */
#include <stdio.h>
#include "Pitch.h"

const pitch_duration_t Pitch::DURATION_PER_SAMPLE_IN_MS = 1024.00 * 1000.00 / 44100.00;

Pitch::Pitch() : _next(NULL) {}

Pitch::Pitch(pitch_idx_t  idx,
			pitch_freq_t freq,
			int counter,
			pitch_freq_t maxPosDev,
			pitch_freq_t maxNegDev,
			pitch_freq_t avgPosDev,
			pitch_freq_t avgNegDev) : _next(NULL)
{
	_pitchIndex = idx;
	_frequency = freq;
	_totalPitchCounter = counter;
	_maxNegativeDeviation = maxPosDev;
	_maxPositiveDeviation = maxNegDev;
	_avgNegativeDeviation = avgPosDev;
	_avgPositiveDeviation = avgNegDev;
	_duration = DURATION_PER_SAMPLE_IN_MS * _totalPitchCounter;
}

Pitch::Pitch(Pitch & pitchNode) : _next(NULL)
{
	_pitchIndex = pitchNode._pitchIndex;
	_frequency = pitchNode._frequency;
	_totalPitchCounter = pitchNode._totalPitchCounter;
	_maxNegativeDeviation = pitchNode._maxNegativeDeviation;
	_maxPositiveDeviation = pitchNode._maxPositiveDeviation;
	_avgNegativeDeviation = pitchNode._avgNegativeDeviation;
	_avgPositiveDeviation = pitchNode._avgPositiveDeviation;
	_duration = DURATION_PER_SAMPLE_IN_MS * pitchNode._totalPitchCounter;
}

double Pitch::maxPosDeviationPercentage()
{
	PitchDictionary pd;
	return _maxPositiveDeviation * 100.00 / pd.indexToFrequency(_pitchIndex);
}
double Pitch::maxNegDeviationPercentage()
{
	PitchDictionary pd;
	return _maxNegativeDeviation * 100.00 / pd.indexToFrequency(_pitchIndex);
}

double Pitch::avgPosDeviationPercentage()
{
	PitchDictionary pd;
	return _avgPositiveDeviation * 100.00 / pd.indexToFrequency(_pitchIndex);
}
double Pitch::avgNegDeviationPercentage()
{
	PitchDictionary pd;
	return _avgNegativeDeviation * 100.00 / pd.indexToFrequency(_pitchIndex);
}




PitchNodeList::PitchNodeList() {}
void PitchNodeList::push(Pitch * pNode)
{
	if (_tail != NULL)
	{
		_tail->_next = pNode;
		pNode->_next = NULL;
		_tail = pNode;
	}
	else
	{
		pNode->_next = NULL;
		_head = pNode;
		_tail = pNode;
	}
}


Pitch * PitchNodeList::head()
{
	return _head;
}

Pitch * PitchNodeList::last()
{
	return _tail;
}



Music::Music()
{
	;
}

Music::~Music()
{
	Pitch * pPitch = NULL;
	while ((pPitch = _pitchNodeList.head()) != NULL)
	{
        _pitchNodeList.removeHead();
		delete pPitch;
		pPitch = NULL;
	}
}

void Music::insertAndMergeANote (Pitch * pPitch)
{
	Pitch * pLastPitch = _pitchNodeList.last();
	if (NULL != pLastPitch)
	{
		if (pLastPitch->_pitchIndex == pPitch->_pitchIndex)
		{
			// same pitch as the last one, merge the _duration
			pLastPitch->_maxNegativeDeviation = MIN(pLastPitch->_maxNegativeDeviation, pPitch->_maxNegativeDeviation);
			pLastPitch->_maxPositiveDeviation = MAX(pLastPitch->_maxPositiveDeviation, pPitch->_maxPositiveDeviation);
			pLastPitch->_avgNegativeDeviation = (pLastPitch->_avgNegativeDeviation * pLastPitch->_totalPitchCounter + pPitch->_avgNegativeDeviation * pPitch->_totalPitchCounter)
					/ (pLastPitch->_totalPitchCounter + pPitch->_totalPitchCounter);
			pLastPitch->_avgPositiveDeviation = (pLastPitch->_avgPositiveDeviation * pLastPitch->_totalPitchCounter + pPitch->_avgPositiveDeviation * pPitch->_totalPitchCounter)
									/ (pLastPitch->_totalPitchCounter + pPitch->_totalPitchCounter);
			pLastPitch->_totalPitchCounter += pPitch->_totalPitchCounter;
			pLastPitch->_duration = Pitch::DURATION_PER_SAMPLE_IN_MS * pLastPitch->_totalPitchCounter;

			delete pPitch;
			pPitch = NULL;
		}
		else
		{
			_pitchNodeList.push(pPitch);
		}
	}
	else
	{
		_pitchNodeList.push(pPitch);
	}
}

PitchNodeList & Music::getNodeList()
{
	return _pitchNodeList;
}

void Music::display ()
{
	Pitch * pPitch = _pitchNodeList.head();
	PitchDictionary pd;
	while (pPitch != NULL)
	{
		printf ("Detected Note(%s), _duration(%lf).\n",
				pd.indexToPitchName(pPitch->_pitchIndex),
				pPitch->_duration);
		printf ("PitchNode Statistics: %.2lf[%.2lf%%], %.2lf[%.2lf%%], %.2lf[%.2lf%%], %.2lf[%.2lf%%]\n",
				pPitch->_avgNegativeDeviation, pPitch->avgNegDeviationPercentage(),
				pPitch->_avgPositiveDeviation, pPitch->avgPosDeviationPercentage(),
				pPitch->_maxNegativeDeviation, pPitch->maxNegDeviationPercentage(),
				pPitch->_maxPositiveDeviation, pPitch->maxPosDeviationPercentage()
				);
		pPitch = pPitch->_next;
	}
}



