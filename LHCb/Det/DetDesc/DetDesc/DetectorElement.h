// $Id: DetectorElement.h,v 1.32 2005-12-07 13:19:07 cattanem Exp $
#ifndef  DETDESC_DETECTORELEMENT_H
#define  DETDESC_DETECTORELEMENT_H 1

// Some pragmas to avoid warnings in VisualC
#ifdef WIN32 
  // Disable warning C4786: identifier was truncated to '255' characters
  // in the debug information
  #pragma warning ( disable : 4786 )
#endif

// Include Files
#include <algorithm>
#include <string>
#include <vector>
#include <map>

#include "GaudiKernel/ITime.h"
#include "GaudiKernel/MsgStream.h"

#include "DetDesc/Services.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/ParamValidDataObject.h"
#include "DetDesc/IDetectorElement.h"
#include "DetDesc/CLIDDetectorElement.h"


// Forward declarations
class IDataProviderSvc;
class IMessageSvc;
template <class T> class DataObjectFactory;
class IUpdateManagerSvc;
class IRegistry;

/** @class DetectorElement DetectorElement.h DetDesc/DetectorElement.h
 *
 *  A "basic" implementation of Detector Description - Detector Element
 *
 *  @author Sebastien Ponce
 *  @author Rado Chytracek
 *  @author Ivan Belyaev
 *  @author Marco Clemencic
 *  @author Juan Palacios
*/

