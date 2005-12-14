// $Id: DeMuonDetector.cpp,v 1.12 2005-12-14 17:31:05 asarti Exp $

// Include files
#include "MuonDet/DeMuonDetector.h"
#include "MuonDet/DeMuonGasGap.h"
#include "MuonDet/MuonChamberGrid.h"
#include "MuonDet/MuonChamberLayout.h"
#include "MuonDet/DeMuonRegion.h"

//Detector description
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/SolidBox.h"

//Muon Kernel
#include "MuonKernel/MuonTileID.h"

/** @file DeMuonDetector.cpp
 * 
 * Implementation of class : DeMuonDetector
 *
 * @author Alessia Satta
 * @author Alessio Sarti, asarti@lnf.infn.it
 *
 */

#include "DetDescCnv/XmlUserDetElemCnv.h"

typedef XmlUserDetElemCnv<DeMuonDetector>       XmlDeMuonDetector;
static CnvFactory<XmlDeMuonDetector>           s_XmlDeMuonDetectorFactory;
const ICnvFactory&  XmlDeMuonDetectorFactory = s_XmlDeMuonDetectorFactory;

/// Standard Constructor
DeMuonDetector::DeMuonDetector() {

  bool debug = false;
  if(debug) std::cout<< "Building the Detector !!!" <<std::endl;
  m_detSvc = this->dataSvc();
  m_stations = 0;
  m_regions = 0;
}

/// Standard Destructor
DeMuonDetector::~DeMuonDetector()
{
}
  
const CLID& DeMuonDetector::clID () const 
{ 
  return DeMuonDetector::classID() ; 
}

StatusCode DeMuonDetector::initialize()  
{

  MsgStream msg( msgSvc(), name() );
  msg << MSG::VERBOSE << "Initializing the detector" <<endreq;

  StatusCode sc = DetectorElement::initialize();
  if( sc.isFailure() ) { 
    msg << MSG::ERROR << "Failure to initialize DetectorElement" << endreq;
    return sc ; 
  }

  //Initialize the maximum number of allowed chambers per region
  int myDum[4] = {12,24,48,192};
  for(int dum = 0; dum<4; dum++) {MaxRegions[dum] = myDum[dum];}

  //Initializing the Layout
  m_chamberLayout.initialize();
  m_chamberLayout.setDataProvider(m_detSvc);
  m_ChmbPtr =  m_chamberLayout.fillChambersVector(m_detSvc);

  //fill geo info
  fillGeoInfo();

  //Initialize vectors containing Detector informations
  CountDetEls();

  return sc;
}



StatusCode DeMuonDetector::Hit2GapNumber(HepPoint3D myPoint, 
					 int station, int & gapNumber,
					 int & chamberNumber, int& regNum){
  
  //This methods sets the gap = 0 if only the
  //station is provided [HitPoint.z() = 0]
  StatusCode sc = StatusCode::FAILURE;
  bool isIn = false; bool debug = false;

  MsgStream msg( msgSvc(), name() );
  if(debug)   std::cout<<"My station: "<<station<<std::endl;
  sc = Hit2ChamberNumber(myPoint,station,chamberNumber,regNum);
  DeMuonChamber * theChmb = getChmbPtr(station,regNum,chamberNumber);

  //Set SC to failure until gap is found
  sc = StatusCode::FAILURE;

  double zPoi = myPoint.z(); int Igap;
  double zChmb = (theChmb->geometry())->toGlobal(HepPoint3D(0,0,0)).z();  
  for (Igap = 0; Igap <4; Igap++) {
    //Use z information to catch the gap.
    if((zPoi-zChmb)<-20+Igap*16) break;
  }
  if(debug)   std::cout<<"Z difference: "<<zPoi<<" "<<
                zChmb<<" "<<zPoi-zChmb<<" "<<Igap<<std::endl;
  if(Igap <4) { 	
    sc = StatusCode::SUCCESS;
    gapNumber = Igap; 
  } else {
    //Is the chamber returned containing the hit?
    IDetectorElement::IDEContainer::iterator itGap= theChmb->childBegin();
    for(itGap=theChmb->childBegin(); itGap<theChmb->childEnd(); itGap++){
      IGeometryInfo* geoGap = (*itGap)->geometry();  
      isIn = geoGap->isInside(myPoint);
      if(isIn) {
        DeMuonGasGap*  myGap =  dynamic_cast<DeMuonGasGap*>( *itGap ) ;
        gapNumber = myGap->gasGapNumber();
        sc = StatusCode::SUCCESS;
        break;
      }
    }
  }
  
  return sc;
}


