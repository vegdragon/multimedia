class PitchNode
{

private:
  int        frequency;
  char       syllableName[4];
  PitchNode  next;
};

class PitchWindow
{
public:
  PitchWindow():pos(0) { memset (ring, 0, WINDOW_SIZE); }
  void insert ()

private:
  const unsigned int WINDOW_SIZE; 
  char ring[WINDOW_SIZE];
  unsigned int pos;
};


class PitchDesc
{

private:
  const char pitchName[][]   = { "Ab", "A", "A#", "Bb", "B", "B#", "Cb", "C", "C#", "Db", "D", "D#", "Eb", "E", "E#", "Fb", "F", "F#", "Gb", "G", "G#" };
  const char sylableName[][] = { "la", "la","la", "si", "si","si", "si", "do","do", "re", "re","re", "mi", "mi","mi", "fa","fa", "fa", "sol","sol","sol" };
  
};
