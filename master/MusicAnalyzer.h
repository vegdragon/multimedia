/*
 * MusicAnalyzer.h
 *
 *  Created on: Sep 27, 2015
 *      Author: wujian
 */

#ifndef MUSICANALYZER_H_
#define MUSICANALYZER_H_

#include <vector>
#include "PitchNode.h"

class MusicAnalyzer {
public:
	MusicAnalyzer();
	virtual ~MusicAnalyzer();

private:
	vector<PitchNode> pitchList;
};

#endif /* MUSICANALYZER_H_ */