StatusCode DeMuonDetector::Hit2ChamberNumber(HepPoint3D myPoint, 
					     int station, 
					     int & chamberNumber, int& regNum){
  
  StatusCode sc = StatusCode::FAILURE;
  bool isIn = false; bool debug = false;
  
  MsgStream msg( msgSvc(), name() );

  double x = myPoint.x();  double y = myPoint.y(); double z = myPoint.z();

  if(debug) std::cout<< "Hit 2 chamber problems " <<x<<" "
                     <<y<<" "<<station<<" "<<std::endl;
  //Returning the most likely chamber
  m_chamberLayout.chamberMostLikely(x,y,station,chamberNumber,regNum);
  if(regNum>=0){
    
    //Providing all 3 numbers identifies a chamber
    IGeometryInfo* geoChm = 
      (getChmbPtr(station,regNum,chamberNumber))->geometry();  
    
    //For calls not providing hit z take the z of chamber center
    if(!z) {myPoint.setZ(geoChm->toGlobal(HepPoint3D(0,0,0)).z());}
    
    //Is the chamber returned containing the hit?
    isIn = geoChm->isInside(myPoint);
    
    if(isIn) {
      msg << MSG::DEBUG << "Hit found" <<endreq;
      sc = StatusCode::SUCCESS;
    } else {
      
      msg << MSG::DEBUG <<"Starting point::  "
          <<station<<" "<<chamberNumber<<" "<<regNum<<endreq;
      
      //Find the vector of chambers near the one under investigation
      double x_ref(0),y_ref(0); std::vector<DeMuonChamber*> myChams;
      x_ref = geoChm->toGlobal(HepPoint3D(0,0,0)).x();
      y_ref = geoChm->toGlobal(HepPoint3D(0,0,0)).y();
      
      int x_sgn(0),y_sgn(0);
      //Avoid unnecessary checks for resolution problems
      if(fabs(x-x_ref)>0.01) {x_sgn = (x-x_ref)/fabs(x-x_ref);}
      if(fabs(y-y_ref)>0.01) {y_sgn = (y-y_ref)/fabs(y-y_ref);}
      
      msg << MSG::DEBUG << "Hit not found Try to look in corner "
          <<x_ref<< " "<<x<<" "<<x_sgn<<" "<<y_ref<<" "<<y<<" "<<
        y_sgn<<endreq;    
      
      
      myChams = m_chamberLayout.neighborChambers(chamberNumber,
                                                 station,regNum,x_sgn,
                                                 y_sgn);
      
      if(debug) std::cout<< "Neighborh chamber problems" <<std::endl;
      
      std::vector<DeMuonChamber*>::iterator aChamber;
      //Loops on found chambers
      for(aChamber = myChams.begin(); aChamber<myChams.end(); aChamber++){
        
        int tmpChn(0), tmpRen(0), tmpStn(0);
        tmpChn = (*aChamber)->chamberNumber();
        tmpRen = (*aChamber)->regionNumber();
        tmpStn = (*aChamber)->stationNumber();
        
        //Accessing Geometry Info
        IGeometryInfo* geoOthChm = (getChmbPtr(tmpStn,tmpRen,tmpChn))
          ->geometry();  
        if(geoOthChm) {
          //For calls not providing hit z take the z of chamber center
          if(!z) {myPoint.setZ(geoOthChm->toGlobal(HepPoint3D(0,0,0)).z());}
	
          isIn = geoOthChm->isInside(myPoint);
          
          if(isIn) {
            sc = StatusCode::SUCCESS;
            //Returns the correct region and chamber number
            chamberNumber = (*aChamber)->chamberNumber();
            regNum = (*aChamber)->regionNumber();
            break;
          }
        } else {
          msg << MSG::WARNING 
              << "Could not find Geometry info of a given chamber!"
              <<endreq;    
        }
      }
      
    //Debug the chambers returned
      if(debug) {
        std::cout<<"Chmb test n.  :"<<chamberNumber+1<<"  ";
        std::cout<<"Chmb ret.  : ";
        for(aChamber = myChams.begin(); aChamber<myChams.end(); aChamber++){
          std::cout<<(*aChamber)->chamberNumber()+1<<" ";
        }
        std::cout<<" "<<std::endl;
      }
    }
  }
  
  if(!isIn) {
    msg << MSG::DEBUG << 
      "Smart seek didn't work. Perform loop on all chambers :( !!! " 
        <<endreq;
    int msta(0),mreg(0),mchm(0); 
    //Getting stations
    IDetectorElement::IDEContainer::iterator itSt=this->childBegin();
    for(itSt=this->childBegin(); itSt<this->childEnd(); itSt++){
      if(msta == station) {
        //Getting regions
        mreg = 0;
        IDetectorElement::IDEContainer::iterator itRg=(*itSt)->childBegin();
        for(itRg=(*itSt)->childBegin(); itRg<(*itSt)->childEnd(); itRg++){
          //Getting chambers
          mchm = 0;
          IDetectorElement::IDEContainer::iterator itCh=(*itRg)->childBegin();
          for(itCh=(*itRg)->childBegin(); itCh<(*itRg)->childEnd(); itCh++){
            IGeometryInfo* geoAllChm = (*itCh)->geometry();  
            
            //For calls not providing hit z take the z of chamber center
            if(!z) {myPoint.setZ(geoAllChm->toGlobal(HepPoint3D(0,0,0)).z());}
            
            isIn = geoAllChm->isInside(myPoint);
            if(isIn) {
              sc = StatusCode::SUCCESS;
              //Returns the correct region and chamber number
              chamberNumber = mchm;  regNum = mreg;
              msg << MSG::DEBUG << "Hit found in chamber C: " <<
                chamberNumber<<" , R: "<<regNum<<" ,S: "<<station<<endreq;
              return sc;
            }
            mchm++;
          }//Chamber Loop
          mreg++;
        }//Region Loop
      }
      msta++;
    }//Stations Loop
  }  
  return sc;
}


