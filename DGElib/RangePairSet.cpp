/*
 * \file RangePairSet.cpp
 */

#include "RangePairSet.h"

void RangePairSet::view( void ) const
{
	std::cout<<std::endl;
	RangePair::view();
	for(std::list<RangePair>::const_iterator i=path.begin();i!=path.end();i++)
		std::cout<<"\t"<<*i<<std::endl;
}

void RangePairSet::viewWithLabel( void ) 
{
	std::cout<<"\n-------------------Range set--------------------------"<<std::endl;
	RangePair::viewWithLabel();
	std::cout<<"-----------------connected ranges---------------------"<<std::endl;
	for(std::list<RangePair>::iterator i=path.begin();i!=path.end();i++)
		i->viewWithLabel();
	std::cout<<"------------------------------------------------------"<<std::endl;
}


void RangePairSet::computeScoreWithDynaProg(double mism,double gapo_p,double gape_p)
{
	std::list<RangePair>::const_iterator prev,r=path.begin();
	prev=r++;
	score=prev->getScore();
	while(r!=path.end())
	{
		long long diag1=
			(long long)prev->getRangeQ().getEnd()
			-(long long)prev->getRangeS().getEnd();
		long long diag2=
			(long long)r->getRangeQ().getStart()
			-(long long)r->getRangeS().getStart();
		long long gap=llabs(diag1-diag2);

		long long mismatch;
		if(diag1>diag2)
			mismatch=
				(long long)r->getRangeQ().getStart()
				-(long long)prev->getRangeQ().getEnd();
		else
			mismatch=
				(long long)r->getRangeS().getStart()
				-(long long)prev->getRangeS().getEnd();

		mismatch=mismatch>0?mismatch:0;

		score+=r->getScore()
		-(long long)rint(gapo_p+gape_p*gap)
		-(long long)rint(mismatch*mism);
		prev=r++;
	}
	score=score>0?score:0;
}

void RangePairSet::computeScoreWithLength(void)
{
	unsigned long sumScore = 0;
	for (std::list<RangePair>::iterator i =  path.begin(); i !=path.end(); i++)
	{
		unsigned long score = (i->getRangeQ().getEnd() - i->getRangeQ().getStart()) + 1;
		i->setScore(score);
		i->setLength(score);
		sumScore = sumScore + score;
	}
	setScore(sumScore);
	setLength(sumScore);
}

void RangePairSet::setPath(const std::list<RangePair> rp_list,double mism,double gapo_p,double gape_p)
{
	path.clear();
	if(rp_list.empty())
		return;
	// Update for gcc 4.6.3
	//*this=RangePairSet::RangePairSet(rp_list.front());

	*this=RangePairSet(rp_list.front());

	std::list<RangePair>::const_iterator prev,r=rp_list.begin();
	prev=r++;
	while(r!=rp_list.end())
	{
		first.merge(r->first);
		second.merge(r->second);
		identity=(((identity/100)*length+(r->getIdentity()/100)*r->getLength())
				/(length+r->getLength()))*100;
		e_value=std::min(e_value,r->getE_value());
		length+=r->getLength();
		path.push_back(*r);
		prev=r++;
	}
	computeScoreWithDynaProg(mism,gapo_p,gape_p);
	if(!isPlusStrand())
		path.sort(RangePair::greater);
}

void RangePairSet::setPath(double mism,double gapo_p,double gape_p)
{
  RangePairSet::setPath(path, mism, gapo_p,gape_p);
}

void RangePairSet::addPath(const RangePair& rp)
{
  path.push_back(rp);
}

void RangePairSet::write(std::ostream& out, unsigned id,
		const std::string& nameQ, const std::string& nameS)  const
{
	
	for(std::list<RangePair>::const_iterator i=path.begin();i!=path.end();i++)
	{
		out<<id<<"\t"<<nameQ
		<<"\t"<<i->getRangeQ().getStart()
		<<"\t"<<i->getRangeQ().getEnd()
		<<"\t"<<nameS
		<<"\t"<<i->getRangeS().getStart()
		<<"\t"<<i->getRangeS().getEnd()
		<<"\t"<<i->getE_value()
		<<"\t"<<i->getScore()
		<<"\t"<<i->getIdentity()
		<<std::endl;
	}
}

