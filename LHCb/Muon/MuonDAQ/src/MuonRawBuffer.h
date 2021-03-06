// $Id: MuonRawBuffer.h,v 1.12 2008-10-23 13:42:17 asatta Exp $
#ifndef MUONRAWBUFFER_H 
#define MUONRAWBUFFER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "MuonDAQ/IMuonRawBuffer.h"            // Interface
#include "Event/RawEvent.h"
#include "MuonDet/DeMuonDetector.h"
#include "GaudiKernel/IIncidentListener.h"
#include "Event/MuonODEData.h"
#include "Event/MuonPPEventInfo.h"
#include "DAQKernel/DecoderToolBase.h"

#include <string>

/** @class MuonRawBuffer MuonRawBuffer.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2005-10-18
 */
class MuonRawBuffer :  public Decoder::ToolBase, virtual public IMuonRawBuffer, virtual public IIncidentListener
{
public: 
  /// Standard constructor
  MuonRawBuffer( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);
  
  virtual ~MuonRawBuffer( ); ///< Destructor
  StatusCode initialize();
  StatusCode finalize();
  StatusCode getTile(std::vector<LHCb::MuonTileID>& tile);
  StatusCode getTileAndTDC(std::vector<std::pair<LHCb::MuonTileID,unsigned int> > & tileAndTDC);
  StatusCode getPads(std::vector<LHCb::MuonTileID>& pads);

  StatusCode getNZSupp(std::vector<std::pair<LHCb::MuonTileID,unsigned int> > & tileAndTDC);
  // to get muon data belonging to one particular raw bank
private:
  StatusCode getTile(const LHCb::RawBank* r,std::vector<LHCb::MuonTileID>& tile);
  StatusCode getTileAndTDC(const LHCb::RawBank* r,std::vector<std::pair<LHCb::MuonTileID,unsigned int> > & tileAndTDC);
  StatusCode getPads(const LHCb::RawBank* r,std::vector<LHCb::MuonTileID>& pads);
public: 
  StatusCode getTile( LHCb::RawEvent* raw,std::vector<LHCb::MuonTileID>& tile,const std::string& offset);
  StatusCode getTileAndTDC( LHCb::RawEvent* raw,std::vector<std::pair<LHCb::MuonTileID,unsigned int> > & tileAndTDC,const std::string& offset);
  StatusCode getPads( LHCb::RawEvent* raw,std::vector<LHCb::MuonTileID>& pads,const std::string& offset);
  StatusCode getNZSupp( LHCb::RawEvent* raw,std::vector<std::pair<LHCb::MuonTileID,unsigned int> > & tileAndTDC,const std::string& offset);
  StatusCode getNZSupp(const LHCb::RawBank* r,std::vector<std::pair<LHCb::MuonTileID,unsigned int> > & tileAndTDC);
  StatusCode dumpNZSupp(const LHCb::RawBank* r,unsigned int ode_num);
  MuonPPEventInfo getPPInfo(const LHCb::RawBank* r,unsigned int pp_num);
  MuonPPEventInfo getPPInfo(unsigned int tell1Number,unsigned int pp_num);
  //StatusCode getTDCInODE();
 
  StatusCode getPads( int tell1,std::vector<LHCb::MuonTileID>& pads);
  StatusCode getPads( int tell1);
  StatusCode getPadsInStation( int station,std::vector<std::vector<LHCb::MuonTileID>* > & pads);
  

  void forceReset()
  {
    clearData();
  };
  MuonODEData getODEData(const LHCb::RawBank* r,unsigned int link);
  unsigned int BXCounter(unsigned int tell1Number);
  bool PPReachedHitLimit(unsigned int Tell1Number,int pp_num);
  bool LinkReachedHitLimit(unsigned int Tell1Number,int link_num);
private:
  MuonTell1Header getHeader(const LHCb::RawBank* r);
public:
  LHCb::RawBankReadoutStatus status(){return m_status;};
  void putStatusOnTES();

