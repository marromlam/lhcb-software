#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/Algorithm.h"
#include "MDF/StreamDescriptor.h"
#include "MDF/MDFWriter.h"
#include "MDF/MDFIO.h"
#include "Writer/Connection.h"
#include "Writer/RPCComm.h"
#include "TMD5.h"

#include <map>

#ifndef MDFWRITERNET_H
#define MDFWRITERNET_H


static inline unsigned long adler32(unsigned long adler,
				    const char *buf,
				    unsigned int len);

/*
 * LHCb namespace
 */

namespace LHCb {

  /** @class File
   *  Represents an open file to which data are being streamed.
   *
   * @author: Sai Suman Cherukuwada
   * @version: 1.0
   */
  class File {
  private:
    /// The number of bytes written so far in the current file.
    size_t m_bytesWritten;

    /// The current file being written to.
    std::string m_fileName;

    /// The sequence number of the last command that was sent for this file.
    unsigned int m_seqNum;

    /// The Adler32 checksum of the file.
    unsigned int m_adler32;

    /// The MD5 checksum of the file.
    TMD5 *m_md5;

    /// The run number that this file is associated with.
    unsigned int m_runNumber;

    /// The last time a write was made to this file (for determining timeouts).
    time_t m_lastUpdated;

    /// A boolean to keep track of whether the file is open or closed.
    bool m_fileOpen;

    /// A link to the next element in the list.
    File *m_next;

    /// A link to the previous element in the list.
    File *m_prev;

  public:

    /// Increments the sequence number.
    inline void incSeqNum() { ++m_seqNum; }

    /// Returns the current sequence number.
    inline unsigned int getSeqNum() { return m_seqNum; }

    /// Sets the status of the current file to open.
    inline void open() { m_fileOpen = true; }

    /// Returns the number of bytes written to the file so far.
    inline size_t getBytesWritten() { return m_bytesWritten; }

    /// Returns the name of the file.
    inline std::string* getFileName() { return &m_fileName; }

    /// Returns the Adler32 checksum computed so far.
    inline int getAdlerChecksum() { return m_adler32; }

    /// Returns the MD5 checksum calculated so far.
    inline TMD5* getMD5Checksum() { return m_md5; }

    /// Returns the run number associated with this file.
    inline unsigned int getRunNumber() { return m_runNumber; }

    /// Returns how many seconds elapsed since the last update.
    inline time_t getTimeSinceLastWrite() { return time(NULL) - m_lastUpdated; }

    /// Returns true if the file is open, and false otherwise.
    inline bool isOpen() { return m_fileOpen; }

    /// Sets the next file after this one.
    inline void setNext(File *next) { m_next = next; }

    /// Sets the previous file before this one.
    inline void setPrev(File *prev) { m_prev = prev; }

    /// Gets the next file after this element.
    inline File* getNext() { return m_next; }

    /// Gets the previous element after this element.
    inline File* getPrev() { return m_prev; }

    /// Constructor
    File(std::string fileName, unsigned int runNumber) {
      m_seqNum = 0;
      m_fileName = fileName;
      m_runNumber = runNumber;
      m_md5 = new TMD5();
      m_adler32 = adler32(0, NULL, 0);
      m_lastUpdated = time(NULL);
      m_fileOpen = false;
      m_bytesWritten = 0;
      m_prev = NULL;
      m_next = NULL;
    }

    /// Updates the checksums and the bytes written count for the file, given a new chunk to be written.
    /// Returns the number of bytes written in all so far.
    inline size_t updateWrite(const void *data, size_t len) {
      m_md5->Update((UChar_t*)data, (UInt_t)len);
      m_adler32 = adler32(m_adler32, (const char*)data, len);
      m_bytesWritten += len;
      m_lastUpdated = time(NULL);
      return m_bytesWritten;
    }

    /// Destructor.
    ~File() {
      delete m_md5;
    }
  };

  /**
   * Need to define a custom list here because we need to modify elements on the fly.
   * Can't do that with STL iterators, and don't need anything other than sequential
   * searching because the list will at maximum be 3 elements long.
   */
  class FileList {
  private:
    File *m_top;
    File *m_bottom;

  public:
    FileList(void) { m_top = NULL; m_bottom = NULL; }

    void addFile(File *f)
    {
      if(!m_top) {
	m_top = f;
	m_bottom = f;
        f->setNext(NULL); 
	f->setPrev(NULL);
      } else {
	m_bottom->setNext(f);
	f->setPrev(m_bottom);
	f->setNext(NULL);
	m_bottom = f;
      }
    }

    File *getFile(unsigned int runNumber)
    {
      File *tmp = m_top;
      while(tmp) {
        if(tmp->getRunNumber() == runNumber)
          return tmp;
        tmp = tmp->getNext();
      }
      return NULL;
    }
    File* removeFile(File *f) 
    {
      if(f->getPrev() == NULL)
	m_top = f->getNext();
      else
	f->getPrev()->setNext(f->getNext());
      if(f->getNext() == NULL)
	m_bottom = f->getPrev();
      else
	f->getNext()->setPrev(f->getPrev());
      return f->getNext();
    }

