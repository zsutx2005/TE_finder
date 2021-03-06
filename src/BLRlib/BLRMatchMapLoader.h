/***
 *
 * BLRMatchMapLoader.h
 *
 ***/

#ifndef BLRMATCHMAPLOADER_H
#define BLRMATCHMAPLOADER_H

#include <iostream>
#include <stdlib.h>
#include <SDGString.h>
#include <utility>
#include <map>
#include "BLRMatchMap.h"

class BLRMatchMapLoader
{
	public:
	void readAlign(BLRMatchMap& blrmm, std::istream& input_align, int verbose=0);
	void loadAlign(BLRMatchMap& blrmm, SDGString filename, int verbose=0){
		std::ifstream input_align(filename);
		readAlign(blrmm, input_align, verbose);
	};

	void readPath(BLRMatchMap& blrmm, std::istream& input_path, int verbose=0);
	void loadPath(BLRMatchMap& blrmm, SDGString filename, int verbose=0){
		std::ifstream input_path(filename);
		readPath(blrmm, input_path, verbose);
	};
	
				
};

#endif
