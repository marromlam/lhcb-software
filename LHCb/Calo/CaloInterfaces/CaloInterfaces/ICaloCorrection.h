// $Id: ICaloCorrection.h,v 1.8 2007-03-29 16:55:34 odescham Exp $
// ============================================================================
#ifndef CALOINTERFACES_ICALOCORRECTION_H 
#define CALOINTERFACES_ICALOCORRECTION_H 1
// Include files
// STD & STL 
#include <functional>
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/Point3DTypes.h"
#include "Event/CaloPosition.h"

/** @class ICaloCorrection ICaloCorrection.h CaloInterfaces/ICaloCorrection.h
 *  
 *  A minimal abstract interface for S-,L-shape correction functions 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   22/03/2002
 */

static const InterfaceID IID_ICaloCorrection( "ICaloCorrection" , 1 , 0 );


class ICaloCorrection :
  public virtual IAlgTool , 
  public std::binary_function<const LHCb::CaloPosition, const  LHCb::CaloPosition::Center , LHCb::CaloPosition*>
{
public:
  
  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_ICaloCorrection; };
  
  /** The main processing method 
   *  @param  position  uncorrected position
   *  @param  seed      position of the 'seed' 
   *  @param  corrected new created corrected CaloPosition object 
   *  @return status code 
   */  
  virtual StatusCode     process    
  ( const LHCb::CaloPosition    & position  , 
    const LHCb::CaloPosition::Center & seed      , 
    LHCb::CaloPosition*&        corrected ) const = 0 ;
  
  /** The main processing method 
   *  @param  position  uncorrected position
   *  @param  seed      position of the 'seed' 
   *  @return new created corrected CaloPosition object 
   */  
  virtual LHCb::CaloPosition* process    
  ( const LHCb::CaloPosition & position , 
    const LHCb::CaloPosition::Center & seed     ) const = 0 ;
  
  /** The main processing method (functor interface)
   *  @param  position  uncorrected position
   *  @param  seed      position of the 'seed' 
   *  @return new created corrected CaloPosition object 
   */  
  virtual LHCb::CaloPosition* operator() 
    ( const LHCb::CaloPosition & position , 
      const LHCb::CaloPosition::Center & seed     ) const = 0 ;

protected:
  
  
};

// ============================================================================
#endif // CALOINTERFACES_ICALOCORRECTION_H
// ============================================================================
