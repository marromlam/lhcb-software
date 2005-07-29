#include<math.h>

#include "STDet/STWafer.h"
#include "Kernel/ITChannelID.h"

/// Standard constructor.
STWafer::STWafer(double pitch, int firstStrip, unsigned int nWafer, 
                 unsigned int iStation, unsigned int iLayer, unsigned int iWafer,
		 double ul, double ur, double vd, double vu, double dz,
                 double deadWidth, double thickness): 
  m_Pitch(pitch),
  m_FirstStrip(firstStrip),
  m_station(iStation),
  m_layer(iLayer),
  m_wafer(iWafer),
  m_UL(ul),
  m_UR(ur),
  m_VU(vu),
  m_VD(vd),
  m_DZ(dz),
  m_deadWidth(deadWidth),
  m_waferThickness(thickness)
{
  m_NumStrips = (int)((m_UR - m_UL)/m_Pitch);

  // dead regions
  double waferHeight = (fabs(vu-vd)+2*m_deadWidth)/(double)nWafer;
  for (unsigned int jWafer = 1;jWafer<nWafer;jWafer++){
    double vDead = vd-m_deadWidth+(waferHeight*(double)jWafer);
    m_deadRegions.push_back(vDead);
  } // iWafer
}

/// Get number of strip at the point (u,v)
int STWafer::stripNum(const double u, const double v) const  
{
  int id = ITChannelID::nullStrip;
  if (u<m_UR && u>m_UL && v<m_VU && v>m_VD) {
    id = m_FirstStrip + (int)floor((u - m_UL)/m_Pitch);
  }
  return id;
}

/// Get number of nearest strip to the point (u,v)
int STWafer::nearestStripNum(const double u) const
{
  int id =ITChannelID::nullStrip; ;
  if (u<m_UR && u>m_UL) {
    id = m_FirstStrip + (int)floor((u - m_UL)/m_Pitch);
  }
  else if (u>=m_UR) {
    id = m_FirstStrip + m_NumStrips - 1;
  }
  else {
    id = m_FirstStrip;
  }
  return id;
}

/// Check if point (u,v) is inside of this Wafer
bool STWafer::isInside(const double u, const double v) const
{
 return (u<(m_UR+(0.5*m_Pitch)) && u>(m_UL-(0.5*m_Pitch)) 
         && v<m_VU && v>m_VD); 
}

/// Check if point (u,v) is inside of this Wafer with tolerance
bool STWafer::isInside(const double u, const double v, double tolerance) const
{
  return (u<(m_UR+tolerance) && u>(m_UL-tolerance) 
         && v<(m_VU+tolerance) && v>(m_VD-tolerance));
}

bool STWafer::isInsideFullDetail(const double u, const double v, 
                                 const double zLocal) const
{

  /// detailed check in local coordinate system
  bool isInside = this->isInside(u,v); 
  if ((fabs(zLocal) < 0.5*m_waferThickness)&&(isInside == true)){
    // detailed check
    std::vector<double>::const_iterator iterD = m_deadRegions.begin();
    while ((iterD != m_deadRegions.end())&&(isInside == true)){
      if (fabs(v-*iterD)<m_deadWidth){
        isInside = false;
      }
      ++iterD;
    } // iterD
  } 

  return isInside;
}

/// Check if (u,v) is in this Wafer and check if it is in a vertical dead zone given some tolerance
bool STWafer::isInVDeadZone(const double u, const double v, const double tolerance) const {

  double vmax = v + tolerance;
  double vmin = v - tolerance;
  
  // Check if point (u,v) is inside of this Wafer with tolerance
  bool isInsideWafer = this->isInside(u, v, tolerance);

  // To be in a dead zone, it also needs to be in a Wafer
  bool isInsideDeadZone = false;

  if(isInsideWafer == true){
    std::vector<double>::const_iterator iterD = m_deadRegions.begin();
    while((iterD != m_deadRegions.end()) && (isInsideDeadZone == false)){
      if((fabs(vmax - *iterD) < m_deadWidth) || (fabs(vmin - *iterD) < m_deadWidth)){
        isInsideDeadZone = true;
      }
      ++iterD;
    } // iterD
  }
  
  // return true if in a wafer and a dead zone
  return isInsideDeadZone;
}


/// Get u-coordinate of a strip
double STWafer::U(const int strip) const 
{

  double u = 0.;
  if (isStrip(strip)) {
    u =  m_UL + (double)(strip - m_FirstStrip)*m_Pitch;
  }
 
  return u;
}

std::ostream& STWafer::printOut( std::ostream& os ) const{

  os << " wafer: " << std::endl;
  os << "  station " << m_station << " layer " << m_layer << " wafer " << m_wafer <<std::endl;  
  os << "      pitch " << m_Pitch << " # strips " << m_NumStrips <<std::endl;
  os << "      uR  " << m_UR << " uL " <<m_UL <<std::endl;
  os << "      vD  " << m_VD << " uL " <<m_VU <<std::endl;
  os << "      dz   "<< m_DZ << std::endl;

  return os;
}


MsgStream& STWafer::printOut( MsgStream& os ) const{

  os << " wafer: " << endreq;
  os << " station " << m_station << " layer " << m_layer << " wafer " << m_wafer <<endreq;
  os << "      pitch " << m_Pitch << " # strips " << m_NumStrips <<endreq;
  os << "      uR  " << m_UR << " uL " <<m_UL << endreq;
  os << "      vD  " << m_VD << " uL " <<m_VU << endreq;
  os << "      dz   "<< m_DZ << endreq;

  return os;
}














