// $Id: DeRichLocations.h,v 1.4 2008-06-24 09:12:35 jonrob Exp $
#ifndef RICHDET_DERICHHPDPANELLOCATIONS_H
#define RICHDET_DERICHHPDPANELLOCATIONS_H 1

// ************************************************************************************************************

/** @namespace DeRichLocations
 *
 *  Namespace for the location of Rich Detector Elelements in xml
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 *
 */
namespace DeRichLocations
{

  /// Rich1 location in transient detector store
  static const std::string& Rich1 = "/dd/Structure/LHCb/BeforeMagnetRegion/Rich1";
  /// Rich2 location in transient detector store
  static const std::string& Rich2 = "/dd/Structure/LHCb/AfterMagnetRegion/Rich2";

  /// RichSystem location in transient detector store
  static const std::string& RichSystem = "/dd/Structure/LHCb/AfterMagnetRegion/Rich2/RichSystem";

  /// Location of Rich1 top panel
  static const std::string& Rich1TopPanel =
  "/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PDPanel0";
  static const std::string& Rich1Panel0=Rich1TopPanel;

  /// Location of Rich1 bottom panel
  static const std::string& Rich1BottomPanel =
  "/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PDPanel1";
  static const std::string& Rich1Panel1=Rich1BottomPanel;

  /// Location of Rich2 left panel
  static const std::string& Rich2LeftPanel =
  "/dd/Structure/LHCb/AfterMagnetRegion/Rich2/RichSystem/HPDPanel0";
  static const std::string& Rich2Panel0=Rich2LeftPanel;

  /// Location of Rich2 right panel
  static const std::string& Rich2RightPanel =
  "/dd/Structure/LHCb/AfterMagnetRegion/Rich2/RichSystem/HPDPanel1";
  static const std::string& Rich2Panel1=Rich2RightPanel;

  /// Multi solid aerogel location
  static const std::string& Aerogel = "/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/Aerogel";

  /// Rich1 gas (C4F10) location
  static const std::string& Rich1Gas = "/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/Rich1Gas";
  static const std::string& C4F10    = Rich1Gas;

  /// Rich2 gas (CF4) location
  static const std::string& Rich2Gas  = "/dd/Structure/LHCb/AfterMagnetRegion/Rich2/Rich2Gas";
  static const std::string& CF4       = Rich2Gas;

  /// Rich1 Beampipe location in TDS
  static const std::string& Rich1BeamPipe = "/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/Rich1BeamPipe";
  /// Rich2 Beampipe location in TDS
  static const std::string& Rich2BeamPipe = "/dd/Structure/LHCb/AfterMagnetRegion/Rich2/Rich2BeamPipe";

}

// ************************************************************************************************************

#endif // RICHDET_DERICHHPDPANELLOCATIONS_H
