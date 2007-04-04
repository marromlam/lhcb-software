
/** @file DeRichSystem.h
 *
 *  Header file for detector description class : DeRichSystem
 *
 *  CVS Log :-
 *  $Id: DeRichSystem.h,v 1.8 2007-04-04 10:56:38 jonrob Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2006-01-26
 */

#ifndef RICHDET_DERICHSYSTEM_H
#define RICHDET_DERICHSYSTEM_H 1

// DetDesc
#include "DetDesc/DetectorElement.h"

// LHCbKernel
#include "Kernel/RichSide.h"
#include "Kernel/RichDetectorType.h"
#include "Kernel/RichSmartID.h"

// boost
#include "boost/array.hpp"

//local
#include "RichDet/RichDAQDefinitions.h"
#include "RichDet/DeRichLocations.h"

// External declarations
extern const CLID CLID_DERichSystem;

/** @class DeRichSystem DeRichSystem.h
 *
 * Class for generic info about the Rich system. In particular
 * to provide conversions and mappings between the various
 * RICH numbering schemes and to provide data on which HPDs, Level0 and Level1
 * boards are currently active.
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date   27/01/2006
 *
 * @todo Remove the hardcoded numbers in the copyNumber function
 */
class DeRichSystem: public DetectorElement
{

public:

  /// Constructor for this class
  DeRichSystem();

  /// Default destructor
  virtual ~DeRichSystem();

  /** Retrieves reference to class identifier
   *  @return the class identifier for this class
   */
  const CLID& clID() const { return classID(); }

  /** Retrieves reference to class identifier
   *  @return the class identifier for this class
   */
  static const CLID& classID();

  /** This is where most of the geometry is read and variables initialised
   *  @return Status of initialisation
   *  @retval StatusCode::FAILURE Initialisation failed, program should
   *  terminate
   */
  virtual StatusCode initialize();

public:

  /** Convert an HPD RichSmartID into the corresponding HPD hardware number
   *  @param smartID The RichSmartID for the HPD
   *  @return The corresponding HPD hardware ID
   */
  const Rich::DAQ::HPDHardwareID hardwareID( const LHCb::RichSmartID smartID ) const;

  /** Convert a RICH HPD hardware number into the corresponding HPD RichSmartID
   *  @param hID The hardware ID for the HPD
   *  @return The corresponding HPD RichSmartID
   */
  const LHCb::RichSmartID richSmartID( const Rich::DAQ::HPDHardwareID hID ) const;

  /** Convert a RICH Level0 hardware number into the corresponding HPD RichSmartID
   *  @param l0ID The HPD level0 for the HPD
   *  @return The corresponding HPD RichSmartID
   */
  const LHCb::RichSmartID richSmartID( const Rich::DAQ::Level0ID l0ID ) const;

  /// Returns a list of all active HPDs identified by their RichSmartID
  const LHCb::RichSmartID::Vector & activeHPDRichSmartIDs() const;

  /// Returns a list of all active HPDs identified by their hardware IDs
  const Rich::DAQ::HPDHardwareIDs & activeHPDHardwareIDs() const;

  /** Ask whether a given HPD is currently active or dead
   *  @param id The RichSmartID for the HPD
   *  @return boolean indicating if the given HPD is active or not
   *  @attention For speed, this method does NOT check if the given HPD id is a valid one
   *             or not. Invalid HPD ids will return true
   */
  bool hpdIsActive( const LHCb::RichSmartID id ) const;

  /** Ask whether a given HPD is currently active or dead
   *  @param id The hardware id for the HPD
   *  @return boolean indicating if the given HPD is active or not
   *  @attention For speed, this method does NOT check if the given HPD id is a valid one
   *             or not. Invalid HPD ids will return true
   */
  bool hpdIsActive( const Rich::DAQ::HPDHardwareID id ) const;

  /** Obtain the Level0 ID number for a given HPD RichSmartID
   *  @param smartID The RichSmartID for the HPD
   *  @return The corresponding HPD Level0 ID
   */
  const Rich::DAQ::Level0ID level0ID( const LHCb::RichSmartID smartID ) const;

  /** Obtain the Level0 ID number for a given HPD hardware ID
   *  @param hardID The hardware ID for the HPD
   *  @return The corresponding HPD Level0 ID
   */
  const Rich::DAQ::Level0ID level0ID( const Rich::DAQ::HPDHardwareID hardID ) const;

  /** Obtain the Level1 ID number for a given HPD RichSmartID
   *  @param smartID The RichSmartID for the HPD
   *  @return The corresponding HPD Level1 ID
   */
  const Rich::DAQ::Level1ID level1ID( const LHCb::RichSmartID smartID ) const;

