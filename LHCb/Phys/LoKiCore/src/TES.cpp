// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgContextSvc.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/ToStream.h"
#include "GaudiKernel/ObjectContainerBase.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GetAlgs.h"
// ============================================================================
// LHCbKernel
// ============================================================================
#include "Kernel/Counters.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/TES.h"
#include "LoKi/ILoKiSvc.h"
#include "LoKi/Constants.h"
#include "LoKi/Exception.h"
#include "LoKi/Services.h"
// ============================================================================
#ifdef __INTEL_COMPILER       // Disable ICC remark
#pragma warning(disable:2259) // non-pointer conversion may lose significant bits
#pragma warning(disable:1572) // floating-point equality and inequality comparisons are unreliable
#endif
// ============================================================================
/** @file
 *  Implementation file for classes from namespace LoKi::TES
 *
 *  This file is a part of LoKi project -
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2020-02-13
 */
// ============================================================================
namespace
{
  // ==========================================================================
  /// the invalid algorithm name
  const std::string s_INVALID = "-<NON-YET-LOCATED-ALGORITHM>-" ;
  // ==========================================================================
}
// ============================================================================
/*  constructor from TES location and root-in-tes flag
 *  @param location     TES-location
 *  @param useRootInTES RootInTES-flag 
 */
// ============================================================================
LoKi::TES::Get::Get
( const std::string& location     , 
  const bool         useRootInTES ) 
  : LoKi::AuxFunBase () 
  , m_location     ( location     ) 
  , m_useRootInTES ( useRootInTES ) 
  , m_algorithm () 
  , m_datasvc   () 
{
  if ( gaudi() ) { getAlgSvc() ; }
}
// ============================================================================
// virtual destructor 
// ============================================================================
LoKi::TES::Get::~Get()
{
  if ( m_algorithm && !gaudi() ) { m_algorithm.reset() ; }
  if ( m_datasvc   && !gaudi() ) { m_datasvc  .reset() ; }
}
// ============================================================================
void LoKi::TES::Get::getAlgSvc() const
{
  //
  if ( !(!m_algorithm) || !(!m_datasvc)) { return ; }
  //
  // 1. locate algorithm 
  //
  ILoKiSvc* l = lokiSvc() ;
  Assert ( 0 != l   , "ILoKiSvc*       points to NULL!" ) ;
  SmartIF<IAlgContextSvc> cntx ( l ) ;
  if ( !(!cntx) )
  { m_algorithm = Gaudi::Utils::getGaudiAlg ( cntx ) ; }
  //
  // 2. use data service only in case root-in-tes is not required 
  //
  if ( !m_algorithm && !m_useRootInTES ) 
  {
    const LoKi::Services& svc = LoKi::Services::instance() ;
    m_datasvc = svc.evtSvc() ;
  } 
  //
  Assert ( !(!m_algorithm) || !(!m_datasvc)  , 
           "Neither algorithm nor service is located" ) ;
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& LoKi::TES::Get::fillStream ( std::ostream& s ) const 
{
  s << " GET(" << "'" << location() << "'" ;
  if ( !useRootInTES() ) { s << ", False" ; }
  return s << ") " ;
}
// ============================================================================
// get algorithm name
// ============================================================================
const std::string& LoKi::TES::Get::algName() const
{ return !algorithm() ? s_INVALID : algorithm()->name() ; }
// ============================================================================
// constructor from TES location
// ============================================================================
LoKi::TES::Exists::Exists
( const std::string& location  ,
  const bool         rootInTes )
  : LoKi::AuxFunBase ( std::tie ( location , rootInTes ) )  
  , LoKi::Functor<void,bool> ()
  , LoKi::TES::Get ( location , rootInTes ) 
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::TES::Exists::~Exists(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::TES::Exists* LoKi::TES::Exists::clone() const
{ return new LoKi::TES::Exists ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::TES::Exists::result_type
LoKi::TES::Exists::operator() ( /* LoKi::TES::Exists::argument */ ) const
{ return LoKi::TES::exists_<DataObject> ( *this ) ; }
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream&
LoKi::TES::Exists::fillStream ( std::ostream& s ) const
{
  s << " EXISTS( " ;
  Gaudi::Utils::toStream ( location () , s ) ;
  if ( useRootInTES()  ) { s << " , False" ; }
  return s << " ) " ;
}
// ============================================================================

// ============================================================================
// constructor from TES location
// ============================================================================
LoKi::TES::Contains::Contains
( const std::string& location     ,
  const bool         useRootInTes )
  : LoKi::AuxFunBase ( std::tie ( location , useRootInTes ) )  
  , LoKi::Functor<void,double> ()
  , LoKi::TES::Get ( location , useRootInTes ) 
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::TES::Contains::~Contains(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::TES::Contains* LoKi::TES::Contains::clone() const
{ return new LoKi::TES::Contains ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::TES::Contains::result_type
LoKi::TES::Contains::operator() ( /* LoKi::TES::Contains::argument */ ) const
{
  //
  const ObjectContainerBase *obj = LoKi::TES::get_<ObjectContainerBase> ( *this ) ;
  //
  if ( NULL == obj ) { return -1 ; } // REUTRN
  //
  return obj -> numberOfObjects () ;
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream&
LoKi::TES::Contains::fillStream ( std::ostream& s ) const
{
  s << " CONTAINS( " ;
  Gaudi::Utils::toStream ( location() , s ) ;
  if ( !useRootInTES() ) { s << " , False" ; }
  return s << " ) " ;
}
// ============================================================================


// ============================================================================
// constructor from TES location
// ============================================================================
LoKi::TES::Counter::Counter
( const std::string& location     ,
  const std::string& counter      ,
  const double       bad          ,
  const bool         useRootInTes )
  : LoKi::AuxFunBase ( std::tie ( location , counter , bad , useRootInTes ) )  
  , LoKi::TES::Contains ( location , useRootInTes )
  , m_counter ( counter )
  , m_bad     ( bad     )
{}
// ============================================================================
// constructor from TES location
// ============================================================================
LoKi::TES::Counter::Counter
( const std::string& location     ,
  const std::string& counter      )
  : LoKi::AuxFunBase ( std::tie ( location , counter ) )  
  , LoKi::TES::Contains ( location )
  , m_counter ( counter )
  , m_bad     ( LoKi::Constants::NegativeInfinity )
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::TES::Counter::~Counter(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::TES::Counter* LoKi::TES::Counter::clone() const
{ return new LoKi::TES::Counter ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::TES::Counter::result_type
LoKi::TES::Counter::operator() ( /* LoKi::TES::Contains::argument */ ) const
{
  //
  const Gaudi::Numbers* data = LoKi::TES::get_<Gaudi::Numbers> ( *this );
  if( NULL == data )
  {
    Error ("No valid object is found for TES location, return 'bad'") ;
    return m_bad ;
  }
  //
  const Gaudi::Numbers::Map& m = data->counters() ;
  Gaudi::Numbers::Map::const_iterator ifind = m.find ( counter() ) ;
  if ( m.end() == ifind )
  {
    Error ( "No counter is found, return 'bad'") ;
    return m_bad ;     // RETURN
  }
  //
  return ifind->second ;
  //
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream&
LoKi::TES::Counter::fillStream ( std::ostream& s ) const
{
  s << " COUNTER( " ;
  Gaudi::Utils::toStream ( location () , s ) ;
  s << "," ;
  Gaudi::Utils::toStream ( counter  () , s ) ;
  //
  if ( LoKi::Constants::NegativeInfinity != m_bad  ) { s << ", " << m_bad  ; }
  if ( !useRootInTES ()                            ) { s << " , False" ; }
  //
  return s << " ) " ;
}
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace TES 
  {
    // =======================================================================-
    /** Helper class used to extract information from a \c StatEntity object.
     *
     * @author Marco Clemencic <marco.clemencic@cern.ch>
     */
    class StatEntityGetter {
    private:
      /// List of known getters in \c StatEntity.
      enum StatFunction 
        {
          nEntries,
          sum,
          sum2,
          mean,
          rms,
          meanErr,
          min,
          max,
          eff,
          effErr
        };
      /// Base class for the internal helper object.
      /// The derived class have to re-implement the operator() and the method
      /// name().
      struct BaseHelper 
      {
        /// Virtual destructor.
        virtual ~BaseHelper() {}
        /// Accessor method.
        /// Extract some data from a \c StatEntity object.
        virtual double operator() (const StatEntity &ent) const = 0;
        /// Name of the data member of \c StatEntity that the helper gets.
        virtual std::string name() const = 0;
      };
      
      /// Helper used to actually access the content of \c StatEntity.
      template <StatFunction Fun>
      struct Helper: public BaseHelper {
        /// Virtual Destructor.
        virtual ~Helper() {}
        /// Extract the data member specified with the template argument from a
        /// \c StatEntity object.
        virtual double operator()(const StatEntity &ent) const;
        /// Return a string representing, essentially, the template argument.
        virtual std::string name() const;
      };
      
      /// Commodity function to create a new specialized \c Helper instance.
      /// This function is used only by the other, non-templated, setHelper() method.
      template <StatFunction Fun>
      inline void setHelper();
      
      /// Commodity function to create a new specialized Helper instance.
      /// This method uses internally the templated one for simplicity. The
      /// separation between the two member is not really needed except to ensure
      /// that all the entries in the enum StatFunction are used and (not more).
      void setHelper(StatFunction fun);
      
      /// Pointer to the actual instance of the Helper class.
      boost::shared_ptr<BaseHelper> m_helper;
      
    public:
      
      /// Constructor.
      /// Maps a string the the required Helper instance to extract the requested
      /// data member for \c StatEntity.
      StatEntityGetter(const std::string &fun) {
        // basic
        if      ( "nEntries" == fun ) { setHelper(nEntries); }
        else if ( "sum"      == fun ) { setHelper(sum     ); }
        else if ( "sum2"     == fun ) { setHelper(sum2    ); }
        else if ( "mean"     == fun ) { setHelper(mean    ); }
        else if ( "rms"      == fun ) { setHelper(rms     ); }
        else if ( "meanErr"  == fun ) { setHelper(meanErr ); }
        else if ( "min"      == fun ) { setHelper(min     ); }
        else if ( "max"      == fun ) { setHelper(max     ); }
        // derived
        else if ( "efficiency"    == fun ) { setHelper(eff   ); }
        else if ( "efficiencyErr" == fun ) { setHelper(effErr); }
        else if ( "eff"           == fun ) { setHelper(eff   ); }
        else if ( "effErr"        == fun ) { setHelper(effErr); }
        // a'la ROOT
        else if ( "Sum"     == fun ) { setHelper(sum     ); }
        else if ( "Sum2"    == fun ) { setHelper(sum2    ); }
        else if ( "Mean"    == fun ) { setHelper(mean    ); }
        else if ( "Rms"     == fun ) { setHelper(rms     ); }
        else if ( "RMS"     == fun ) { setHelper(rms     ); }
        else if ( "MeanErr" == fun ) { setHelper(meanErr ); }
        else if ( "Min"     == fun ) { setHelper(min     ); }
        else if ( "Max"     == fun ) { setHelper(max     ); }
        else if ( "Eff"     == fun ) { setHelper(eff     ); }
        else if ( "EffErr"  == fun ) { setHelper(effErr  ); }
        else if ( "N"       == fun ) { setHelper(nEntries); }
        else if ( "Entries" == fun ) { setHelper(nEntries); }
        else if ( "entries" == fun ) { setHelper(nEntries); }
        
        else { throw LoKi::Exception("invalid function name '" + fun + "'") ; }
      }
      
      /// Accessor function.
      /// Forwards the call to the \c Helper instance.
      inline double operator() (const StatEntity &ent) const
      {
        assert(m_helper.get());
        return (*m_helper)(ent);
      }
      
      /// Name of the \c StatEntity data member.
      /// Forwards the call to the \c Helper instance.
      inline std::string name() const 
      {
        assert(m_helper.get());
        return m_helper->name();
      }
    };

// Macro to simplify the specialization of Helper
#define SpecializedHelper(Fun)                  \
    template <>                                                         \
  double StatEntityGetter::Helper<StatEntityGetter::Fun>::operator() (const StatEntity &ent) const { \
    return ent.Fun(); \
  } \
  template <> \
  std::string StatEntityGetter::Helper<StatEntityGetter::Fun>::name() const { \
    return #Fun; \
  } \
  template <> \
  inline void StatEntityGetter::setHelper<StatEntityGetter::Fun>() { \
    m_helper.reset(new Helper<Fun>); \
  }

SpecializedHelper(nEntries)
SpecializedHelper(sum     )
SpecializedHelper(sum2    )
SpecializedHelper(mean    )
SpecializedHelper(rms     )
SpecializedHelper(meanErr )
SpecializedHelper(min     )
SpecializedHelper(max     )
SpecializedHelper(eff     )
SpecializedHelper(effErr  )

// Function used to convert the enum value to a Helper instance.
// It uses a switch to profit from the compiler checks on the treated cases,
// ensuring that all the entries in the enum (and not more) are considered.
void StatEntityGetter::setHelper(StatFunction fun) {
  switch (fun) {
  case nEntries: setHelper<nEntries>(); break;
  case sum     : setHelper<sum     >(); break;
  case sum2    : setHelper<sum2    >(); break;
  case mean    : setHelper<mean    >(); break;
  case rms     : setHelper<rms     >(); break;
  case meanErr : setHelper<meanErr >(); break;
  case min     : setHelper<min     >(); break;
  case max     : setHelper<max     >(); break;
  case eff     : setHelper<eff     >(); break;
  case effErr  : setHelper<effErr  >(); break;
  }
}

}} // end of namespace LoKi::TES

// ============================================================================
// constructor from TES location
// ============================================================================
LoKi::TES::Stat::Stat
( const std::string&            location     ,
  const std::string&            counter      ,
  const StatEntityGetter&       function     ,
  const double                  bad          ,
  const bool                    useRootInTes )
  : LoKi::TES::Counter ( location , counter , bad , useRootInTes )
  , m_getter  ( new StatEntityGetter(function) )
{}
// ============================================================================
// constructor from TES location
// ============================================================================
LoKi::TES::Stat::Stat
( const std::string&            location     ,
  const std::string&            counter      ,
  const StatEntityGetter&       function     )
  : LoKi::TES::Counter ( location , counter )
  , m_getter  ( new StatEntityGetter(function) )
{}
// ============================================================================
// constructor from TES location
// ============================================================================
LoKi::TES::Stat::Stat
( const std::string&            location     ,
  const std::string&            counter      ,
  const std::string&            function     ,
  const double                  bad          ,
  const bool                    useRootInTes )
  : LoKi::AuxFunBase (  std::tie ( location , counter , function , bad , useRootInTes ) ) 
  , LoKi::TES::Counter ( location , counter , bad , useRootInTes )
  , m_getter  ( new StatEntityGetter(function) )
{}
// ============================================================================
// constructor from TES location
// ============================================================================
LoKi::TES::Stat::Stat
( const std::string&            location     ,
  const std::string&            counter      ,
  const std::string&            function          )
  : LoKi::AuxFunBase (  std::tie ( location , counter , function ) ) 
  , LoKi::TES::Counter ( location , counter )
  , m_getter  ( new StatEntityGetter(function) )
{}

// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::TES::Stat::~Stat(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::TES::Stat* LoKi::TES::Stat::clone() const
{ return new LoKi::TES::Stat ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::TES::Stat::result_type
LoKi::TES::Stat::operator() ( /* LoKi::TES::Contains::argument */ ) const
{
  //
  if ( counter().empty() )
  {
    const Gaudi::Counter* cnt = LoKi::TES::get_<Gaudi::Counter>  (*this ) ;
    if( NULL != cnt )
    {
      return (*m_getter)(cnt->counter());
    }
  }
  //
  const Gaudi::Counters* data = LoKi::TES::get_<Gaudi::Counters> ( *this) ;
  if ( NULL == data )
  {
    Error ("No valid object is found for TES location, return 'bad'") ;
    return bad ()  ;
  }
  //
  const Gaudi::Counters::Map& m = data->counters() ;
  Gaudi::Counters::Map::const_iterator ifind = m.find ( counter() ) ;
  if ( m.end() == ifind )
  {
    Error ( "No counter is found, return 'bad'") ;
    return bad ()  ;     // RETURN
  }

  return (*m_getter)(ifind->second);
}

// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream&
LoKi::TES::Stat::fillStream ( std::ostream& s ) const
{
  s << " STAT( " ;
  Gaudi::Utils::toStream ( location () , s ) ;
  s << "," ;
  Gaudi::Utils::toStream ( counter  () , s ) ;
  //
  s << m_getter->name();
  //
  if ( LoKi::Constants::NegativeInfinity != bad()  ) { s << ", " << bad() ; }
  if ( !useRootInTES ()                            ) { s << " , False" ; }
  //
  return s << " ) " ;
}
// ============================================================================

// ============================================================================
// The END
// ============================================================================
