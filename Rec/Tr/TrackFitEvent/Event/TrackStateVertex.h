
#ifndef TrackFitEvent_TrackStateVertex_H
#define TrackFitEvent_TrackStateVertex_H 1

// Include files
#include "Event/State.h"

namespace LHCb 
{

  // Forward declarations
  class TwoProngVertex ;
  namespace TrackVertexHelpers {
    class VertexTrack ;
  } ;

  /** @class TrackStateVertex TrackStateVertex.h
   *
   * Vertex(ing) class for multitrack vertices
   *
   * @author Wouter Hulsbergen
   * created Wed Dec 12 14:58:51 2007
   * 
   */

  class TrackStateVertex
  {
  public:
    /// Sets minimal data content for useable vertex. The rest we do with setters.
    TrackStateVertex() ;
    
    ///
    ~TrackStateVertex() ;
    
    ///
    void addTrack( const LHCb::State& state ) ;
    
    /// fit a single iteration. returns the delta-chisquare.
    double fitOneStep() ;

    /// fit until converged
    bool fit( double maxdchisq=0.01, size_t maxiterations=10) ;

    // calculate the chisquare of the vertex fit
    double chi2() const ;

    /// number of dofs in vertex fit
    int nDoF() const { return nTracks()*2 - 3 ; }

    size_t nTracks() const { return m_tracks.size() ; }

    /// Fitted state vector of track i
    Gaudi::TrackVector stateVector(size_t i) const ;
    /// Fitted covariance of track i
    Gaudi::TrackSymMatrix stateCovariance(size_t i) const ;
    /// Correlation matrix between state i and j
    Gaudi::Matrix5x5 stateCovariance(size_t i, size_t j) const ;
    /// Fitted state for track i
    LHCb::State state(size_t i) const ;
    /// Input state for track i
    const LHCb::State& inputState(size_t i) const ;
    /// Let's get rid of this one.
    LHCb::TwoProngVertex* createTwoProngVertex(bool computemomcov) const ;
    /// Position of the vertex
    Gaudi::XYZPoint position() const { return Gaudi::XYZPoint(m_pos(0),m_pos(1),m_pos(2)) ; }

  private:
    typedef TrackVertexHelpers::VertexTrack VertexTrack ;
    typedef std::vector< TrackVertexHelpers::VertexTrack* > VertexTrackContainer ;
    typedef ROOT::Math::SVector<double,3> MomentumParameters ;
    typedef ROOT::Math::SVector<double,3> PositionParameters ;
    typedef Gaudi::SymMatrix3x3           PositionCovariance ;
    typedef Gaudi::SymMatrix3x3           MomentumCovariance ;
    VertexTrackContainer m_tracks ;
    PositionParameters m_pos ;
    PositionCovariance m_poscov ;
    bool m_isFitted ;
  } ;
  
} // namespace LHCb;

#endif ///RecEvent_TrackVertex_H
