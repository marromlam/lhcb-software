// $Id: VeloRHit.h,v 1.9 2010-04-07 05:17:03 wouter Exp $
#ifndef INCLUDE_TF_VELORHIT_H
#define INCLUDE_TF_VELORHIT_H 1

#include "GaudiKernel/Range.h"
#include "TfKernel/VeloHit.h"

#include "VeloDet/DeVeloRType.h"

namespace LHCb {
  class VeloLiteCluster;
}

namespace Tf {

  /** @class VeloRHit VeloRHit.h
   *  Representation of VELO r measurement
   *
   * @authors S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   * @date   2007-06-12
   */

  class VeloRHit : public VeloHit {

  public:

    //----------------------------------------------------------------------
    // shortcut typedefs
    //----------------------------------------------------------------------

    typedef std::vector<VeloRHit* > container_type;
    typedef Gaudi::Range_<container_type> range_type;

    typedef velo_rhit_tag hit_type_tag; ///< the hit type tag

  public:

    /** Standard Constructor
     *  @param[in] sensor Pointer to DeVeloRType sensor for this hit
     *  @param[in] clus   Associated Velo Cluster
     *  @param[in] signal The cluster signal
     */
    inline VeloRHit(const DeVeloRType* sensor,
                    const LHCb::VeloLiteCluster& clus, double signal);

    //== simple accessors
    double r()                             const noexcept { return m_coord; }
    double rHalfBox()                      const noexcept { return m_coordHalfBox; }
    double rIdeal()                        const noexcept { return m_coordIdeal; }
    double z()                             const noexcept { return m_sensor->z(); }
    //== access to coords used for sorting and searching (for r the same as coords)
    double sortCoord()                     const noexcept { return m_coord; }
    double sortCoordHalfBox()              const noexcept { return m_coordHalfBox; }
    double sortCoordIdeal()                const noexcept { return m_coordIdeal; }
    /// Returns pointer to the associated sensor for this hit
    const DeVeloRType* sensor()            const noexcept { return m_sensor; }

  private:

    const DeVeloRType* m_sensor;        ///< link to detector element

  };


  //----------------------------------------------------------------------
  // shortcut typedefs
  //----------------------------------------------------------------------

  typedef VeloRHit::container_type VeloRHits;
  typedef VeloRHit::range_type VeloRHitRange;

  //----------------------------------------------------------------------
  // inline implementations
  //----------------------------------------------------------------------

  // constructor
  inline VeloRHit::VeloRHit(const DeVeloRType* s, 
                            const LHCb::VeloLiteCluster& c, 
                            double signal)
    : VeloHit(c
              , RegionID(c.channelID(),s)
              , s->globalR(c.channelID().strip(),c.interStripFraction())
              , s->halfboxR(c.channelID().strip(),c.interStripFraction())
              , s->rOfStrip(c.channelID().strip(),c.interStripFraction())
              , s->rPitch( c.channelID().strip() ) / std::sqrt(12.0)
              , signal
              )
      , m_sensor(s)
  { ; }
  
  // our dynamic casts
  inline const VeloRHit* HitBase::veloRHit() const noexcept
  {
    return type()==RegionID::VeloR ? static_cast<const VeloRHit*>(this) : nullptr;
  }

}

#endif // INCLUDE_TF_VELORHIT_H