  std::vector<std::pair<MuonTell1Header, unsigned int> > getHeaders(LHCb::RawEvent* raw);
  std::vector<std::pair<MuonTell1Header, unsigned int> > getHeaders();
  
  
protected:

private:

  StatusCode  decodeTileAndTDCDC06(const LHCb::RawBank* r);
  StatusCode  decodeTileAndTDCV1(const LHCb::RawBank* r);
  void clearData(); 
  virtual void handle ( const Incident& incident );
  StatusCode decodePadsDC06(const LHCb::RawBank* r);
  StatusCode decodePadsV1(const LHCb::RawBank* r);
  StatusCode decodeNZSupp(const LHCb::RawBank* r);
  StatusCode decodeNZSupp(int tell1Number);
  StatusCode DecodeDataPad(const LHCb::RawBank* r);
  
  StatusCode DecodeData(const LHCb::RawBank* r);
  StatusCode checkBankSize(const LHCb::RawBank* rawdata);
  StatusCode checkAllHeaders(LHCb::RawEvent* raw);
  void fillTell1Header(unsigned int tell1,unsigned int data);
  void setTESOffset(const std::string& offset="");
  void restoreTESOffset();
  void initStatus();	  
  void initStatusNZS();	  

  unsigned int m_NLink;
  unsigned int m_ODEWord;
  unsigned int m_M1Tell1;
  DeMuonDetector* m_muonDet;
  mutable std::vector<std::pair<LHCb::MuonTileID, unsigned int> > m_storage[MuonDAQHelper_maxTell1Number];
  bool m_alreadyDecoded[MuonDAQHelper_maxTell1Number];
  mutable std::vector<LHCb::MuonTileID> m_padStorage[MuonDAQHelper_maxTell1Number];
  bool m_padAlreadyDecoded[MuonDAQHelper_maxTell1Number];
  bool m_ODEAlreadyDecoded[MuonDAQHelper_maxTell1Number];
  bool m_already_decoded_headerTell1[MuonDAQHelper_maxTell1Number];
  bool m_checkTell1HeaderPerformed;
  bool m_checkTell1HeaderResult;
  
  //mutable MuonTell1Header m_Tell1Header[[MuonDAQHelper_maxTell1Number];
  mutable MuonODEData m_ODEData[MuonDAQHelper_maxTell1Number*24];
  mutable MuonPPEventInfo m_PPEventInfo[MuonDAQHelper_maxTell1Number*4];
                                        
  mutable short m_eventHeader[MuonDAQHelper_maxTell1Number];
  unsigned int m_counter_invalid_hit[MuonDAQHelper_maxTell1Number];
  unsigned int m_processed_bank[MuonDAQHelper_maxTell1Number];
  unsigned int m_NZScounter_invalid_hit[MuonDAQHelper_maxTell1Number];
  unsigned int m_NZSprocessed_bank[MuonDAQHelper_maxTell1Number];
  unsigned int m_hitNumInLink[MuonDAQHelper_maxTell1Number*24];
  unsigned int m_hitNumInPP[MuonDAQHelper_maxTell1Number*4];
  unsigned int m_hit_checkSize[  MuonDAQHelper_maxTell1Number];
  unsigned int m_pad_checkSize[  MuonDAQHelper_maxTell1Number];
  unsigned int m_tell1_header_error[  MuonDAQHelper_maxTell1Number];
  unsigned int m_tell1_header_ORODE_error[  MuonDAQHelper_maxTell1Number];
  unsigned int m_tell1_header_SYNCH_data_error[  MuonDAQHelper_maxTell1Number];
  unsigned int m_tell1_header_SYNCH_BC_error[  MuonDAQHelper_maxTell1Number];
  unsigned int m_tell1_header_SYNCH_Evt_error[  MuonDAQHelper_maxTell1Number];
  std::string m_offsetForTES;
  bool m_TESChanged;
  std::string m_storeOriginalValue;
  
  LHCb::RawBankReadoutStatus m_status;
  LHCb::RawBankReadoutStatus m_statusFull;
  bool m_statusCreated;
  
  
};
#endif // MUONRAWBUFFER_H
