// $Id: HltMuonTrack.h,v 1.2 2010-02-12 07:49:12 graven Exp $
#ifndef HLTMUONTRACK_H 
#define HLTMUONTRACK_H 1

// Include files
#include "HltMuonPoint.h"

/** @class HltMuonTrack HltMuonTrack.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2004-10-08
 */
class HltMuonTrack {
public: 
  /// Standard constructor
  HltMuonTrack( ){m_clone=0;}; 

  virtual ~HltMuonTrack( ){}; ///< Destructor
  void setPoint(unsigned int station, HltMuonPoint point)
  {m_points[station]=point;  
  };
  
  HltMuonPoint  point(unsigned int station)
  {return m_points[station];  
  };
  
  void setClone(){m_clone=1;};
  bool clone(){if(m_clone>0)return true;
	else return false;};

double slopeX(int stationFirst,int stationSecond, 
	double zFirst, double zSecond){
return 
(m_points[stationFirst].x()-m_points[stationSecond].x())/(zFirst-zSecond);
};
double slopeY(int stationFirst,int stationSecond, 
        double zFirst, double zSecond){
return 
(m_points[stationFirst].y()-m_points[stationSecond].y())/(zFirst-zSecond);
};


protected:

private:
  HltMuonPoint m_points[5];
  unsigned int m_clone;  
};
#endif // HLTMUONTRACK_H