class DetectorElement: public ParamValidDataObject,
                       virtual public IDetectorElement {

  friend class DataObjectFactory<DetectorElement> ;  

public:
  /// Type of the internal list of used conditions
  typedef std::map<std::string,SmartRef<Condition> > ConditionMap;

  /// Constructors 
  DetectorElement (const std::string& name = "NotYetDefined");
  
  DetectorElement (const std::string& name,
                   const ITime& iValidSince,
                   const ITime& iValidTill); 

  ///  Destructor 
  virtual ~DetectorElement();

public:

  /**
   * This method initializes the detector element. It should be overridden
   * and used for computation purposes. This is a kind of hook for adding
   * user code easily in the initialization of a detector element.
   */
  virtual StatusCode initialize();

public:

  /// Obtain class ID
  virtual const  CLID& clID()   const  { return classID(); }
  static const   CLID& classID()       { return CLID_DetectorElement; }
  
  virtual const std::string& name () const {
    static std::string s_empty = "";
    IRegistry* pReg = registry();
    return (0!=pReg) ? pReg->identifier() : s_empty;;
  } 

  /// Return the SmartRef for the condition called 'name'.
  virtual SmartRef<Condition> condition(const std::string &name) const;

  /// Iterator to the first element of the list of conditions.
  inline  ConditionMap::const_iterator conditionBegin() const 
  { return m_de_conditions.begin(); }
  
  /// Iterator to the last+1 element of the list of conditions.
  inline  ConditionMap::const_iterator conditionEnd() const 
  { return m_de_conditions.end(); }

  /// delegation for geometry 
  inline        IGeometryInfo* geometry(); 

  /// delegation for geometry  (const version) 
  inline  const IGeometryInfo* geometry() const; 

  /// helper member using IGeometryInfo::isInside
  bool isInside(const Gaudi::XYZPoint& globalPoint) const;

  const IDetectorElement* childDEWithPoint(const Gaudi::XYZPoint& globalPoint) const;

  /// delegation for alignnment                
  inline        IAlignment*    alignment(); 

  /// delegation for alignment (const version) 
  inline  const IAlignment*    alignment() const; 

  // delegation for calibration 
  inline        ICalibration*  calibration(); 

  // delegation for calibration (const version) 
  inline  const ICalibration*  calibration() const; 

  // delegation for readout 
  inline        IReadOut*      readOut(); 

  // delegation for readout (const version) 
  inline  const IReadOut*      readOut() const ; 

  // delegation for slow control 
  inline        ISlowControl*  slowControl(); 

  // delegation for slow control (const version) 
  inline  const ISlowControl*  slowControl() const; 

  // delegation for fast control 
  inline        IFastControl*  fastControl(); 

  // delegation for fast control (const version) 
  inline  const IFastControl*  fastControl() const; 

  // another way to access: "pseudo-conversion"
  // cast to         IGeometryInfo*
  inline operator       IGeometryInfo*(); 

  // cast to   const IGeometryInfo* 
  inline operator const IGeometryInfo*() const; 

  // cast to         IAligment*     
  inline operator       IAlignment*   (); 

  // cast to   const IAlignment*    
  inline operator const IAlignment*   () const; 

  // cast to         ICalibration*  
  inline operator       ICalibration* (); 

  // cast to   const ICalibration*  
  inline operator const ICalibration* () const; 

  // cast to         IReadOut*      
  inline operator       IReadOut*     (); 

  // cast to   const IReadOut*      
  inline operator const IReadOut*     () const; 

  // cast to         ISlowControl*  
  inline operator       ISlowControl* (); 

  // cast to   const ISlowControl*  
  inline operator const ISlowControl* () const; 

  // cast to         IFastControl*  
  inline operator       IFastControl* (); 

  // cast to   const IFastControl*  
  inline operator const IFastControl* () const; 

  // cast to         IGeometryInfo&     
  // (potentially could throw DetectorElementException)
  inline operator       IGeometryInfo&(); 

  // cast to   const IGeometryInfo& 
  // (potentially could throw DetectorElementException)
  inline operator const IGeometryInfo&() const; 

  // cast to         IAlignment&    
  // (potentially could throw DetectorElementException)
  inline operator       IAlignment&   (); 

  // cast to   const IAlignment&    
  // (potentially could throw DetectorElementException)
  inline operator const IAlignment&   () const; 

  // cast to         ICalibration&  
  // (potentially could throw DetectorElementException)
  inline operator       ICalibration& ()                ; 

  // cast to   const ICalibration&  
  // (potentially could throw DetectorElementException)
  inline operator const ICalibration& ()          const ; 

  // cast to         IReadOut&      
  // (potentially could throw DetectorElementException)
  inline operator       IReadOut&     ()                ; 

  // cast to   const IReadOut&      
  // (potentially could throw DetectorElementException)
  inline operator const IReadOut&     ()          const ; 

  // cast to         ISlowControl&  
  // (potentially could throw DetectorElementException)
  inline operator       ISlowControl& ()                ; 

  // cast to   const ISlowControl&  
  // (potentially could throw DetectorElementException)
  inline operator const ISlowControl& ()          const ; 

  // cast to         IFastControl&  
  // (potentially could throw DetectorElementException)
  inline operator       IFastControl& ()                ; 

  // cast to   const IFastControl&  
  // (potentially could throw DetectorElementException)
  inline operator const IFastControl& ()          const ; 

  //  printout (overloaded)
  // (potentially could throw DetectorElementException)
  virtual std::ostream& printOut( std::ostream& ) const;  

  /// reset to the initial state
  IDetectorElement* reset() ;    

  virtual MsgStream&    printOut( MsgStream&    ) const;  

  // pointer to parent IDetectorElement (const version)
  virtual IDetectorElement*  parentIDetectorElement() const;

  // (reference to) container of pointers to child detector elements 
  virtual IDetectorElement::IDEContainer& childIDetectorElements() const;

  // iterators for manipulation of daughter elements 
  inline virtual IDetectorElement::IDEContainer::iterator childBegin() {
    return childIDetectorElements().begin();
  }
  inline virtual IDetectorElement::IDEContainer::const_iterator
    childBegin() const { return childIDetectorElements().begin() ; }
  inline virtual IDetectorElement::IDEContainer::iterator
    childEnd  () { return childIDetectorElements().end  () ; }
  inline virtual IDetectorElement::IDEContainer::const_iterator
    childEnd  () const { return childIDetectorElements().end  () ; }

  /**
   * Method used to access the ParamValidDataObject methods from IDetectorElement
   * interface.
   */
  virtual const ParamValidDataObject *params() const;

  /**
   * Return a sensitive volume identifier for a given point in the 
   * global reference frame.
   */

  virtual const int sensitiveVolumeID(const Gaudi::XYZPoint& globalPos) const;

  /// Used to create a link with a given name to the condition at 'path' in the detector data store.
  void createCondition(std::string &name, std::string &path);

  //-- N. Gilardi; 2005.07.08 ---------------------------------------------
  /// Returns list of existing parameter vectors as a vector of their names
  virtual std::vector<std::string> conditionNames() const;
  //-----------------------------------------------------------------------

  ///
  /// specific   
  // create "ghost" 
  const IGeometryInfo* createGeometryInfo();

  // create "orphan"
  const IGeometryInfo* createGeometryInfo( const std::string& logVol);

  // create "regular"
  const IGeometryInfo* createGeometryInfo
         (const std::string& logVol, 
          const std::string& support,
          const ILVolume::ReplicaPath & replicaPath );

  const IGeometryInfo* createGeometryInfo
         (const std::string& logVol, 
          const std::string& support,
          const ILVolume::ReplicaPath & replicaPath,
          const std::string& alignmentPath);

  const IGeometryInfo* createGeometryInfo
         (const std::string& logVol, 
          const std::string& support,
          const std::string& namePath);

  const IGeometryInfo* createGeometryInfo
         (const std::string& logVol, 
          const std::string& support,
          const std::string& namePath,
          const std::string& alignmentPath);

  const IAlignment* createAlignment (const std::string& condition);

  const ICalibration* createCalibration (const std::string& condition);

  const IReadOut* createReadOut (const std::string& condition);

  const ISlowControl* createSlowControl (const std::string& condition);

  const IFastControl* createFastControl (const std::string& condition);

public:

  // Implementation of IInterface
  virtual unsigned long addRef();
  virtual unsigned long release();
  virtual StatusCode queryInterface( const InterfaceID& , void** );
  
protected:
  
  /// specific 
  void setGeometry( IGeometryInfo* geoInfo ) { m_de_iGeometry = geoInfo; }
  
  IDataProviderSvc*  dataSvc () const;
  IMessageSvc*       msgSvc  () const;
  IUpdateManagerSvc* updMgrSvc() const;
  
private:

  // technicalities 

  inline void Assert (bool assertion,
                      const std::string& name =
                      "DetectorElement Unknown Exception") const;

  // for IDetectorElement implementation 
  IGeometryInfo*         m_de_iGeometry     ;
  IAlignment*            m_de_iAlignment    ;
  ICalibration*          m_de_iCalibration  ;   
  IReadOut*              m_de_iReadOut      ;
  ISlowControl*          m_de_iSlowControl  ;
  IFastControl*          m_de_iFastControl  ;

  /// Container of the SmartRefs for the conditions.
  ConditionMap m_de_conditions;

  mutable bool                              m_de_childrensLoaded;
  mutable IDetectorElement::IDEContainer    m_de_childrens; 

  /// This defines the type of a userParameter
  enum userParamKind { DOUBLE, INT, OTHER };

  /// reference to services
  DetDesc::Services* m_services;

};

// implementation of the inlines functions
#include "DetDesc/DetectorElement.icpp"

#endif    //    DETDESC_DETECTORELEMENT_H 

