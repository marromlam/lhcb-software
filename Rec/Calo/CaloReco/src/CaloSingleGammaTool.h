#ifndef CALOSINGLEGAMMATOOL_H
#define CALOSINGLEGAMMATOOL_H 1

// Include files
// Kernel
#include "Kernel/Point3DTypes.h"
#include "Kernel/Vector3DTypes.h"
#include "LHCbMath/Line.h"
#include "LHCbMath/GeomFun.h"
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
// Event
#include "Event/CaloHypotheses.h" 
// CaloInterfaces
#include "CaloInterfaces/ICaloHypoLikelihood.h"
// from CaloEvent/Event
#include "Event/CaloDigit.h"
// from CaloUtils
#include "CaloUtils/ClusterFunctors.h"

// Forward declarations
class DeCalorimeter;
class LHCb::CaloHypo;
class IDataProviderSvc;


class CaloSingleGammaTool :  public  GaudiTool ,
                             virtual public ICaloHypoLikelihood {
public:

  CaloSingleGammaTool
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent );
  
  virtual ~CaloSingleGammaTool();

  virtual StatusCode initialize ();
  virtual StatusCode finalize   ();
  virtual const LHCb::CaloHypotheses::Hypothesis& hypothesis () const ; 
  virtual double operator() (const LHCb::CaloHypo* hypo ) const ;
  virtual double likelihood (const LHCb::CaloHypo* hypo ) const ;
  

protected:

 private:
  DeCalorimeter *m_detEcal;
  DeCalorimeter *m_detPrs;
  DeCalorimeter *m_detSpd;


  Gaudi::Plane3D m_planeEcal  ;
  Gaudi::Plane3D m_planeSpd   ;
  Gaudi::Plane3D m_planePrs   ;
  
  std::string m_nameOfECAL;
  std::string m_nameOfSPD;
  std::string m_nameOfPRS;
  
  LHCb::CaloHypotheses::Hypothesis m_hypothesis;
  Gaudi::XYZPoint           m_vertex    ;
  bool m_extrapolation;
  double m_shiftSpd;
};

#endif // CALOSINGLEGAMMATOOL_H