StatusCode DeMuonDetector::Pos2StChamberNumber(const double x,
					       const double y,
					       int station,
					       int & chamberNumber, int& regNum){
  StatusCode sc = StatusCode::FAILURE;
  //Hit Z is not know (giving only the station).
  //Take the chamber Z to update the hit later on.
  HepPoint3D hitPoint(x,y,0); 

  sc = Hit2ChamberNumber(hitPoint,station,chamberNumber,regNum);

  return sc;
}

StatusCode DeMuonDetector::Pos2StGapNumber(const double x,
                                           const double y,
                                           int station, int & gapNumber,
                                           int & chamberNumber, int& regNum){
  StatusCode sc = StatusCode::FAILURE;
  //Hit Z is not know (giving only the station).
  //Take the chamber Z to update the hit later on.
  HepPoint3D hitPoint(x,y,0); 
  
  sc = Hit2GapNumber(hitPoint,station,gapNumber,chamberNumber,regNum);

  return sc;
}


StatusCode DeMuonDetector::Pos2StChamberPointer(const double x,
                                                const double y,
                                                int station,
                                                DeMuonChamber* & 
                                                chamberPointer){
  StatusCode sc = StatusCode::FAILURE;
  //Hit Z is not know (giving only the station).
  //Take the chamber Z to update the hit later on.
  HepPoint3D hitPoint(x,y,0); 
  int chamberNumber(-1),regNum(-1);
  sc = Hit2ChamberNumber(hitPoint,station,chamberNumber,regNum);
  if((regNum > -1) && (chamberNumber>-1)) {
    DeMuonChamber*  myPtr =  getChmbPtr(station,regNum,chamberNumber) ;
    chamberPointer = myPtr;
  }
  
  return sc;
}