void RangePairSet::writePath(std::ostream& out, unsigned id,
		const std::string& nameQ, const std::string& nameS)  const
{

	for(std::list<RangePair>::const_iterator i=path.begin();i!=path.end();i++)
	{
		out<<id<<"\t"<<nameQ
		<<"\t"<<i->getRangeQ().getStart()
		<<"\t"<<i->getRangeQ().getEnd()
		<<"\t"<<nameS
		<<"\t"<<i->getRangeS().getStart()
		<<"\t"<<i->getRangeS().getEnd()
		<<"\t"<<i->getE_value()
		<<"\t"<<i->getScore()
		<<"\t"<<i->getIdentity()
		<<std::endl;
	}
}

void RangePairSet::writeGFF3(std::ostream& out, unsigned id,
		const std::string& nameQ, const std::string& nameS, const std::string& source) const
{
	out<<nameQ
	<<"\t"<<source
	<<"\t"<<"match"
	<<"\t"<<getRangeQ().getMin()
	<<"\t"<<getRangeQ().getMax()
	<<"\t"<<getScore()<<"\t";
	if(isPlusStrand()) out<<"+";
	else out<<"-";
	out<<"\t."
	<<"\tID="<<id
	<<";Name="<<nameS
	<<";Target="<<nameS<<" "<<getRangeS().getMin()<<" "<<getRangeS().getMax()
	<<";Note=e-value:"<<getE_value()
	<<",identity:"<<getIdentity()
	<<std::endl;

	int count=0;
	for(std::list<RangePair>::const_iterator i=path.begin();i!=path.end();i++)
	{
		count++;
		out<<nameQ
		<<"\t"<<source
		<<"\t"<<"match_part"
		<<"\t"<<i->getRangeQ().getMin()
		<<"\t"<<i->getRangeQ().getMax()
		<<"\t"<<i->getScore()<<"\t";
		if(isPlusStrand()) out<<"+";
		else out<<"-";
		out<<"\t."
		<<"\tID="<<id<<"."<<count
		<<";Parent="<<id
		<<";Name="<<nameS<<"."<<count
		<<";Target="<<nameS<<" "<<getRangeS().getMin()<<" "<<getRangeS().getMax()
		<<";Note=e-value:"<<getE_value()
		<<",identity:"<<getIdentity()
		<<std::endl;
	}
}

void RangePairSet::writeBED(std::ostream& out, const std::string& nameQ, const std::string& nameS, const std::string& color) const
{
	
	ulong referenceMin = getRangeQ().getMin();
	out<<nameQ
	<<"\t"<<referenceMin
	<<"\t"<<getRangeQ().getMax()
	<<"\t"<<nameS
	<<"\t"<<getScore()<<"\t";
	if(isPlusStrand()) out<<"+";
	else out<<"-";
	out<<"\t"<<getRangeQ().getMin()
	<<"\t"<<getRangeQ().getMax()
	<<"\t"<<color;
	
	std::vector<unsigned> vecBlockSize; 
	//std::vector<int> vecBlockStarts;
	if (path.size() > 1){
		// block count
		out<<"\t"<<path.size()<<"\t";
		// block sizes
		for(std::list<RangePair>::const_iterator i=path.begin();i!=path.end();i++)
		{
			out<<i->getRangeQ().getLength()<<",";
		}
		out<<"\t";
		// block starts
		for(std::list<RangePair>::const_iterator i=path.begin();i!=path.end();i++)
		{
			ulong relativeMin = i->getRangeQ().getMin() - referenceMin;
			out<<relativeMin<<",";
		}

	}
	out<<std::endl;
}

void RangePairSet::writeRpsAttr(std::ostream& out, unsigned id,
		const std::string& nameQ, const std::string& nameS) const
{
	out<<"["<<id<<"\t"<<nameQ
	<<"\t"<<getRangeQ().getStart()
	<<"\t"<<getRangeQ().getEnd()
	<<"\t"<<nameS
	<<"\t"<<getRangeS().getStart()
	<<"\t"<<getRangeS().getEnd()
	<<"\t"<<getE_value()
	<<"\t"<<getScore()
	<<"\t"<<getIdentity()
	<<"]"<<std::endl;
}

