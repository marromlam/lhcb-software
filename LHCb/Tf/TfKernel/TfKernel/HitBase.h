
//-----------------------------------------------------------------------------
/** @file HitBase.h
 *
 *  Header file for track find hit base class Tf::HitBase
 *
 *  CVS Log :-
 *  $Id: HitBase.h,v 1.4 2007-08-16 14:43:19 jonrob Exp $
 *
 *  @author S. Hansmann-Menzemer, W. Houlsbergen, C. Jones, K. Rinnert
 *  @date   2007-05-30
 */
//-----------------------------------------------------------------------------

#ifndef TFKERNEL_HITBASE_H
#define TFKERNEL_HITBASE_H 1

// STL
#include <bitset>

// LHCbKernel
#include "Kernel/LHCbID.h"

// TfKernel
#include "TfKernel/RegionID.h"

namespace Tf
{

  // forward declarations
  class VeloRHit;
  class VeloPhiHit;
  class STHit;
  class OTHit;

  //-------------------------------------------------------------------
  // Hit type tags
  //-------------------------------------------------------------------
  struct hit_base_tag                          {};  ///< type tag for hit base
  struct line_hit_tag    : public hit_base_tag {};  ///< type tag for line hit
  struct ot_hit_tag      : public line_hit_tag {};  ///< type tag for OT hit
  struct st_hit_tag      : public line_hit_tag {};  ///< type tag for ST hit
  struct velo_hit_tag    : public hit_base_tag {};  ///< type tag for Velo hit
  struct velo_rhit_tag   : public velo_hit_tag {};  ///< type tag for Velo R hit
  struct velo_phihit_tag : public velo_hit_tag {};  ///< type tag for Velo Phi hit

  /** @class HitBase TfKernel/HitBase.h
   *  Base class for storage of hits in all tracking systems
   *  @author S. Hansmann-Menzemer, W. Houlsbergen, C. Jones, K. Rinnert
   *  @date   2007-05-30
   */
  class HitBase
  {

  public:

    /** @enum EStatus
     *
     *  Enumerated type for bits in status flag.
     *
     *  @author S. Hansmann-Menzemer, W. Houlsbergen, C. Jones, K. Rinnert
     *  @date   2007-05-30
     */
    enum EStatus
      {
        UsedByTsaSeeding=0, ///< Hit has been used by the Tsa Seeding algorithm
        UsedByPatForward,   ///< Hit has been used by the Pat Forward tracking algorithm
        UsedByPatSeeding,   ///< Hit has been used by the Pat Seeding algorithm
        UsedByVeloPhi,      ///< Hit has been used by the Velo Phi tracking algorithm XXX???XXX
        UsedByVeloRZ,       ///< Hit has been used by the Velo R-Z tracking algorithm
        UsedByUnknown,      ///< Hit has been used by an unkown algorithm
        Unusable,           ///< Hit is unusable (XXX???XXX On what basis, decided by whom?)
        NUMSTATUSBITS       ///< Number of status types - For internal use.
      };

    /// Nested type that describes 'use' status of this hit
    typedef std::bitset<NUMSTATUSBITS> StatusFlag ;

    /// The hit type tag (XXX???XXX Wouter, could you maybe expand a bit on what these tag structs are for and how they are used ?)
    typedef hit_base_tag hit_type_tag;

    //== Simple accessors to internal data members

    /** Access the coord value (XXX???XXX what is coord X ? Y ? Z ? )
     *  @return The value of coord for this hit */
    inline float        coord()         const { return m_coord ; }

    /** Access the variance value
     *  @return The value of the variance for this hit */
    inline float        variance()      const { return m_variance ; }

    /** Access the weight (1/variance) value
     *  @return The value of the weight for this hit */
    inline float        weight  ()      const { return 1.0 / variance();  }

    /** Access the LHCbID channel identifier
     *  @return The channel identifier for this hit */
    inline LHCb::LHCbID lhcbID  ()      const { return m_lhcbID; }

    /** Access the RegionID for this hit
     *  @return The channel identifier for this hit */
    inline RegionID     regionID()      const { return m_regionID; }

    /** Access the LHCbID channel identifier for this
     *  @return The channel identifier for this hit */
    inline StatusFlag   status  ()      const { return m_status; }