StatusCode DeMuonDetector::Pos2ChamberPointer(const double x,
                                              const double y,
                                              const double z,
                                              DeMuonChamber* & chamberPointer){
  //Dummy conversion z <-> station  
  StatusCode sc = Pos2StChamberPointer(x,y,getStation(z),chamberPointer);
  return sc;
}


StatusCode DeMuonDetector::Pos2ChamberNumber(const double x,
					     const double y,
					     const double z,
					     int & chamberNumber, int& regNum){
  StatusCode sc = StatusCode::FAILURE;
  //Z is know/provided.
  HepPoint3D hitPoint(x,y,z);   int sta = getStation(z);

  sc = Hit2ChamberNumber(hitPoint,sta,chamberNumber,regNum);

  return sc;
}



StatusCode DeMuonDetector::Pos2GapNumber(const double x,
					 const double y,
					 const double z, int & gapNumber,
					 int & chamberNumber, int& regNum){
  StatusCode sc = StatusCode::FAILURE;
  //Z is know/provided.
  HepPoint3D hitPoint(x,y,z);   int sta = getStation(z);

  sc = Hit2GapNumber(hitPoint,sta,gapNumber,chamberNumber,regNum);

  return sc;
}


StatusCode DeMuonDetector::Pos2ChamberTile(const double x,
					   const double y,
					   const double z,
					   MuonTileID& tile){
  StatusCode sc = StatusCode::SUCCESS;
  int dumChmb(-1), reg(-1);

  //Return the chamber number
  sc = Pos2ChamberNumber(x,y,z,dumChmb,reg);

  //Convert chamber number into a tile
  tile = m_chamberLayout.tileChamberNumber(getStation(z),reg,dumChmb);

  return sc;
}

IDetectorElement* DeMuonDetector::ReturnADetElement(int lsta, int lreg, 
                                                    int lchm) {
  
  IDetectorElement* myDet = (IDetectorElement*)0;
  int msta(0),mreg(0),mchm(0); 
  
  //Getting stations
  IDetectorElement::IDEContainer::iterator itSt=this->childBegin();
  for(itSt=this->childBegin(); itSt<this->childEnd(); itSt++){
    if((msta == lsta) && (lreg == -1) && (lchm == -1)) {
      myDet = *itSt;
      return myDet;
    } 
    //Getting regions
    mreg = 0;
    IDetectorElement::IDEContainer::iterator itRg=(*itSt)->childBegin();
    for(itRg=(*itSt)->childBegin(); itRg<(*itSt)->childEnd(); itRg++){
      if((msta == lsta) && (mreg == lreg) && (lchm == -1)) {
        myDet = *itRg;
        return myDet;
      } 
      //Getting chambers
      mchm = 0;
      IDetectorElement::IDEContainer::iterator itCh=(*itRg)->childBegin();
      for(itCh=(*itRg)->childBegin(); itCh<(*itRg)->childEnd(); itCh++){
        
        if((msta == lsta) && (mreg == lreg) && (mchm == lchm)) {
          myDet = *itCh;
          return myDet;
        } 
        mchm++;
      }//Chamber Loop
      mreg++;
    }//Region Loop
    msta++;
  }//Stations Loop
  
  return myDet;
  
}

void DeMuonDetector::CountDetEls() {
  
  IDetectorElement* myDet = (IDetectorElement*)0;
  int msta(0),mreg(0),mallreg(0); 

  //Getting stations
  IDetectorElement::IDEContainer::iterator itSt=this->childBegin();
  for(itSt=this->childBegin(); itSt<this->childEnd(); itSt++){

    myDet = *itSt;
    if(myDet) msta++;

    //Getting regions
    mreg = 0;
    IDetectorElement::IDEContainer::iterator itRg=(*itSt)->childBegin();
    for(itRg=(*itSt)->childBegin(); itRg<(*itSt)->childEnd(); itRg++){
      
      myDet = *itRg;
      if(myDet) {
        mreg++;
        mallreg++;
      }

    }//Region Loop

    m_regsperSta[msta-1] = mreg;

  }//Stations Loop

  m_stations = msta;
  m_regions = mallreg;

  return;
  
}

