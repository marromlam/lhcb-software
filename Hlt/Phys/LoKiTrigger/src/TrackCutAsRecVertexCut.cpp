// ============================================================================
// Include files
// ============================================================================
#include <boost/foreach.hpp>
// ============================================================================
// Event 
// ============================================================================
#include "Event/RecVertex.h"
// ============================================================================
// Local
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
#include "LoKi/AuxFunBase.h"
#include "LoKi/Constants.h"
#include "LTTools.h"
#include "LoKi/TrackCutAsRecVertexCut.h"
// ============================================================================
/* @file
 *
 * Implementation file for the classes Hlt_TrackCutAsRecVertexCut_Any and
 * Hlt_TrackCutAsRecVertexCut_All
 *
 * @date 2012-02-27
 * @author Pieter David pieter.david@cern.ch
 */
// ============================================================================
LoKi::RecVertices::Hlt_TrackCutAsRecVertexCut_Any::Hlt_TrackCutAsRecVertexCut_Any
( const LoKi::BasicFunctors<const LHCb::Track*>::Predicate& cut ,
  const bool useExtraInfo                                       )
  : LoKi::BasicFunctors<const LHCb::VertexBase*>::Predicate()
  , m_cut          ( cut )
  , m_useExtraInfo ( useExtraInfo )
{ 
  if ( useExtraInfo ) { retrieveFailKey() ; } 
}
// ============================================================================
void LoKi::RecVertices::Hlt_TrackCutAsRecVertexCut_Any::retrieveFailKey()
{
  // from LoKi::Hlt1::HelperTool
  // get GaudiAlgorithm
  GaudiAlgorithm*  alg = LoKi::Hlt1::Utils::getGaudiAlg ( *this ) ;
  Assert ( 0 != alg    , "GaudiAlgorithm points to NULL!" ) ;
  // get IAlgorithm
  IAlgorithm*     ialg = LoKi::Hlt1::Utils::getAlg      ( *this ) ;
  Assert ( alg == ialg , "GaudiAlgorithm is not *my* IAlgorithm" ) ;
  // get the InfoID from HltANNSvc
  SmartIF<IANNSvc> ann = LoKi::Hlt1::Utils::annSvc( *this ) ;
  const std::string infoIDName = "HltUnit/" + alg->name() ;
  boost::optional<IANNSvc::minor_value_type> _info = ann->value("InfoID" , infoIDName );
  Assert( _info , " request for unknown Info ID : " + infoIDName );
  m_extraInfoKey = _info->second;

}
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::RecVertices::Hlt_TrackCutAsRecVertexCut_Any::result_type
LoKi::RecVertices::Hlt_TrackCutAsRecVertexCut_Any::operator()
  ( LoKi::RecVertices::Hlt_TrackCutAsRecVertexCut_Any::argument v ) const
{
  result_type ret = false;

  //
  const LHCb::VertexBase* vb = v ;
  const LHCb::RecVertex*  rv = dynamic_cast<const LHCb::RecVertex*> ( vb ) ;
  //
  if ( 0 == rv ) 
  {
    Error ("LHCb::VertexBase* is not LHCb::RecVertex*, return PositiveInfinity");
    return LoKi::Constants::PositiveInfinity ;
  }
  //
  BOOST_FOREACH( SmartRef<LHCb::Track> tr, rv->tracks() )
  {
    if ( ! m_useExtraInfo ) {
      ret = ret || ( m_cut( tr ) );
    } else {
      if ( ! tr->hasInfo( m_extraInfoKey ) ) {
        result_type pass = m_cut( tr );
        ret = ret || pass;
        if ( ! pass ) {
          tr->addInfo( m_extraInfoKey, 1. );
        }
      }
    }
  }
  return ret;
}

// ============================================================================
LoKi::RecVertices::Hlt_TrackCutAsRecVertexCut_All::Hlt_TrackCutAsRecVertexCut_All
( const LoKi::BasicFunctors<const LHCb::Track*>::Predicate& cut )
  : m_cut( cut )
{}
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::RecVertices::Hlt_TrackCutAsRecVertexCut_All::result_type
LoKi::RecVertices::Hlt_TrackCutAsRecVertexCut_All::operator()
  ( LoKi::RecVertices::Hlt_TrackCutAsRecVertexCut_All::argument v ) const
{
  result_type ret = true;
  const LHCb::VertexBase* vb = v ;
  const LHCb::RecVertex*  rv = dynamic_cast<const LHCb::RecVertex*> ( vb ) ;
  if( 0 == rv )
  {
    Error ("LHCb::VertexBase* is not LHCb::RecVertex*, return PositiveInfinity");
    return LoKi::Constants::PositiveInfinity ;
  }
  SmartRefVector<LHCb::Track>::const_iterator it = rv->tracks().begin();
  while ( ret && ( it != rv->tracks().end() ) ) {
    ret = m_cut( *it );
    ++it;
  }
  return ret;
}

// ============================================================================
// The END
// ============================================================================
