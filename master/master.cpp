#include <stdio.h>
#include <iostream>
#include <fstream>

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


void testPitchNode()
{
  PitchDictionary pd;
  int deviation;
  int idx = -1;

  idx = pd.frequencyToIndex(420, &deviation);

  cout<<"freq(420) idx: idx="<<idx<<endl;
  cout<<"Pitch Name = "<<pd.indexToPitchName(idx)<<endl;
}

int main(int argc, char** argv)
{
  char buffer[256];
  const char music_file_name[] = "music_score.dat";
  const char pitch_data_file_name[] = "pitch_data.dat";
  
  DataReader music_reader(music_file_name);
  DataReader pitch_reader(pitch_data_file_name);

  music_reader.read(buffer);
  cout<<endl;
  pitch_reader.read(buffer);

  // cin.get();

  testPitchNode();

  return 0;
}