DeMuonChamber* DeMuonDetector::getChmbPtr(const int station, const int region,
			    const int chmb) {
  DeMuonChamber* myPtr;
  int encode, re(0);
  encode = 276*station+chmb;
  if(region) {
    re = region;
    while(re >= 1) {encode += MaxRegions[re-1]; re--;}
  }
  myPtr = m_ChmbPtr.at(encode);
  return myPtr;
}

void DeMuonDetector::fillChmbPtr() {
  
  int encode(0);
  bool debug = false;
  //Stations
  for(int iS = 0; iS<5; iS++){ 
    //Regions
    for(int iR = 0; iR<4; iR++){ 
      //Chambers
      for(int iC = 0; iC<MaxRegions[iR]; iC++){ 
        
        IDetectorElement* det  = ReturnADetElement(iS,iR,iC);
        DeMuonChamber*  myPtr =  dynamic_cast<DeMuonChamber*>( det ) ;
        
        if(debug) std::cout<<"Filling chamber Pointer: "<<encode<<" "
                           <<iS<<" "<<MaxRegions[iR]<<" "<<iR<<" "<<iC<<
                    std::endl;
        if(myPtr) {
          m_ChmbPtr.at(encode) = myPtr;
        } else {
          m_ChmbPtr.at(encode) = (DeMuonChamber*)0;
        }
        encode++;
      }
    }
  }
  if(debug)  std::cout<<" Filled the chamber pointer "<<std::endl;
  return;
}


