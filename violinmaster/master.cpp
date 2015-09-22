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
  fstream fout;
  fout.open(data_source, ios::in);
  cout<<data_source<<" contents: "<<endl;
  while(!fout.eof())
  {
    fout.getline(buffer,256,'\n');
    cout<<buffer<<endl;
  }

  fout.close();

  return;
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

  cin.get();

  return 0;
}
