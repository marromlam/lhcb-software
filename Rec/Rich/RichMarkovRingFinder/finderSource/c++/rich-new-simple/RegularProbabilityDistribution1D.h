
#ifndef LESTER_REGULAR_PROBABILITY_DISRIBUTION_1D_H
#define LESTER_REGULAR_PROBABILITY_DISRIBUTION_1D_H

#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include "CLHEP/Random/RandFlat.h"

namespace Lester {

  template <class Index>
  class RegularProbabilityDistribution1D {
  public:
    class EvaluatedTooEarly {
    };
    class NotEnoughData : public std::exception {
      const char * what() const throw () {
        return "Not Enough Data";
      }
    };
    class DataNotRegular {
    };
  private:
    Index minX;
    Index maxX;
    Index deltaX;
    Index delta;
    Index start;
    Index stop;
    Index sda;
    double totProb;
    typedef std::vector<Index> XVals;
    typedef std::vector<double> PVals;
    XVals xVals;
    PVals pVals;
  public:
    RegularProbabilityDistribution1D() : totProb(0) {
    };
    RegularProbabilityDistribution1D(std::string fileName) : totProb(0) {
      std::ifstream f(fileName.c_str());
      readFromStream(f);
    };
    RegularProbabilityDistribution1D(std::istream & is) : totProb(0) {
      readFromStream(is);
    };
  private:
    void readFromStream(std::istream & is) {
      Index index;
      double probability;
      while (is>>index) {
        is>>probability;
        addIndexAndProbability(index,probability);
      };
      finishInitialisation();
    };
    void finishInitialisation() {
      assert(xVals.size()==pVals.size());
      if (xVals.size()<2) {
        std::cout << "About to throw a benny" << std::endl;
        throw NotEnoughData();
      };
      //Check that the data really is "regular"
      minX=xVals[0];
      maxX=(*(xVals.end()-1));
      delta = maxX-minX;
      const double nGaps = static_cast<double>(xVals.size()-1);
      assert(nGaps>=1);
      deltaX = delta/nGaps;
      checkRegularity();
      start = minX - deltaX*0.5;
      stop = maxX + deltaX*0.5;
      sda = static_cast<double>(xVals.size())/(stop-start);
      normalise();
    };
    void checkRegularity() const {
      if (xVals.size()<2) {
        std::cout << "About to throw a benny" << std::endl;
        throw NotEnoughData();
      };
      const double tol = 0.0001;
      for (typename XVals::const_iterator xit = xVals.begin();
           xit+1 != xVals.end();
           ++xit) {
        const double miniDeltaX = (*(xit+1))-(*xit);
        const double discrep = fabs((miniDeltaX-deltaX)/(miniDeltaX+deltaX));
        if (discrep>tol) {
          std::cout << "About to throw a benny" << std::endl;
          throw DataNotRegular();
        };
      };
    };
    void normalise() {
      const double totArea = totProb * deltaX;
      for (PVals::iterator pit = pVals.begin(); pit != pVals.end(); ++pit) {
        (*pit)/=totArea;
      };
      // now totArea will be one!
    };
    void clear() {
      // Resets everything
      xVals.clear();
      pVals.clear();
      totProb = 0;
    };
  private:
    void addIndexAndProbability(const Index index, const double probability) {
      assert(probability>=0);
      xVals.push_back(index);
      pVals.push_back(probability);
      totProb += probability;
    };
  public:
    inline double probabilityDensityOf(const Index index) const {
      const int i = static_cast<int>((index - start)*sda);
      //std::cout << index<< " " << start << " " << stop << " " <<sda<<" "<< i << std::endl;
      if (i>=0 && i<static_cast<int>(pVals.size())) {
        return pVals[i];
      } else {
        return 0;
      };
    };
    inline double operator () (const Index index) const {
      return probabilityDensityOf(index);
    };
    //private:
    // void printMapTo(std::ostream & i) const {
    //   for (typename Map::const_iterator it = cache.begin();
    //    it!=cache.end();
    //    ++it) {
    // i <<  "prob=" << it->first << " index=" << it->second << std::endl;
    //  };
    //};
  };

};

#endif
