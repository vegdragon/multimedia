#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>

#include "PitchNode.h"

using namespace std;


class DataReader
{
public:
  DataReader(const char * source) : data_source(source) {}
  ~DataReader() {}

  void read(char * buffer); 

private:
  const char * data_source;
};


void DataReader::read(char* buffer)
{
  fstream fin;
  fin.open(data_source, ios::in);
  if (!fin.good()) { cout<<"failed to open file"; return; }

  cout<<data_source<<" contents: "<<endl;
  while(!fin.eof())
  {
    fin.getline(buffer,256,'\n');
    cout<<buffer<<endl;
  }

  fin.close();

  return;
}



void testPitchNode(double pitchSample)
{
  PitchDictionary pd;
  pitch_freq_t    deviation = 0;
  int             idx = -1;

  idx = pd.frequencyToIndex(pitchSample, &deviation);

  printf ("Detected freq(%lf)\tStandard freq(%lf)\n",
		  pitchSample, pd.indexToFrequency(idx));

  printf ("Pitch Name(%s)\tdeviation(%lf)\taccuracy(%lf\%)\n",
		  pd.indexToPitchName(idx),
		  deviation,
		  deviation/pd.indexToFrequency(idx) * 100.00
		  );
}

int main(int argc, char** argv)
{
  char buffer[256];
  const char music_file_name[] = "music_score.dat";
  // const char pitch_data_file_name[] = "pitch_data.dat";
  const char pitch_samples_file_name[] = "pitch_samples_freq_(a3-c4-d4#-f4#)";
  std::vector<double> pitchSamples;
  
  DataReader music_reader(music_file_name);
  DataReader pitch_sample_reader(pitch_samples_file_name);

  music_reader.read(buffer);
  cout<<endl;
  pitch_sample_reader.read(buffer);
  cout<<endl;

  // cin.get();
  //
  //check to see that the file was opened correctly:
  std::ifstream ifile(pitch_samples_file_name, std::ios::in);
  if (!ifile.is_open()) {
	  std::cerr << "There was a problem opening the input file!\n";
	  exit(1);//exit or do additional error checking
  }

  double num = 0.0;
  //keep storing values from the text file so long as data exists:
  while (ifile >> num) {
	  pitchSamples.push_back(num);
  }

  //verify that the scores were stored correctly:
  PitchWindow pw;
  for (unsigned int i = 0; i < pitchSamples.size(); ++i) {
	  std::cout << pitchSamples[i] << std::endl;
	  testPitchNode(pitchSamples[i]);
  }



  return 0;
}
