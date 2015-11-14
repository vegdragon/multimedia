//============================================================================
// Name        : HelloWorld.cpp
// Author      : Jian
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>

#include "kiss_fft.h"
#include "PitchDictionary.h"

#ifndef M_PI
#define M_PI 3.14159265358979324
#endif

#define N 				1024
#define SAMPLE_RATE 	44100
#define HZ_PER_POINT 	SAMPLE_RATE/N

using namespace std;

//vector<short> audioSamples;
//
//void FileToBuffer(const string& strFilename)
//{
//    ifstream fin(strFilename.c_str(), ios::in|ios::binary);
//    short iSample;
//
//    cout<<"Samples Output"<<endl;
//    while(fin.read((char *)&iSample,sizeof(short)))
//    {
//        cout<<iSample<< " ";
//        audioSamples.push_back(iSample);
//    }
//}

void TestFft(const char* title, const kiss_fft_cpx in[N], kiss_fft_cpx out[N])
{
  kiss_fft_cfg cfg;
  double energy = -1;
  double tmp = 0;

  //printf("%s\n", title);

  if ((cfg = kiss_fft_alloc(N, 0/*is_inverse_fft*/, NULL, NULL)) != NULL)
  {
    int i = 0;
    int n = 0;
    int freq = 0;

    kiss_fft(cfg, in, out);
    free(cfg);

    for (i = 0; i < N; i++)
    {
//      printf(" in[%2zu] = %+f , %+f    "
//             "out[%2zu] = %+f , %+f\n",
//             i, in[i].r, in[i].i,
//             i, out[i].r, out[i].i);
      tmp = sqrtf(out[i].r*out[i].r + out[i].i*out[i].i);

      if (tmp > energy)
      {
          freq = tmp * N / 2;
          energy = tmp;
          n = i;
      }
    }

    if (n!=0 && energy > 1000)
    {
        PitchDictionary pd;
        pitch_freq_t deviation;
        pitch_idx_t idx = pd.frequencyToIndex(n * HZ_PER_POINT, deviation);
        const char * pitchName = pd.indexToPitchName(idx);
        printf ("energy = %f\tn = %d\tfreq1 = %d, freq2 = %d, pitchName=%s \n", energy, n, n * HZ_PER_POINT, freq, pitchName);
    }
  }
  else
  {
    printf("not enough memory?\n");
    exit(-1);
  }
}

void getFreqWithKissFft(short * auidoData, int len)
{
    kiss_fft_cpx in[N], out[N];
    int i;
    
    int count = 0;
    for (i=0;i<len;i++)
    {
        in[count].r = auidoData[i];
        in[count].i = 0;
        if (count++>=N)
        {
            TestFft("recorded data", in, out);
            count = 0;
        }
    }
}
//int main(void)
//{
//  kiss_fft_cpx in[N], out[N];
//  int i;
//#if 0
//  for (i = 0; i < N; i++)
//    in[i].r = in[i].i = 0;
//  TestFft("Zeroes (complex)", in, out);
//
//  for (i = 0; i < N; i++)
//    in[i].r = 1, in[i].i = 0;
//  TestFft("Ones (complex)", in, out);
//
//  for (i = 0; i < N; i++)
//    in[i].r = sin(2 * M_PI * 4 * i / N), in[i].i = 0;
//  TestFft("SineWave (complex)", in, out);
//#endif
//
//  FileToBuffer("violin.wav");
//  int count = 0;
//  for (std::vector<short>::iterator it = audioSamples.begin(); it != audioSamples.end(); ++it)
//  {
//	  in[count].r = *it;
//	  in[count].i = 0;
//	  if (count++>=N)
//	  {
//		TestFft("SineWave (complex)", in, out);
//		count = 0;
//	  }
//  }
//
//  return 0;
//}