std::vector< std::pair<MuonFrontEndID, std::vector<float> > > 
DeMuonDetector::listOfPhysChannels(HepPoint3D my_entry, HepPoint3D my_exit, 
                                   int region, int chamber) {
  
  //Pair vector;  
  std::vector< std::pair<MuonFrontEndID, std::vector<float> > > tmpPair;
  
  int regNum(0),chamberNumber(0);
  int station = getStation(my_entry.z());
  int regNum1(0),chamberNumber1(0);
  int station1 = getStation(my_exit.z());
  
  MsgStream msg( msgSvc(), name() );
  
  if((region == -1) || (chamber == -1)) {
    //Hit entry
    Hit2ChamberNumber(my_entry,station,chamberNumber,regNum);
    
    //Hit exit
    Hit2ChamberNumber(my_exit,station1,chamberNumber1,regNum1);
    
    if(chamberNumber != chamberNumber1 || regNum != regNum1) { 
      msg << MSG::ERROR <<
        "Hit entry and exit are in different chambers! Returning a void list."
          <<endreq; 
      return tmpPair;
    }
  } else {
    //If given.. assign region and chamber number
    regNum = region; chamberNumber = chamber;
  }
  
  //Getting the chamber pointer.
  DeMuonChamber*  myChPtr =  getChmbPtr(station,regNum,chamberNumber) ;

  bool isIn = false;
  //Getting a gap [gaps in same chamber have same dimensions]
  DeMuonGasGap*  myGap = (DeMuonGasGap*) 0; int gapCnt(0);
  IDetectorElement::IDEContainer::iterator itGap=myChPtr->childBegin();
  for(itGap=myChPtr->childBegin(); itGap<myChPtr->childEnd(); itGap++){
    myGap =  dynamic_cast<DeMuonGasGap*>( *itGap ) ;

    //Providing all 3 numbers identifies a chamber
    IGeometryInfo* geoGap = (*itGap)->geometry();  
    
    //Is the gap containing the hit?
    isIn = geoGap->isInside(my_entry);
    if(isIn) break;

    gapCnt++;
  }

  if(!myGap) {
    msg << MSG::ERROR <<"Could not find the gap. Returning a void list."<<endreq; 
    return tmpPair;
  }

  //Gap Geometry info  
  IGeometryInfo*  geoCh=myGap->geometry();

  //Retrieve the chamber box dimensions  
  const SolidBox *box = dynamic_cast<const SolidBox *>
    (geoCh->lvolume()->solid());
  float dx = box->xHalfLength();  float dy = box->yHalfLength();
  
  //Refer the distances to Local system [should be the gap]
  HepPoint3D new_entry = geoCh->toLocal(my_entry);
  HepPoint3D new_exit  = geoCh->toLocal(my_exit);

  //Define relative dimensions
  float mod_xen, mod_yen, mod_xex, mod_yex;
  if( dx && dy ) {
    mod_xen = (new_entry.x()+dx)/(2*dx);
    mod_yen = (new_entry.y()+dy)/(2*dy);
    mod_xex = (new_exit.x()+dx)/(2*dx);
    mod_yex = (new_exit.y()+dy)/(2*dy);
  } else {
    msg << MSG::ERROR <<"Null chamber dimensions. Returning a void list."<<
      endreq; 
    return tmpPair;
  }

  //Getting the grid pointer
  Condition* aGrid = myChPtr->condition((myChPtr->getGridName()).data());
  MuonChamberGrid* theGrid = dynamic_cast<MuonChamberGrid*>(aGrid);

  //Convert relative distances into absolute ones
  std::vector< std::pair<MuonFrontEndID, std::vector<float> > > myPair;
  std::vector< std::pair< MuonFrontEndID,std::vector<float> > >::iterator 
    tmpPair_it;
  std::vector<float> myVec; MuonFrontEndID myFE;
  if(theGrid) {
    //Gets list of channels
    tmpPair = theGrid->listOfPhysChannels(mod_xen,mod_yen,mod_xex,mod_yex);
    
    tmpPair_it = tmpPair.begin();
    for(tmpPair_it = tmpPair.begin();tmpPair_it<tmpPair.end(); tmpPair_it++){
      myFE  = tmpPair_it->first;
      myVec = tmpPair_it->second;
      myFE.setLayer(gapCnt/2);

      for(int iDm = 0; iDm<4; iDm++){  
        myVec.at(iDm) = iDm%2 ? myVec.at(iDm)*2*dy : myVec.at(iDm)*2*dx;
        //Added resolution effect
        if(fabs(myVec.at(iDm)) < 0.0001) myVec.at(iDm) = 0;
      }
      myPair.push_back(std::pair< MuonFrontEndID,std::vector<float> >
                       (myFE,myVec));
    }
  } else {
    msg << MSG::ERROR <<"No grid found. Returning a void list."<<endreq; 
    return tmpPair;
  }
  return myPair;
}

StatusCode DeMuonDetector::Tile2XYZ(MuonTileID tile, 
				    double & x, double & dx,
				    double & y, double & dy, 
				    double & z, double & dz){
  
  StatusCode sc = StatusCode::FAILURE;

  //Ask the chamber Layout about the tile.
  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG <<"Calling Tile2XYZpos method!"<<endreq; 

  sc = m_chamberLayout.Tile2XYZpos(tile,x,dx,y,dy,z,dz);

  return sc;
}

StatusCode DeMuonDetector::getPCCenter(MuonFrontEndID fe,int chamber,
                                       int station,int region,
                                       double& xcenter, double& ycenter,
                                       double& zcenter)
{
  //  MsgStream msg( msgSvc(), name() );

  DeMuonChamber*  myChPtr =  getChmbPtr(station,region,chamber) ;

  DeMuonGasGap*  myGap = NULL;
  IDetectorElement::IDEContainer::iterator itGap=myChPtr->childBegin();
    myGap= dynamic_cast<DeMuonGasGap*>(*itGap);  
  //Gap Geometry info  
  IGeometryInfo*  geoCh=myGap->geometry();
  //Retrieve the chamber box dimensions  
  const SolidBox *box = dynamic_cast<const SolidBox *>
    (geoCh->lvolume()->solid());
  float dx = box->xHalfLength();  
  float dy = box->yHalfLength();
  Condition* aGrid = 
  myChPtr->condition((myChPtr->getGridName()).data());
  MuonChamberGrid* theGrid = dynamic_cast<MuonChamberGrid*>(aGrid);
  double xcenter_norma=-1;
  double ycenter_norma=-1;  
  theGrid->getPCCenter(fe,xcenter_norma,ycenter_norma);
  double xcenter_gap=xcenter_norma*2*dx-dx;
  double ycenter_gap=ycenter_norma*2*dy-dy;
  unsigned int layer=fe.getLayer();
  HepPoint3D loc(xcenter_gap,ycenter_gap,0);   
  HepPoint3D glob= geoCh->toGlobal(loc);
  xcenter=glob.x();
  ycenter=glob.y();
  zcenter=glob.z();
  return StatusCode::SUCCESS;
  
}