  /** Obtain the Level1 ID number for a given HPD hardware ID
   *  @param hardID The hardware ID for the HPD
   *  @return The corresponding HPD Level1 ID
   */
  const Rich::DAQ::Level1ID level1ID( const Rich::DAQ::HPDHardwareID hardID ) const;

  /** Obtain the Level1 input number for a given HPD RichSmartID
   *  @param smartID The RichSmartID for the HPD
   *  @return The corrresponding Level1 input number
   */
  const Rich::DAQ::Level1Input level1InputNum( const LHCb::RichSmartID smartID ) const;

  /** Obtain the Level1 input number for a given HPD hardware ID
   *  @param hardID The hardware ID for the HPD
   *  @return The corrresponding Level1 input number
   */
  const Rich::DAQ::Level1Input level1InputNum( const Rich::DAQ::HPDHardwareID hardID ) const;

  /** Obtain a list of RichSmartID HPD identifiers for a given level1 ID
   *  @param l1ID The level1 ID number
   *  @return Vector of all HPD RichSmartIDs for that Level1 board
   */
  const LHCb::RichSmartID::Vector & l1HPDSmartIDs( const Rich::DAQ::Level1ID l1ID ) const;

  /** Obtain a list of HPD hardware identifiers for a given level1 ID
   *  @param l1ID The level1 ID number
   *  @return Vector of all HPD hardware IDs for that Level1 board
   */
  const Rich::DAQ::HPDHardwareIDs & l1HPDHardIDs( const Rich::DAQ::Level1ID l1ID ) const;

  /** Return which RICH detector a given Level1 ID is for
   *  @param l1ID The Level 1 ID
   *  @return The RICH detector
   */
  const Rich::DetectorType richDetector( const Rich::DAQ::Level1ID l1ID ) const;

  /// Direct access to the mapping between Level1 IDs and HPD RichSmartIDs
  const Rich::DAQ::L1ToSmartIDs & l1HPDSmartIDs() const;

  /// Direct access to the mapping between Level1 IDs and HPD RichSmartIDs
  const Rich::DAQ::L1ToHardIDs & l1HPDHardIDs() const;

  /// Returns a list of all valid Level1 board IDs
  const Rich::DAQ::Level1IDs & level1IDs() const;

  /// Return the copy number for a given smartID
  inline unsigned int copyNumber( const LHCb::RichSmartID smartID ) const
  {
    return( smartID.rich() == Rich::Rich1 ?
            smartID.panel()*98 + smartID.hpdCol()*14 + smartID.hpdNumInCol() :
            m_rich1NumberHpds + smartID.panel()*144 + smartID.hpdCol()*16 + smartID.hpdNumInCol() );
  }

  /**
   * Retrieves the location of the HPD in the detector store, so it can be
   * loaded using the getDet<DeRichHPD> method.
   * @return The location of the HPD in the detector store
   */
  std::string getDeHPDLocation(LHCb::RichSmartID smartID) const;


private: // methods

  /// Update methods for HPD mappings
  StatusCode buildHPDMappings();

  /// Fill the maps for the given RICH detector
  StatusCode fillMaps( const Rich::DetectorType rich );

private: // data

  /// Type for mapping from RichSmartID to Rich::DAQ::HPDHardwareID
  typedef GaudiUtils::HashMap< const LHCb::RichSmartID, Rich::DAQ::HPDHardwareID > SoftToHard;
  SoftToHard m_soft2hard; ///< Software ID to hardware ID map

  /// Type for mapping from Rich::DAQ::HPDHardwareID to RichSmartID
  typedef GaudiUtils::HashMap< const Rich::DAQ::HPDHardwareID, LHCb::RichSmartID > HardToSoft;
  HardToSoft m_hard2soft; ///< HPD Hardware ID to software ID map

  /// Type for mapping from Rich::DAQ::HPDHardwareID to RichSmartID
  typedef GaudiUtils::HashMap< const Rich::DAQ::Level0ID, LHCb::RichSmartID > L0HardToSoft;
  L0HardToSoft m_l0hard2soft; ///< Level0 Hardware ID to software ID map

  /// List of all active HPD RichSmartIDs
  LHCb::RichSmartID::Vector m_smartIDs;

  /// List of all inactive HPD RichSmartIDs
  LHCb::RichSmartID::Vector m_inactiveSmartIDs;

  /// List of all active HPD hardware IDs
  Rich::DAQ::HPDHardwareIDs m_hardIDs;

  /// List of all inactive HPD hardware IDs
  Rich::DAQ::HPDHardwareIDs m_inactiveHardIDs;