bool RangePairSet::diffQ(const RangePairSet& r)
{
	bool modif=false;

	std::list<RangePair> r_path=r.path;
	for(std::list<RangePair>::iterator i=path.begin();i!=path.end();i++)
		for(std::list<RangePair>::iterator j=r_path.begin();j!=r_path.end();
		j++)
		{
			if (i->overlapQ(*j))
			{
				modif=true;
				RangePair new_r=i->diffQ(*j);
				if(!new_r.empty() && new_r.getRangeQ().getLength()>= 10)
				{
					path.push_back(new_r);
				}
				if(i->empty() || i->getRangeQ().getLength()< 10)
				{
					i=path.erase(i);
					i--;
					break;
				}
			}
		}

	if(path.empty())
		clear();

	setPath(path); //warning: recompute score with alignment penalties at 0
	return modif;
}


bool RangePairSet::split(const RangePairSet& r, std::list<RangePairSet>& lrp )
{
  bool modif=false;
  lrp.clear();
  std::list<RangePair> r_path=r.path;
  r_path.sort(RangePair::less);
  path.sort(RangePair::less);
  std::list<RangePair>::iterator i=path.begin();
  std::list<RangePair>::iterator j=r_path.begin();
  while (i!=path.end())
    {
      while( j!=r_path.end() && *j<*i) j++;
      if(j==r_path.end()) break;
      std::list<RangePair>::iterator next_i=i;
      next_i++;
      if(next_i==path.end()) break;
      if(*next_i>*j)
	{
	  modif=true;
	  std::list<RangePair> lpath;
	  lpath.splice(lpath.begin(),path,path.begin(),next_i);
	  RangePairSet new_rps(lpath);
	  lrp.push_back(new_rps);
	  i=next_i;
	}
      else i++;
    }
  setPath(path);
  return modif;
}


bool RangePairSet::inserted(const RangePairSet& r)
{
  std::list<RangePair> r_path=r.path;
  r_path.sort(RangePair::less);
  path.sort(RangePair::less);
  std::list<RangePair>::iterator prev_i,i=path.begin();
  std::list<RangePair>::iterator j=r_path.begin();
  if(j->getRangeQ().getMin()<i->getRangeQ().getMax()){
    return false;
  }
  while(i!=path.end() && j->getRangeQ().getMax()>i->getRangeQ().getMin()) i++;
  if(i==path.end()){
    return false;
  }
  prev_i=i;
  prev_i--;
  if(prev_i==path.end())
{

    return false;
}
  while( j->getRangeQ().getMax()<i->getRangeQ().getMin()
	 && j->getRangeQ().getMin()>prev_i->getRangeQ().getMax()
	 && j!=r_path.end()) j++;
  if(j==r_path.end())
{
    return true;
}

  return false;
}


unsigned RangePairSet::overlapQ_length(const RangePairSet& r) const
{
	unsigned overlap=0;
	if(getRangeQ().getNumChr()!=r.getRangeQ().getNumChr()) return false;
	for(std::list<RangePair>::const_iterator i=path.begin();
	i!=path.end();i++)
	{
		bool found=false;
		for(std::list<RangePair>::const_iterator j=r.path.begin();
		j!=r.path.end();j++)
		{
			if(i->getRangeQ().overlap(j->getRangeQ()))
			{
				found=true;
				if(i->getRangeQ().isIncluded(j->getRangeQ()))
					overlap+=j->getRangeQ().getLength();
				else if (i->getRangeQ().isContained(j->getRangeQ()))
					overlap+=i->getRangeQ().getLength();
				else if (i->getRangeQ().getMin()>j->getRangeQ().getMin())
					overlap+=j->getRangeQ().getMax()-i->getRangeQ().getMin();
				else
					overlap+=i->getRangeQ().getMax()-j->getRangeQ().getMin();
			}
			else if(found) break;
		}
	}
	return overlap;
}

void RangePairSet::setPathDirectly(const std::list<RangePair> rp_list){
    path = rp_list;
}

bool operator==( const RangePairSet &rps1, const RangePairSet &rps2 )
{

	return( rps1.first == rps2.first
			&& rps1.second == rps2.second
			&& rps1.e_value == rps2.e_value
			&& rps1.score == rps2.score
			&& rps1.identity == rps2.identity
			&& rps1.path == rps2.path);
}

bool operator!=(const RangePairSet &rps1, const RangePairSet &rps2)
{
	return !(rps1 == rps2);
}