    /** @brief Test if this hit satisfies the given status type.
     *  E.g.
     *  @code
     *   // test if the hit was previously used by the Tsa Seeding algorithm
     *   bool wasUsedByTsaSeeding = hit->testStatus(HitBase::UsedByTsaSeeding);
     *  @endcode
     *  @param stat The flag to test
     *  @return The status of the test
     *  @retval TRUE  The hit has the given status flag set
     *  @retval FALSE The hit does not have the given status flag set
     */
    inline bool         testStatus( const EStatus stat) const { return m_status.test(stat); }

    /** Set the given status flag for this hit
     *  @param[in] stat The status type
     *  @param[in] val  The flag value (true or false) for the given status type
     */
    inline void         setStatus ( const EStatus stat,
                                    const bool val = true ) const { m_status.set(stat,val) ; }

    /** Test if the hit has been used by any algorithm
     *  @retval TRUE  The hit has been used by at least one algorithm
     *  @retval FALSE The hit has not been used by any algorithm yet
     */
    inline bool         isUsed    () const { return m_status.any() ; }

    /** Label this hit as having been used by an anonymous algorithm
     *  @param[in] flag Flag value to set (true=used, false=not used)
     */
    inline void         setUsed   ( const bool flag ) const { m_status.set(UsedByUnknown,flag) ; }

    /** Access the region number for the hit */
    inline unsigned int region  () const { return static_cast<unsigned int>(m_regionID.region()); }

    /** Access the layer number for the hit */
    inline unsigned int layer   () const { return static_cast<unsigned int>(m_regionID.layer()); }

    /** Access the station number for the hit */
    inline unsigned int station () const { return static_cast<unsigned int>(m_regionID.station()); }

    /** Access the region(detector) type for the hit */
    inline unsigned int type    () const { return static_cast<unsigned int>(m_regionID.type()); }

    /** Access the velo half for the hit (XXX???XXX Same as layer() - So why have it ?) */
    inline unsigned int veloHalf() const { return static_cast<unsigned int>(m_regionID.veloHalf()); }

    /** Access the zone number for the hit XXX???XXX what is a zone ? */
    inline unsigned int zone    () const { return static_cast<unsigned int>(m_regionID.zone()); }

    // dynamic casts

    /** Dynamic cast this base hit to a VeloRHit hit
     *  @return Pointer to a VeloRHit hit
     *  @retval NULL     Hit is NOT of type VeloRHit
     *  @retval NON-NULL Hit is of type VeloRHit
     */
    const VeloRHit*   veloRHit()   const;

    /** Dynamic cast this base hit to a VeloPhiHit hit
     *  @return Pointer to a VeloPhiHit hit
     *  @retval NULL     Hit is NOT of type VeloPhiHit
     *  @retval NON-NULL Hit is of type VeloPhiHit
     */
    const VeloPhiHit* veloPhiHit() const;

    /** Dynamic cast this base hit to a OTHit hit
     *  @return Pointer to a OTHit hit
     *  @retval NULL     Hit is NOT of type OTHit
     *  @retval NON-NULL Hit is of type OTHit
     */
    const OTHit*      othit()      const;

    /** Dynamic cast this base hit to a STHit hit
     *  @return Pointer to a STHit hit
     *  @retval NULL     Hit is NOT of type STHit
     *  @retval NON-NULL Hit is of type STHit
     */
    const STHit*      sthit()      const;

  protected:

    // only inherited objects are supposed to be created
    
    /** Default constructor */
    HitBase() {}

    /** Constructor from hit information
     *  @param id       The LHCbID channel ID for this hit
     *  @param regionid The RegionID for this hit
     *  @param coord    The coord value for this hit (XXX???XXX What is this ?)
     *  @param variance The value of the variance for this hit
     */
    HitBase( const LHCb::LHCbID id, 
             const RegionID& regionid, 
             const float coord    = 0, 
             const float variance = 0 )
      : m_lhcbID(id), m_regionID(regionid), m_coord(coord), m_variance(variance) {}

    /** Set the coord value
     *  @param x The coord value to use */
    void setCoord(const float x) { m_coord = x ; }

    /** Set the variance value
     *  @param x The variance value to use */
    void setVariance(const float v) { m_variance = v ; }

  protected:

    LHCb::LHCbID m_lhcbID;          ///< The hit LHCbID channel identifier
    RegionID m_regionID;            ///< The hit RegionID       
    float m_coord;                  ///< The hit coord (XXX???XXX X value ?)
    float m_variance;               ///< The hit variance
    mutable StatusFlag m_status;    ///< The hit status word (bit packed)

  };

}

#endif // TFKERNEL_HITBASE_H
