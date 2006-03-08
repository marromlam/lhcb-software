// $Id: ProtoParticle.cpp,v 1.1 2006-03-08 13:38:58 cattanem Exp $
// Include files 

// STD and STL
#include <iostream> 
// from Gaudi
//#include "GaudiKernel/Kernel.h"

// local
#include "Event/ProtoParticle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ProtoParticle
//                                 derived from class ContainedObject
//
// 2002-07-08 : Gloria Corti
// 2005-07-25 : P. Koppenburg
//-----------------------------------------------------------------------------

//=============================================================================
// Copy constructor
//=============================================================================
LHCb::ProtoParticle::ProtoParticle(const LHCb::ProtoParticle& proto)
  : KeyedObject<int>()
  , m_extraInfo( proto.extraInfo() )
  , m_calo( proto.calo() )
  , m_track( proto.track() )
  , m_richPID( proto.richPID() )
  , m_muonPID( proto.muonPID() )
{

}

//=============================================================================
// Clone 
//=============================================================================
LHCb::ProtoParticle* LHCb::ProtoParticle::clone() const
{
  return new LHCb::ProtoParticle(*this);
}

//=============================================================================
// Assignment operator
//=============================================================================
LHCb::ProtoParticle& LHCb::ProtoParticle::operator=(const LHCb::ProtoParticle& orig) {
  
  // protect against self assignement
  if( this != &orig ) {
    m_extraInfo = orig.extraInfo();
    m_calo = orig.calo(); 
    m_track = orig.track();
    m_richPID = orig.richPID();
    m_muonPID = orig.muonPID();
  }
  return *this;
}    
    
//=============================================================================
// Return charge of ProtoParticle
//=============================================================================
int LHCb::ProtoParticle::charge() const {
  if ( !track() ) return 0;
  return track()->charge();
}
//=============================================================================
/** Check the presence of the information associated with 
 *  a given key
 *  
 *  @code
 * 
 *  const ProtoParticle* p = ... ;
 *
 *  ProtoParticle::Key key = ... ; 
 *  bool hasKey = p->hasInfo( key ) ;
 *
 *  @endcode 
 *  @param    key key to be checked 
 *  @return  'true' if there is informaiton with the 'key', 
 *           'false' otherwise
 */
bool LHCb::ProtoParticle::hasInfo  ( const int & key) const
{ return m_extraInfo.end() != m_extraInfo.find( key ) ; }

//=============================================================================
/** add/replace new information , associated with the key
 *  
 *  @code
 * 
 *  ProtoParticle* p = ... ;
 *
 *  ProtoParticle::Key  key   = ... ; 
 *  ProtoParticle::Info info  = ... ;
 * 
 *  bool inserted = p->addInfo( key , info ) ;
 *
 *  @endcode 
 * 
 *  @param key key for the information
 *  @param info information to be associated with the key
 *  @param 'true' if informaiton is inserted, 
 *         'false' if the previosu information has been replaced 
 */
bool  LHCb::ProtoParticle::addInfo ( int key, double info )
{ return m_extraInfo.insert( key , info ).second ;}
//=============================================================================
/** extract the information associated with the given key 
 *  If there is no such infomration the default value will 
 *  be returned 
 * 
 *  @code
 * 
 *  const ProtoParticle* p = ... ;
 *
 *  ProtoParticle::Key  key   = ... ; 
 *
 *  // extract the information
 *  ProtoParticle::Info info = p->info( key, -999 ) ;
 * 
 *  @endcode 
 *
 *  @param key key for the information
 *  @param def the default value to be returned 
 *         in the case of missing info
 *  @return information associated with the key if there 
 *          is such information, the default value otherwise 
 */
double LHCb::ProtoParticle::info( int key, double def ) const {
  ExtraInfo::iterator i = m_extraInfo.find( key ) ;
  return m_extraInfo.end() == i ? def : i->second ;
}

//=============================================================================
/** erase the information associated with the given key
 *
 *  @code
 * 
 *  ProtoParticle* p = ... ;
 *
 *  ProtoParticle::Key  key   = ... ; 
 * 
 *  int erased = p->eraseInfo( key ) ;
 *
 *  @endcode 
 * 
 *  @param key key for the information
 *  @return return number of erased elements 
 */
LHCb::ProtoParticle::ExtraInfo::size_type LHCb::ProtoParticle::eraseInfo( int key ){ 
  return m_extraInfo.erase( key ) ; 
}
