// $Id: PackedMCRichDigitSummary.h,v 1.2 2009-10-22 09:21:53 jonrob Exp $
#ifndef EVENT_PackedMCRichDigitSummary_H
#define EVENT_PackedMCRichDigitSummary_H 1

#include <string>

// Kernel
#include "Kernel/StandardPacker.h"

// Event
#include "Event/MCRichDigitSummary.h"

// Gaudi
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/StatusCode.h"

class GaudiAlgorithm;

namespace LHCb
{
  // -----------------------------------------------------------------------

  /** @struct PackedMCRichDigitSummary Event/PackedMCRichDigitSummary.h
   *
   *  Packed MCRichHit
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  struct PackedMCRichDigitSummary
  {
    /// Default constructor
    PackedMCRichDigitSummary()
      : history(0),
        richSmartID(0),
        mcParticle(-1)
    {}

    int   history;
    int   richSmartID;
    int   mcParticle;

  };

  // -----------------------------------------------------------------------

  static const CLID CLID_PackedMCRichDigitSummarys = 1527;

  /// Namespace for locations in TDS
  namespace PackedMCRichDigitSummaryLocation
  {
    static const std::string& Default = "pSim/Rich/DigitSummaries";
  }

  /** @class PackedMCRichDigitSummarys Event/PackedMCRichDigitSummary.h
   *
   *  Packed MCRichDigitSummarys
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class PackedMCRichDigitSummarys : public DataObject
  {

  public:

    /// Vector of packed objects
    typedef std::vector<LHCb::PackedMCRichDigitSummary> Vector;

  public:

    /// Standard constructor
    PackedMCRichDigitSummarys( ) { }

    /// Destructor
    virtual ~PackedMCRichDigitSummarys( ) { }

    /// Class ID
    static const CLID& classID() { return CLID_PackedMCRichDigitSummarys; }

    /// Class ID
    virtual const CLID& clID() const { return PackedMCRichDigitSummarys::classID(); }

  public:

    /// Write access to the data vector
    Vector & data()             { return m_vect; }

    /// Read access to the data vector
    const Vector & data() const { return m_vect; }

  private:

    Vector m_vect; ///< The packed data objects

  };

  // -----------------------------------------------------------------------

  /** @class MCRichDigitSummaryPacker Event/PackedMCRichDigitSummary.h
   *
   *  Utility class to handle the packing and unpacking of the MCRichDigitSummarys
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class MCRichDigitSummaryPacker
  {
  public:

    // These are required by the templated algorithms
    typedef LHCb::MCRichDigitSummary                    Data;
    typedef LHCb::PackedMCRichDigitSummary        PackedData;
    typedef LHCb::MCRichDigitSummarys             DataVector;
    typedef LHCb::PackedMCRichDigitSummarys PackedDataVector;
    static const std::string& packedLocation()   { return LHCb::PackedMCRichDigitSummaryLocation::Default; }
    static const std::string& unpackedLocation() { return LHCb::MCRichDigitSummaryLocation::Default; }

  public:

    /// Default Constructor
    MCRichDigitSummaryPacker() {}

  public:

    /// Pack MCRichDigitSummarys
    void pack( DataVector       & hits,
               PackedDataVector & phits,
               const unsigned int version ) const;

    /// Unpack MCRichDigitSummarys
    void unpack( PackedDataVector & phits,
                 DataVector       & hits ) const;

    /// Compare two MCRichDigitSummarys to check the packing -> unpacking performance
    StatusCode check( const DataVector & dataA,
                      const DataVector & dataB,
                      GaudiAlgorithm & parent ) const;

  private:

    /// Standard packing of quantities into integers ...
    StandardPacker m_pack;

  };

  // -----------------------------------------------------------------------

}

#endif // EVENT_PackedMCRichDigitSummary_H
