#ifndef __MUSIC_DETECTOR__
#define __MUSIC_DETECTOR__

#include <string.h>
#include <vector>

#include "Pitch.h"
#include "PitchDictionary.h"

typedef void (*musicNoteListener_func_t)(pitch_idx_t, int, Pitch &);

class PitchWindow {
public:
	PitchWindow() : windowSize(10), pos(0)
    {
		memset(ring, -1, sizeof(pitch_idx_t) * windowSize);
	}

	PitchWindow(unsigned int size) : pos(0)
	{
		windowSize = size;
		memset(ring, -1, sizeof(pitch_idx_t) * windowSize);
	}

	pitch_idx_t insert(pitch_idx_t idx);

	/* get the pitch detected */
	pitch_idx_t detected();

	void setWindowSize (unsigned int size)
	{
		windowSize = size;
	}
	unsigned int getWindowSize ()
	{
		return windowSize;
	}

	virtual ~PitchWindow() {};

private:
	static const int MAX_WINDOW_SIZE = 50;
    unsigned int windowSize;                // size of the window
	pitch_idx_t ring[MAX_WINDOW_SIZE];      // circular ring of pitch window
	unsigned int pos;                       // position of next incoming pitch in the window

	int PitchWindowMap[PITCH_COUNT];    // counter map of the pitch window.
};

class MusicDetector {
public:
	MusicDetector();
	virtual ~MusicDetector();

    void initiaize();
    void release() {}
	void registerMusicNoteListener(musicNoteListener_func_t listener);
    void startDetection(int windowSize);
    void insert(pitch_freq_t pitchSample);
    void stopDetection();
    pitch_idx_t detectAPitch(pitch_freq_t pitchSample, pitch_freq_t & deviation);

private:
    int detectedPitchCounter;
    pitch_idx_t lastDetectedIdx;

	std::vector<musicNoteListener_func_t> musicNoteListeners;
	PitchWindow pitchWindow;
};
#endif  /* __MUSIC_DETECTOR__ */

