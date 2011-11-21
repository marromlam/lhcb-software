// $Id: $
#ifndef RICHDET_DERICHPMTPANEL_H
#define RICHDET_DERICHPMTPANEL_H 1

// Include files

// Gaudi
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

// DetDesc
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ISolid.h"

// RichDet
#include "RichDet/DeRichPDPanel.h"
#include "RichDet/DeRich.h"
#include "RichDet/DeRichPMT.h"
#include "RichDet/DeRichLocations.h"

/** @class DeRichPMTPanel DeRichPMTPanel.h RichDet/DeRichPMTPanel.h
 *
 *  Detector element for PMT panels
 *  
 *  @author Sajan Easo
 *  @date   2011-10-10
 */
class DeRichPMTPanel : public DeRichPDPanel 
{

public:

  /// Standard constructor
  DeRichPMTPanel(const std::string & name = ""  );

  virtual ~DeRichPMTPanel( ); ///< Destructor

  /**
   * Retrieves reference to class identifier
   * @return the class identifier for this class
   */
  const CLID& clID() const
  {
    return classID();
  }

  static const CLID& classID();
  /**
   * This is where most of the geometry is read and variables initialised
   *
   * @return Status of initialisation
   * @retval StatusCode::FAILURE Initialisation failed, program should
   * terminate
   */
  virtual StatusCode initialize();

  // Returns the detector element for the given PD number
  virtual const DeRichPD* dePD( const unsigned int PDNumber ) const;

  // Converts a Gaudi::XYZPoint in global coordinates to a RichSmartID.
  virtual StatusCode smartID( const Gaudi::XYZPoint& globalPoint,
                              LHCb::RichSmartID& id ) const;

  // Returns the intersection point with the detector plane given a vector and a point.
  virtual LHCb::RichTraceMode::RayTraceResult
  detPlanePoint( const Gaudi::XYZPoint& pGlobal,
                 const Gaudi::XYZVector& vGlobal,
                 Gaudi::XYZPoint& hitPosition,
                 LHCb::RichSmartID& smartID,
                 const LHCb::RichTraceMode mode ) const;

  // Returns the intersection point with an HPD window given a vector and a point.
  virtual LHCb::RichTraceMode::RayTraceResult
  PDWindowPoint( const Gaudi::XYZVector& vGlobal,
                 const Gaudi::XYZPoint& pGlobal,
                 Gaudi::XYZPoint& windowPointGlobal,
                 LHCb::RichSmartID& smartID,
                 const LHCb::RichTraceMode mode ) const;

  // Adds to the given vector all the available readout channels in this HPD panel
  virtual StatusCode readoutChannelList( LHCb::RichSmartID::Vector& readoutChannels ) const;

  /// Get tge sensitivevolumeID
  virtual int sensitiveVolumeID(const Gaudi::XYZPoint& globalPoint) const;

private:

  /// Returns the PD number for the given RichSmartID
  unsigned int pdNumber( const LHCb::RichSmartID smartID ) const;

  std::vector<int> getPmtRowColFromPmtNum( const int aPmtNum );

  int PmtModuleNumInPanelFromModuleNum(const int aMnum) const;
  int PmtModuleNumInPanelFromModuleNumAlone(const int aMnum) const;
  std::vector<int> PmtModuleRowColFromModuleNumInPanel(const int aMnum);

  // Set the rich panel and side
  StatusCode setRichPanelAndSide ( );

  /// Update cached information on geometry changes
  StatusCode geometryUpdate();
  StatusCode getPanelGeometryInfo();
  int getPmtModuleNumFromRowCol(int MRow, int MCol ) const;
  std::vector<int> findPMTArraySetup(const Gaudi::XYZPoint& inPanel) const;
  StatusCode setRichPmtSmartID(const std::vector<int>& aPmtHitChannel,
                               LHCb::RichSmartID& ID ) const;

  DetectorElement* getDeRich1() const;
  int getNumModulesInThisPanel() ;
  int getPmtNumFromRowCol(int PRow, int PCol) const;
  bool isInPmtAnodeLateralAcc(const Gaudi::XYZPoint& aPointInPmtAnode ) const;
  bool isInPmt(const Gaudi::XYZPoint& aPointInPmt ) const;
  bool isInPmtPanel(const Gaudi::XYZPoint& aPointInPanel ) const;
  StatusCode getPanelInterSection ( const Gaudi::XYZPoint& pGlobal,
                                    const Gaudi::XYZVector& vGlobal ,
                                    Gaudi::XYZPoint& panelIntersection,
                                    Gaudi::XYZPoint& panelIntersectionGlobal ) const;

private:

  std::vector<IDetectorElement*> m_DePMTModules; ///< Container for the PMT Modules
                                                 ///as Det Elements

  std::vector<std::vector<DeRichPMT*> > m_DePMTs; ///< Container for the PMTs

  /// Container for the PMTAnodes as Det Elements
  std::vector<std::vector<IDetectorElement*> > m_DePMTAnodes;

  std::vector<double> m_PmtModulePlaneHalfSizeR1;
  std::vector<double> m_PmtModulePlaneHalfSizeR2;
  double m_PmtModulePitch;
  double m_PmtPitch;
  std::vector<int> m_RichPmtNumModulesInRowCol;
  std::vector<int> m_RichPmtModuleCopyNumBeginPanel;
  std::vector<int> m_RichPmtModuleCopyNumEndPanel;
  std::vector<double> m_RichPmtModuleActiveAreaHalfSize;
  std::vector<int> m_NumPmtInRowCol;
  std::vector<int> m_NumPmtModuleInRich;
  int m_NumPmtInRichModule;
  int m_totNumPmtModuleInRich;
  double m_PmtAnodeXSize;
  double m_PmtAnodeYSize;
  double m_PmtPixelGap;
  int m_PmtPixelsInRow;
  int m_PmtPixelsInCol;
  double m_PmtAnodeXEdge;
  double m_PmtAnodeYEdge;
  double m_AnodeXPixelSize;
  double m_AnodeYPixelSize;
  double m_PmtAnodeEffectiveXPixelSize;
  double m_PmtAnodeEffectiveYPixelSize;
  double m_PmtMasterLateralSize;
  double m_RichPmtQuartzThickness;
  double  m_RichPmtQuartzLocalZInPmt;

  Gaudi::Plane3D m_localPlane;
  Gaudi::XYZVector m_localPlaneNormal;
  // Gaudi::Plane3D m_localPlane2;
  int  m_Rich1TotNumPmts;
  int m_Rich2TotNumPmts;
  Gaudi::Plane3D m_detectionPlane_exterior;

};

#endif // RICHDET_DERICHPMTPANEL_H