StatusCode  DeMuonDetector::Chamber2Tile(int  chaNum, int station, int region, 
                                         MuonTileID& tile)
{
  StatusCode sc = StatusCode::SUCCESS; 
  //Convert chamber number into a tile
  tile = m_chamberLayout.tileChamberNumber(station,region,chaNum);  
  return sc;
};

StatusCode  DeMuonDetector::fillGeoInfo()
{
  MsgStream msg( msgSvc(), name() );
  bool debug=false;
  
  StatusCode sc = StatusCode::SUCCESS; 
  IDetectorElement::IDEContainer::iterator itSt=this->childBegin();
  int station=0;
  int region=0;
  
  for(itSt=this->childBegin(); itSt<this->childEnd(); itSt++){
      IDetectorElement::IDEContainer::iterator itRg=(*itSt)->childBegin();
      region=0;      
      if(debug)msg<<MSG::INFO<<" station "<<station<<endreq;      
      for(itRg=(*itSt)->childBegin(); itRg<(*itSt)->childEnd(); itRg++){
        if(debug)msg<<MSG::INFO<<" region "<<region<<endreq;      
        IDetectorElement::IDEContainer::iterator itCh=(*itRg)->childBegin();
        DeMuonRegion* reg=dynamic_cast<DeMuonRegion*> (*itRg);
        for(itCh=(*itRg)->childBegin(); itCh<(*itRg)->childEnd(); itCh++){
          DeMuonChamber* chPt=dynamic_cast<DeMuonChamber*> (*itCh);          
          IDetectorElement::IDEContainer::iterator itGap=(*itCh)->childBegin();
          int gaps=0;          
          double area=0;          
          DeMuonGasGap* 
            myGap= dynamic_cast<DeMuonGasGap*>(*((*itCh)->childBegin()));  
          //Gap Geometry info  
          IGeometryInfo*  geoCh=myGap->geometry();
          
          //Retrieve the chamber box dimensions  
          const SolidBox *box = dynamic_cast<const SolidBox *>
            (geoCh->lvolume()->solid());
          float dx = box->xHalfLength();
          float dy = box->yHalfLength();
          area=4*dx*dy;
          m_areaChamber[station*4+region]=area;          
          for(itGap=(*itCh)->childBegin(); itGap<(*itCh)->childEnd(); itGap++){
            gaps++;            
          }
          m_gapPerRegion[station*4+region]=gaps;
          m_gapPerFE[station*4+region]=gaps/2; 
          SmartRef<Condition> aGrid = (chPt)->condition(chPt->getGridName());
          Condition* bGrid = (chPt)->condition((chPt->getGridName()).data());

          std::vector<std::string>::iterator is;
          MuonChamberGrid* theGrid = dynamic_cast<MuonChamberGrid*>(bGrid);
          int nreadout=1;
          if(theGrid->getGrid2SizeY()>1)nreadout=2;
          m_readoutNumber[station*4+region]=nreadout;
          for( int i = 0; i<nreadout;i++){
            if(i==0){
              m_phChannelNX[i][station*4+region]=theGrid->getGrid1SizeX();
              m_phChannelNY[i][station*4+region]=theGrid->getGrid1SizeY();
              m_readoutType[i][station*4+region]=
                (theGrid->getReadoutGrid())[i];
            }            
            if(i==1){
              m_phChannelNX[i][station*4+region]=theGrid->getGrid2SizeX();
              m_phChannelNY[i][station*4+region]=theGrid->getGrid2SizeY();
              m_readoutType[i][station*4+region]=
                (theGrid->getReadoutGrid())[i];
            }
          }
          int maps=(theGrid->getMapGrid()).size()/2;
          m_LogMapPerRegion[station*4+region]=maps;
          for( int i = 0; i<maps;i++){
            if(nreadout==1&&maps==1)m_LogMapRType[0][station*4+region]=
                                      m_readoutType[i][station*4+region];
            if(maps==2){
              m_LogMapRType[i][station*4+region]=
                m_readoutType[i][station*4+region];
              m_LogMapRType[i][station*4+region]=
                m_readoutType[i][station*4+region];
            }
            m_LogMapMergex[i][station*4+region]=
              (theGrid->getMapGrid())[i*2];
            m_LogMapMergey[i][station*4+region]=
              (theGrid->getMapGrid())[i*2+1];            
          }          
          break;          
        }
        int chamber=0;        
        for(itCh=(*itRg)->childBegin(); itCh<(*itRg)->childEnd(); itCh++){
          chamber++;          
        }m_chamberPerRegion[station*4+region]=chamber;        
        region++;
      }
      station++;      
  }
  // initialization by hand of the logical layout in th edifferent regions
  m_layoutX[0][0]=24;  
  m_layoutX[0][1]=24;  
  m_layoutX[0][2]=24;  
  m_layoutX[0][3]=24;
  m_layoutX[0][4]=48; 
  m_layoutX[0][5]=48;  
  m_layoutX[0][6]=48;  
  m_layoutX[0][7]=48;
  m_layoutX[1][4]=8; 
  m_layoutX[1][5]=4;  
  m_layoutX[1][6]=2;  
  m_layoutX[1][7]=2;
  m_layoutX[0][8]=48;  
  m_layoutX[0][9]=48;  
  m_layoutX[0][10]=48;  
  m_layoutX[0][11]=48;
  m_layoutX[1][8]=8; 
  m_layoutX[1][9]=4;  
  m_layoutX[1][10]=2;  
  m_layoutX[1][11]=2;
  m_layoutX[0][12]=12;  
  m_layoutX[0][13]=12;  
  m_layoutX[0][14]=12;  
  m_layoutX[0][15]=12;  
  m_layoutX[1][13]=4;  
  m_layoutX[1][14]=2;  
  m_layoutX[1][15]=2;
  m_layoutX[0][16]=12;  
  m_layoutX[0][17]=12;  
  m_layoutX[0][18]=12;  
  m_layoutX[0][19]=12;
  m_layoutX[1][17]=4;  
  m_layoutX[1][18]=2;  
  m_layoutX[1][19]=2;  
  
  m_layoutY[0][0]=8;  
  m_layoutY[0][1]=8;  
  m_layoutY[0][2]=8;  
  m_layoutY[0][3]=8;
  m_layoutY[0][4]=1; 
  m_layoutY[0][5]=2;  
  m_layoutY[0][6]=2;  
  m_layoutY[0][7]=2;
  m_layoutY[1][4]=8; 
  m_layoutY[1][5]=8;  
  m_layoutY[1][6]=8;  
  m_layoutY[1][7]=8;
  m_layoutY[0][8]=1;  
  m_layoutY[0][9]=2;  
  m_layoutY[0][10]=2;  
  m_layoutY[0][11]=2;
  m_layoutY[1][8]=8; 
  m_layoutY[1][9]=8;  
  m_layoutY[1][10]=8;  
  m_layoutY[1][11]=8;
  m_layoutY[0][12]=8;  
  m_layoutY[0][13]=2;  
  m_layoutY[0][14]=2;  
  m_layoutY[0][15]=2;  
  m_layoutY[1][13]=8;  
  m_layoutY[1][14]=8;  
  m_layoutY[1][15]=8;
  m_layoutY[0][16]=8;  
  m_layoutY[0][17]=2;  
  m_layoutY[0][18]=2;  
  m_layoutY[0][19]=2;
  m_layoutY[1][17]=8;  
  m_layoutY[1][18]=8;  
  m_layoutY[1][19]=8;
  return sc;
};


