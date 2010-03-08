// $Id: CaloHypo2Calo.h,v 1.4 2010-03-08 01:58:39 odescham Exp $
#ifndef CALOHYPO2CALO_H 
#define CALOHYPO2CALO_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "CaloInterfaces/ICaloHypo2Calo.h"            // Interface
#include "Calo2Calo.h"            
#include "CaloUtils/CellNeighbour.h"

/** @class CaloHypo2Calo CaloHypo2Calo.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2008-09-11
 */
class CaloHypo2Calo : public Calo2Calo, virtual public ICaloHypo2Calo {
public: 
  /// Standard constructor
  CaloHypo2Calo( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);

  virtual ~CaloHypo2Calo( ); ///< Destructor

  StatusCode initialize();
  // cellIDs
  const std::vector<LHCb::CaloCellID>& cellIDs(LHCb::CaloHypo fromHypo, std::string toCalo);
  const std::vector<LHCb::CaloCellID>& cellIDs(LHCb::CaloCluster fromCluster, std::string toCalo);
  const std::vector<LHCb::CaloCellID>& cellIDs(){return m_cells;};
  // digits
  const std::vector<LHCb::CaloDigit*>& digits(LHCb::CaloHypo  fromHypo, std::string toCalo);
  const std::vector<LHCb::CaloDigit*>& digits(LHCb::CaloCluster  fromCluster, std::string toCalo);
  const std::vector<LHCb::CaloDigit*>& digits(){return m_digits;};
  // energy
  double energy(LHCb::CaloHypo  fromCluster, std::string toCalo);
  double energy(LHCb::CaloCluster  fromCluster, std::string toCalo);
  double energy(){return m_energy;};
  // multiplicity
  int multiplicity(LHCb::CaloHypo   fromHypo     , std::string toCalo);
  int multiplicity(LHCb::CaloCluster  fromCluster, std::string toCalo);
  int multiplicity(){return m_count;};
  void setCalos(std::string from, std::string to){Calo2Calo::setCalos(from,to); };
  // external setting
  StatusCode  _setProperty(const std::string& p,const std::string& v){return  setProperty(p,v);};
  

protected:

private:
  bool m_seed;
  bool m_neighb;
  bool m_line;
  bool m_whole;
  int m_status;  
  double m_x;
  double m_y;
  CellNeighbour m_neighbour;
  LHCb::CaloCellID m_lineID ;
  Gaudi::XYZPoint  m_point;
};
#endif // CALOHYPO2CALO_H
