// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/MDF/MDFIO.h,v 1.4 2006-09-25 12:49:51 frankb Exp $
//	====================================================================
//  MDFIO.h
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#ifndef DAQ_MDF_MDFIO_H
#define DAQ_MDF_MDFIO_H

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/StreamBuffer.h"

// forward declarations
class IMessageSvc;
class IDataProviderSvc;
namespace Gaudi { class IFileCatalogSvc; }

/*
 *    LHCb namespace
 */
namespace LHCb {

  class RawBank;
  class RawEvent;
  class MDFHeader;

  /**@class MDFIO MDFIO.h MDF/MDFIO.h
    *
    *
    *  @author  M.Frank
    *  @version 1.0
    */
  class MDFIO  {

  public:
    // Data type: banks from TES=1, 
    //            compressed records data from address = 2
    //            banks data from address (MDF bank first) = 3
    enum Writer_t { MDF_NONE=1, MDF_RECORDS=2, MDF_BANKS=3 };

  protected:
    /// Streambuffer to hold compressed data
    StreamBuffer            m_tmp;
    IMessageSvc*            m_msgSvc;
    IDataProviderSvc*       m_evtSvc;
    /// Input data type
    int                     m_type;
    std::string             m_parent;
    /// File catalog service name
    std::string                   m_catalogName;
    /// Pointer to catalog service if it should be used
    Gaudi::IFileCatalogSvc*       m_catalog;

    /// Helper to retrieve data from Opaque address
    std::pair<const char*,int> getDataFromAddress();
    /// Helper to access header information
    MDFHeader* getHeader();
    /// Setup IO object: set service pointers
    /** @param msg        [IN]    Reference to message service for error logging
      * @param svc        [IN]    Reference to data provider service to locate RawEvent
      */
    void setupMDFIO(IMessageSvc* msg, IDataProviderSvc* svc)  {
      m_msgSvc = msg;
      m_evtSvc = svc;
    }
    /// Set pointer to catalog service
    void setCatalog(Gaudi::IFileCatalogSvc* cat)   {
      m_catalog = cat;
    }
    /// Access to file catalog
    Gaudi::IFileCatalogSvc* catalog() const  {   return m_catalog;    }

    /// Check usage of catalog
    bool useCatalog()  const       {   return !m_catalogName.empty(); }

  public:

    /// Initializing constructor
    MDFIO(Writer_t typ, const std::string& nam) 
      : m_msgSvc(0), m_evtSvc(0), m_type(typ), m_catalog(0), m_parent(nam) {}

    /// Default destructor
    virtual ~MDFIO() {}

    /// Transform file name in presence of catalogs
    virtual std::string getConnection(const std::string& org_conn);

    /// Allocate space
    /** @param ioDesc     [IN]    Output IO descriptor       
      * @param len        [IN]    Total length of the data buffer
      *
      * @return  Pointer to allocated memory space
      */
    virtual std::pair<char*,int> getDataSpace(void* const ioDesc, size_t len) = 0;

    /// Write raw char buffer to output stream
    /** @param ioDesc     [IN]    Output IO descriptor       
      * @param data       [IN]    Data buffer to be streamed
      * @param len        [IN]    Total length of the data buffer
      *
      * @return  Status code indicating success or failure.
      */
    virtual StatusCode writeBuffer(void* const ioDesc, const void* data, size_t len);

    /// Write char buffer to output stream
    /** @param compTyp    [IN]    Compression type
      * @param chksumTyp  [IN]    Checksum type
      * @param ioDesc     [IN]    Output IO descriptor       
      * @param hdr        [IN]    Bank with MDF header structure as payload
      * @param data       [IN]    Data buffer to be streamed
      * @param len        [IN]    Total length of the data buffer
      *
      * Note:
      *     on writing, data contains empty space in front 
      *     of the size RawBank(MDF)::totalSize(),
      *     to this space either
      *       - the entire MDF Rawbank __or__ (m_type==MDF_BANKS)
      *       - the MDF header                (m_type==MDF_RECORDS)
      *     is copied depending on the value of m_type
      *
      * @return  Status code indicating success or failure.
      */
    virtual StatusCode writeDataSpace(int         compTyp, 
                                      int         chksumTyp, 
                                      void* const iodesc,
                                      RawBank*    hdr,
                                      char* const data,
                                      size_t      len);

    /// Commit raw banks to IO stream. -- Main entry point --
    /** @param compTyp    [IN]    Compression type
      * @param chksumTyp  [IN]    Checksum type
      * @param ioDesc     [IN]    Output IO descriptor       
      *
      * @return  Status code indicating success or failure.
      */
    virtual StatusCode commitRawBanks(int               compTyp,
                                      int               chksumTyp,
                                      void* const       ioDesc);

    /// Commit raw banks to IO stream. -- Main entry point --
    /** @param raw        [IN]    Pointer to RawEvent
      * @param hdr_bank   [IN]    Pointer to MDF header bank
      * @param compTyp    [IN]    Compression type
      * @param chksumTyp  [IN]    Checksum type
      * @param ioDesc     [IN]    Output IO descriptor       
      *
      * @return  Status code indicating success or failure.
      */
    virtual StatusCode commitRawBanks(RawEvent*         raw,
                                      RawBank*          hdr_bank,
                                      int               compTyp,
                                      int               chksumTyp,
                                      void* const       ioDesc);

    /// Direct I/O with valid existing raw buffers
    /** @param type       [IN]    raw buffer type (MDF_RECORDS, MDF_BANKS)
      * @param compTyp    [IN]    Compression type
      * @param chksumTyp  [IN]    Checksum type
      * @param ioDesc     [IN]    Output IO descriptor       
      *
      * @return  Status code indicating success or failure.
      */
    virtual StatusCode commitRawBuffer(int               type,
                                       int               compTyp,
                                       int               chksumTyp,
                                       void* const       ioDesc);

    /// Direct I/O with valid existing raw buffers
    /** @param data       [IN]    Reference to data location
      * @param len        [IN]    length of data block
      * @param type       [IN]    raw buffer type (MDF_RECORDS, MDF_BANKS)
      * @param compTyp    [IN]    Compression type
      * @param chksumTyp  [IN]    Checksum type
      * @param ioDesc     [IN]    Output IO descriptor       
      *
      * @return  Status code indicating success or failure.
      */
    virtual StatusCode commitRawBuffer(const void*       data,
                                       size_t            len,
                                       int               type,
                                       int               compTyp,
                                       int               chksumTyp,
                                       void* const       ioDesc);

    /// Read raw banks from IO stream
    /** @param data       [IN]    Reference to StreamBuffer for storing data
      * @param ioDesc     [IN]    Input IO descriptor       
      *
      * @return  Number of valid bytes in stream buffer (-1 on failure).
      */
    virtual std::pair<char*,int> readBanks(void* const ioDesc, bool dbg=false);

    /// Read raw char buffer from input stream
    /** @param ioDesc     [IN]    Input IO descriptor       
      * @param data       [IN]    Data buffer to reseive data
      * @param len        [IN]    Total length of data buffer to receive
      *
      * @return  Status code indicating success or failure.
      */
    virtual StatusCode readBuffer(    void* const       ioDesc,
                                      void* const       data,
                                      size_t            len);
                                      
  };
}      // end namespace LHCb
#endif /* DAQ_MDF_MDFIO_H */