  /// Typedef for mapping from RichSmartID to Level0 ID
  typedef GaudiUtils::HashMap< const LHCb::RichSmartID, Rich::DAQ::Level0ID > SmartIDToL0;
  SmartIDToL0 m_smartid2L0; ///< HPD RichSmartID to L0 ID map

  /// Typedef for mapping from HPD Hardware ID to Level0 ID
  typedef GaudiUtils::HashMap< const Rich::DAQ::HPDHardwareID, Rich::DAQ::Level0ID > HardIDToL0;
  HardIDToL0 m_hardid2L0; ///< HPD Hardware ID to L0 ID map

  /// Typedef for mapping from RichSmartID to Level1 ID
  typedef GaudiUtils::HashMap< const LHCb::RichSmartID, Rich::DAQ::Level1ID > SmartIDToL1;
  SmartIDToL1 m_smartid2L1; ///< HPD RichSmartID to L1 ID map

  /// Typedef for mapping from HPD Hardware ID to Level1 ID
  typedef GaudiUtils::HashMap< const Rich::DAQ::HPDHardwareID, Rich::DAQ::Level1ID > HardIDToL1;
  HardIDToL1 m_hardid2L1; ///< HPD Hardware ID to L1 ID map

  /// Typedef for mapping from RichSmartID to Level1 input number
  typedef GaudiUtils::HashMap< const LHCb::RichSmartID, Rich::DAQ::Level1Input > SmartIDToL1In;
  SmartIDToL1In m_smartid2L1In; ///< HPD RichSmartID to L1 input number map

  /// Typedef for mapping from HPD Hardware ID to Level1 input number
  typedef GaudiUtils::HashMap< const Rich::DAQ::HPDHardwareID, Rich::DAQ::Level1Input > HardIDToL1In;
  HardIDToL1In m_hardid2L1In; ///< HPD Hardware ID to L1 input number map

  /// Typedef for mapping between L1 boards and RICH detector
  typedef GaudiUtils::HashMap< const Rich::DAQ::Level1ID, Rich::DetectorType > L1ToRICH;
  L1ToRICH m_l1ToRich; ///< L1 to RICH map

  Rich::DAQ::L1ToSmartIDs m_l12smartids; ///< L1 ID to RichSmartIDs map
  Rich::DAQ::L1ToHardIDs  m_l12hardids;  ///< L1 ID to HPD hardware IDs map

  /// List of all valid Level1 IDs
  Rich::DAQ::Level1IDs m_l1IDs;

  /// Location of RICH Numbering schemes in Conditions DB
  boost::array<std::string, Rich::NRiches> m_condBDLocs;

  /// The total number of HPDs in Rich1.
  int m_rich1NumberHpds;

};

//=========================================================================
// activeHPDRichSmartIDs
//=========================================================================
inline const LHCb::RichSmartID::Vector& DeRichSystem::activeHPDRichSmartIDs() const
{
  return m_smartIDs;
}

//=========================================================================
// activeHPDHardwareIDs
//=========================================================================
inline const Rich::DAQ::HPDHardwareIDs& DeRichSystem::activeHPDHardwareIDs() const
{
  return m_hardIDs;
}

//=========================================================================
// hpdIsActive
//=========================================================================
inline bool DeRichSystem::hpdIsActive( const LHCb::RichSmartID id ) const
{
  // is this id in the inactive list
  return ( m_inactiveSmartIDs.empty() ||
           std::find( m_inactiveSmartIDs.begin(),
                      m_inactiveSmartIDs.end(),
                      id.hpdID() ) == m_inactiveSmartIDs.end() );
}

//=========================================================================
// hpdIsActive
//=========================================================================
inline bool DeRichSystem::hpdIsActive( const Rich::DAQ::HPDHardwareID id ) const
{
  // is this id in the inactive list
  return ( m_inactiveHardIDs.empty() ||
           std::find( m_inactiveHardIDs.begin(),
                      m_inactiveHardIDs.end(),
                      id ) == m_inactiveHardIDs.end() );
}

//=========================================================================
// l1HPDSmartIDs
// Access mapping between Level 1 IDs and HPD RichSmartIDs
//=========================================================================
inline const Rich::DAQ::L1ToSmartIDs& DeRichSystem::l1HPDSmartIDs() const
{
  return m_l12smartids;
}

//=========================================================================
// l1HPDHardIDs
// Access mapping between Level 1 IDs and HPD RichSmartIDs
//=========================================================================
inline const Rich::DAQ::L1ToHardIDs& DeRichSystem::l1HPDHardIDs() const
{
  return m_l12hardids;
}

//=========================================================================
// level1IDs
//=========================================================================
inline const Rich::DAQ::Level1IDs& DeRichSystem::level1IDs() const
{
  return m_l1IDs;
}

#endif    // RICHDET_DERICHSYSTEM_H
