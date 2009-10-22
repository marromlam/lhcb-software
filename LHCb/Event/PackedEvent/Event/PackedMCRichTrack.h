// $Id: PackedMCRichTrack.h,v 1.2 2009-10-22 09:21:53 jonrob Exp $
#ifndef EVENT_PackedMCRichTrack_H
#define EVENT_PackedMCRichTrack_H 1

#include <string>
#include <vector>

// Kernel
#include "Kernel/StandardPacker.h"

// Event
#include "Event/MCRichTrack.h"

// Gaudi
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

class GaudiAlgorithm;

namespace LHCb
{
  // -----------------------------------------------------------------------

  /** @struct PackedMCRichTrack Event/PackedMCRichTrack.h
   *
   *  Packed MCRichTrack
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  struct PackedMCRichTrack
  {
    /// Default constructor
    PackedMCRichTrack()
      : key(0), mcParticle(-1)
    {}

    int key;
    std::vector<int> mcSegments;
    int mcParticle;

  };

  // -----------------------------------------------------------------------

  static const CLID CLID_PackedMCRichTracks = 1524;

  /// Namespace for locations in TDS
  namespace PackedMCRichTrackLocation
  {
    static const std::string& Default = "pSim/Rich/Tracks";
  }

  /** @class PackedMCRichTracks Event/PackedMCRichTrack.h
   *
   *  Packed MCRichTracks
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class PackedMCRichTracks : public DataObject
  {

  public:

    /// Vector of packed objects
    typedef std::vector<LHCb::PackedMCRichTrack> Vector;

  public:

    /// Standard constructor
    PackedMCRichTracks( ) { }

    /// Destructor
    virtual ~PackedMCRichTracks( ) { }

    /// Class ID
    static const CLID& classID() { return CLID_PackedMCRichTracks; }

    /// Class ID
    virtual const CLID& clID() const { return PackedMCRichTracks::classID(); }

  public:

    /// Write access to the data vector
    Vector & data()             { return m_vect; }

    /// Read access to the data vector
    const Vector & data() const { return m_vect; }

  private:

    Vector m_vect; ///< The packed data objects

  };

  // -----------------------------------------------------------------------

  /** @class MCRichTrackPacker Event/PackedMCRichTrack.h
   *
   *  Utility class to handle the packing and unpacking of the MCRichTracks
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class MCRichTrackPacker
  {
  public:

    // These are required by the templated algorithms
    typedef LHCb::MCRichTrack                    Data;
    typedef LHCb::PackedMCRichTrack        PackedData;
    typedef LHCb::MCRichTracks             DataVector;
    typedef LHCb::PackedMCRichTracks PackedDataVector;
    static const std::string& packedLocation()   { return LHCb::PackedMCRichTrackLocation::Default; }
    static const std::string& unpackedLocation() { return LHCb::MCRichTrackLocation::Default; }

  public:

    /// Default Constructor
    MCRichTrackPacker() {}

  public:

    /// Pack MCRichTracks
    void pack( DataVector       & tracks,
               PackedDataVector & ptracks,
               const unsigned int version ) const;

    /// Unpack MCRichTracks
    void unpack( PackedDataVector & ptracks,
                 DataVector       & tracks ) const;

    /// Compare two MCRichHits to check the packing -> unpacking performance
    StatusCode check( const DataVector & dataA,
                      const DataVector & dataB,
                      GaudiAlgorithm & parent ) const;

  private:

    /// Standard packing of quantities into integers ...
    StandardPacker m_pack;

  };

  // -----------------------------------------------------------------------

}

#endif // EVENT_PackedMCRichTrack_H
