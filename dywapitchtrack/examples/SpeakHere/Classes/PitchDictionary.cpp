/*
 * PitchDictionary.cpp
 *
 *  Created on: Sep 23, 2015
 *      Author: jian
 */
#include <math.h>
#include <string.h>
#include <stdlib.h>


#include "PitchDictionary.h"

const char PitchDictionary::sylableName[7][4] = { "la ", "si ", "do ", "re ", "mi ", "fa ", "sol" };

//**********************
// pitch table
//**********************
const char PitchDictionary::pitchNames[PITCH_COUNT][4] = {
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


const pitch_freq_t PitchDictionary::frequencies[PITCH_COUNT] = {
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

const pitch_freq_t PitchDictionary::indexToFrequency (pitch_idx_t idx)
{
	return frequencies[idx];
}

const char * PitchDictionary::indexToPitchName (pitch_idx_t idx)
{
	return pitchNames[idx];
}

const char * PitchDictionary::indexToSylableName (pitch_idx_t idx)
{
	return sylableName[pitchNames[idx][0] - 'A'];
}

pitch_idx_t PitchDictionary::frequencyToIndex (pitch_freq_t frequency, pitch_freq_t & deviation)
{
	int idx = -1;
	const int countOfFrequencies = sizeof(frequencies)/sizeof(pitch_freq_t);
	pitch_freq_t a = 0, b = 0, c = 0;
	pitch_freq_t absA = 0, absB = 0, absC = 0;

	if (frequency>frequencies[countOfFrequencies-1]
		|| frequency<frequencies[0])
	{
		deviation = -1;
		return -1;
	}

	idx = searchFrequencyInArray(frequency, 0, countOfFrequencies-1);
	a = frequency - frequencies[idx];
	absA = fabs(a);
	b = idx>=countOfFrequencies-1 ? 0xffff:(frequency - frequencies[idx+1]);
	absB = idx>=countOfFrequencies-1 ? 0xffff:fabs(frequency - frequencies[idx+1]);
	c = idx<=0 ? 0xffff:(frequency - frequencies[idx-1]);
	absC = idx<=0 ? 0xffff:fabs(frequency - frequencies[idx-1]);

	if (absA>absB)
	{
		if (b>c) {
			idx = idx - 1;
			deviation = c;
		}
		else {
			idx = idx + 1;
			deviation = b;
		}
	}
	else
	{
		if (absA>absC) {
			idx = idx - 1;
			deviation = c;
		}
		else
		{
			idx = idx - 0;
			deviation = a;
		}
	}

	return idx;
}


const pitch_freq_t * PitchDictionary::getFrequencies() { return frequencies; }


// search algorithm
pitch_freq_t PitchDictionary::searchFrequencyInArray(pitch_freq_t frequency, pitch_idx_t startIdx, pitch_idx_t endIdx)
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