void RangePairSet::mergeQ(RangePairSet& rpsOther) 
{
	cleanConflictsOnOverlappingQuery(rpsOther);
	
	mergeCurrentPathWithOtherPath(rpsOther);
	path.sort(RangePairSet::compareCoordsOnQuery);
	updateSubject(rpsOther);
	updateQuery();
	updateScoreLengthAndIdentityAndEValue();
}

void RangePairSet::cleanConflictsOnOverlappingQuery(RangePairSet &rpsOther)
{
	if (this->score >= rpsOther.getScore()){
		rpsOther.diffQ(*this); 
	}else{
		this->diffQ(rpsOther);
	}
}

void RangePairSet::mergeCurrentPathWithOtherPath(RangePairSet &rpsOther)
{
	std::list<RangePair> newPath;
	for(std::list<RangePair>::const_iterator i=path.begin();
	i!=path.end();i++)
	{
		newPath.push_back(*i);
	}

	std::list<RangePair> pathOther = rpsOther.getPath();
	for(std::list<RangePair>::const_iterator i=pathOther.begin();
	i!=pathOther.end();i++)
	{
		newPath.push_back(*i);
	}
	setPathDirectly(newPath);
}

bool RangePairSet::compareCoordsOnQuery(const RangePairSet &rps1, const RangePairSet &rps2)
{	
	if (rps1.first.getNumChr() == rps2.first.getNumChr()){
		return rps1.getRangeQ() < rps2.getRangeQ();
	}else{
		return rps1.first.getNumChr() < rps2.first.getNumChr();
	}
}

bool RangePairSet::compareScoreDecreasing(const RangePairSet &rps1, const RangePairSet &rps2)
{	
	return rps1.getScore() > rps2.getScore();
}
void RangePairSet::updateSubject(RangePairSet &rpsOther)
{
	
	getRangeS().setNumChr(-1);
	getRangeS().setNameSeq("-1");
	getRangeS().setStart(0);
	getRangeS().setEnd(0);
}

void RangePairSet::updateQuery(void)
{
	RangePair firstRps = path.front(); 
	RangePair lastRps = path.back();
	getRangeQ().setStart(firstRps.getRangeQ().getMin());
	getRangeQ().setEnd(lastRps.getRangeQ().getMax());
	getRangeQ().setNumChr(firstRps.getRangeQ().getNumChr());
	getRangeQ().setNameSeq(firstRps.getRangeQ().getNameSeq());
}

// TODO: duplication with computeScoreWithLength
void RangePairSet::updateScoreLengthAndIdentityAndEValue(void)
{
	unsigned long sumScore = 0;
	for (std::list<RangePair>::iterator i =  path.begin(); i !=path.end(); i++)
	{
		unsigned long score = (i->getRangeQ().getMax() - i->getRangeQ().getMin()) + 1;
		sumScore = sumScore + score;
	}
	setScore(sumScore);
	setLength(sumScore);
	setIdentity(0);
	setE_value(0);
}

void RangePairSet::orientSubjects(void)
{
	bool orientOnPlusStrand = doOrientOnPlusStrand();
	for(std::list<RangePair>::iterator m=path.begin(); m!=path.end(); m++)
	{
		bool isPlusStrand = m->isPlusStrand();
		if (orientOnPlusStrand)
		{
			if (!isPlusStrand){
				m->second.reverse();
			}		
		}else if (isPlusStrand){
				m->second.reverse();
			}
		
	}
    	path.sort(RangePairSet::compareCoordsOnQuery);
	setPathDirectly(path);
}

bool RangePairSet::doOrientOnPlusStrand(void)
{
	unsigned plusStrandScore = 0;
	unsigned minusStrandScore = 0;
	
	for(std::list<RangePair>::iterator m=path.begin(); m!=path.end(); m++)
	{
    		if (m->isPlusStrand())
			plusStrandScore = plusStrandScore + m->getScore();
		else
			minusStrandScore = minusStrandScore + m->getScore();
	}
 	return plusStrandScore > minusStrandScore;		
}

void RangePairSet::readtxt(std::istream& in)
{
      char buff[1024];
      in.getline(buff,1023,'\t');
      id=atol(buff);
      RangePair::readtxt(in);
      path.push_back(*this);
}
