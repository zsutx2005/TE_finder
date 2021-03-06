//
// Created by Hadi Quesneville on 2018-12-21.
//

#ifndef TE_FINDER_BLRMATCHALIGN_H
#define TE_FINDER_BLRMATCHALIGN_H

#include <iostream>
#include <stdlib.h>
#include <SDGString.h>
#include <utility>
#include <map>
#include <unistd.h>
#include "RangePair.h"
#include "RangeMap.h"
#include "BlastMatch.h"
#include "RangeSeq.h"
#include "BLRBioSeqDB.h"
#include "BLRJoinParameter.h"


class BLRMatchAlign {

public:

    struct Key : std::pair<long,long>
    {
        Key(long i=-1,long j=-1)
        {
            first=i; second=j;
        };
    };
    // Key is numQ-numS

    typedef std::map<Key, std::list<RangePair> > MapAlign;

private:
    BLRJoinParameter para;
    MapAlign map_align;
    std::map<std::string,long> name2numQ,name2numS;
    std::map<long,std::string> num2nameQ,num2nameS;


    void add_clean(std::list<RangePair>& rp_list,
                   std::list<RangePair>::iterator iter);

public:

    MapAlign::iterator begin(){ return map_align.begin();};
    MapAlign::iterator end(){ return map_align.end();};

    MapAlign::const_iterator begin() const { return map_align.begin();};
    MapAlign::const_iterator end() const { return map_align.end();};

    SDGString numQ2name(long num) { return num2nameQ[num];};
    SDGString numS2name(long num) { return num2nameS[num];};

    void read(const BLRJoinParameter& param, std::istream& streamName, int verbose=0);
    void load(const BLRJoinParameter& param, const SDGString& filename, int verbose=0){
        std::ifstream input_align(filename);
        read(param,input_align, verbose);
    };
    void write(std::ostream& out);

    void clear(void){map_align.clear();};

    unsigned getNbMatchesInMapAlign(void){
        unsigned nbMatches = 0;
        for (MapAlign::iterator m = map_align.begin(); m != map_align.end(); m++)
            nbMatches += m->second.size();
        return nbMatches;};

    MapAlign getMapAlign(void){return map_align;};

    void insert(RangePair& range);


    std::map<std::string,long> getName2NumQ(void){return name2numQ;};
    std::map<long, std::string> getNum2NameQ(void){return num2nameQ;};
    std::map<std::string,long> getName2NumS(void){return name2numS;};
    std::map<long, std::string> getNum2NameS(void){return num2nameS;};

    unsigned getNbQseq(void) { return num2nameQ.size();};
    unsigned getNbSseq(void) { return num2nameS.size();};

};


#endif //TE_FINDER_BLRMATCHALIGN_H