    File *getFirstFile(void)
    {
      return m_top;
    }
  };


  /** @class MDFWriterNet
   *
   * @author: Sai Suman Cherukuwada
   * @version: 1.0
   */

  class MDFWriterNet : public MDFWriter, INotifyClient {
    typedef LHCb::Connection Connection;
  protected:
    /// The initial storage server hostname to connect to.
    std::string m_serverAddr;

    /// The port of the storage server to connect to.
    int m_serverPort;

    /// The RPC URL of the Run Database service.
    std::string m_runDBURL;

    /// A prefix to a apply to all file names that are created.
    std::string m_filePrefix;

    /// Directory name to apply to all file names that are created.
    std::string m_directory;

    /// File extension to apply to all file names that are created.
    std::string m_fileExtension;

    /// The maximum amount of seconds to wait for events after which a file is closed.
    int m_runFileTimeoutSeconds;

    /// The maximum file size (in MB) to write before creating a new file.
    size_t m_maxFileSizeMB;

    /// The socket send/receive buffer sizes.
    int m_sndRcvSizes;

    /// The current run number in progress.
    int m_currentRunNumber;

    /// Property: The stream identifier
    int m_streamID;

    /// A map of all current open files.
    FileList m_openFiles;

    /// The current open file.
    File *m_currFile;

    /// The connection object being used to talk to the server.
    Connection *m_srvConnection;

    /// The object that encapsulates all RPC communication.
    RPCComm *m_rpcObj;

    /// The message stream to log to.
    MsgStream *m_log;

    /** Generates a new file name from the MDF information.
     * @param runNumber  The current run number, to be included in the file name.
     */
    std::string getNewFileName(unsigned int runNumber);

    /// Similar to MDFWriter::construct()
    virtual void constructNet(void);

    /// Returns the run number from an MDF header.
    unsigned int getRunNumber(const void *data, size_t len);

    /// Returns a File object for the specified run number

  public:
    /// Destructor.
    virtual ~MDFWriterNet();

    /// Sends a CLOSE command to the server to close a file.
    void closeConnection();

    /// Overrides Algorithm::initialize()
    virtual StatusCode initialize();

    /// Overrides Algorith::finalize()
    virtual StatusCode finalize();

    /// Overrides Algorithm::execute()
    virtual StatusCode execute();

    /// Extended Algorithm constructor (delegates to MDFWriter constructor).
    MDFWriterNet(MDFIO::Writer_t typ, const std::string& nam, ISvcLocator* pSvc);

    /// Standard Algorithm constructor (delegates to MDFWriter constructor).
    MDFWriterNet(const std::string& nam, ISvcLocator* pSvc);

    /// Sends the command required to open a file and returns the File object.
    File *createAndOpenFile(unsigned int runNumber);

    /// Sends the command required to close a file given the file object.
    void closeFile(File *file);

    /// Writes a chunk to the storage cluster server. Overrides Algorithm::writeBuffer().
    virtual StatusCode writeBuffer(void *const ioDesc, const void *data, size_t len);

    /// Implemented from INotifyClient.
    virtual void notifyClose(struct cmd_header *cmd);

    /// Implemented from INotifyClient.
    virtual void notifyError(struct cmd_header *cmd, int err_no);

    /// Implemented from INotifyClient.
    virtual void notifyOpen(struct cmd_header *cmd);
  };
}

/* The standard Adler32 algorithm taken from the Linux kernel.*/

#define BASE 65521L /* largest prime smaller than 65536 */
#define NMAX 5552
/* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */

#define DO1(buf,i)  {s1 += buf[i]; s2 += s1;}
#define DO2(buf,i)  DO1(buf,i); DO1(buf,i+1);
#define DO4(buf,i)  DO2(buf,i); DO2(buf,i+2);
#define DO8(buf,i)  DO4(buf,i); DO4(buf,i+4);
#define DO16(buf)   DO8(buf,0); DO8(buf,8);
#define NMAX 5552
/* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */

static inline unsigned long adler32(unsigned long adler,
				    const char *buf,
				    unsigned int len)
{
  unsigned long s1 = adler & 0xffff;
  unsigned long s2 = (adler >> 16) & 0xffff;
  int k;

  if (buf == NULL) return 1L;

  while (len > 0) {
    k = len < NMAX ? len : NMAX;
    len -= k;
    while (k >= 16) {
      DO16(buf);
      buf += 16;
      k -= 16;
    }
    if (k != 0) do {
      s1 += *buf++;
      s2 += s1;
    } while (--k);
    s1 %= BASE;
    s2 %= BASE;
  }
  return (s2 << 16) | s1;
}


#endif //MDFWRITERNET_H

