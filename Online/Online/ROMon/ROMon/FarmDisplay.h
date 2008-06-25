// $Id: FarmDisplay.h,v 1.3 2008-06-25 22:53:08 frankb Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Base class for displays for readout monitoring 
//               in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/FarmDisplay.h,v 1.3 2008-06-25 22:53:08 frankb Exp $
#ifndef ROMON_FARMDISPLAY_H
#define ROMON_FARMDISPLAY_H 1

// Framework includes
#include "ROMon/ROMonDisplay.h"
#include "CPP/Interactor.h"

// C++ include files
#include <map>

class Interactor;
namespace SCR {
  struct Display;
  struct Pasteboard;
}
/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  class ProcessDisplay;
  class SubfarmDisplay;
  class FarmDisplay;
  class ProcFarm;
  class CPUfarm;

  enum { 
    CMD_ADD = 100,
    CMD_CONNECT,
    CMD_CHECK,
    CMD_DELETE,
    CMD_UPDATE,
    CMD_SETCURSOR,
    CMD_LAST
  };


  /**@class InternalDisplay ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   Display base class handling standard actions.
   *
   *   @author M.Frank
   */
  class InternalDisplay   {
  protected:
    SCR::Pasteboard*  m_pasteboard;
    SCR::Display*     m_display;
    FarmDisplay*      m_parent;
    std::string       m_name;
    std::string       m_title;
    int               m_svc;
    time_t            m_lastUpdate;
  public:
    /// Access to pasteboard
    SCR::Pasteboard* pasteboard() const { return m_pasteboard; }
    /// Access to SCR display structure
    SCR::Display* display() const       { return m_display;    }
    /// Access to parent to send interrupts
    FarmDisplay* parent() const         { return m_parent;     }
    /// Access subfarm name
    const std::string& name() const     { return m_name;       }
    /// Access to last update time
    time_t lastUpdate() const           { return m_lastUpdate; }
    /// Initializing constructor
    InternalDisplay(FarmDisplay* parent, const std::string& title);
    /// Standard destructor
    virtual ~InternalDisplay();
    /// Draw bar to show occupancies
    size_t draw_bar(int x, int y, float f1, int scale);
    /// Show the display on the main panel
    void show(int row, int col);
    /// Hide the display from the panel
    void hide();
    /// Close the display
    void close();
    /// Disconnect from services: Only destructor may be called afterwards
    void disconnect();
    /// DIM command service callback
    static void dataHandler(void* tag, void* address, int* size);
    /// Update display content
    virtual void update(const void* data) = 0;
  };

  /**@class HelpDisplay ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   Help display. Dislay shows up on CTRL-h
   *   Help content is in $ROMONROOT/doc/farmMon.hlp
   *
   *   @author M.Frank
   */
  class HelpDisplay : public InternalDisplay {
  public:
    /// Initializing constructor
    HelpDisplay(FarmDisplay* parent, const std::string& title, const std::string& tag);
    /// Standard destructor
    virtual ~HelpDisplay() {}
    /// Update display content
    virtual void update(const void*) {}
  };


  /**@class FarmSubDisplay ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   Display summarizing one single subfarm. Showed as an array on the
   *   main display.
   *
   *   @author M.Frank
   */
  class FarmSubDisplay : public InternalDisplay  {
    int               m_evtBuilt;
    int               m_evtMoore;
    int               m_evtSent;
    int               m_totBuilt;
    int               m_totMoore;
    int               m_totSent;
    bool              m_hasProblems;
  public:
    /// Access to problem flag
    bool hasProblems() const { return m_hasProblems; }
    /// Set timeout error
    void setTimeoutError();
    /// Initializing constructor
    FarmSubDisplay(FarmDisplay* parent, const std::string& title, bool bad=false);
    /// Standard destructor
    virtual ~FarmSubDisplay();
    /// Initialize default display text
    void init(bool bad);
    /// Update display content
    virtual void update(const void* data);
    /// Update display content
    virtual void updateContent(const Nodeset& ns);
  };

  /**@class ProcessDisplay ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   Display showing all processes on a given node.
   *
   *   @author M.Frank
   */
  class ProcessDisplay : public InternalDisplay {
  public:
    /// Initializing constructor
    ProcessDisplay(FarmDisplay* parent, const std::string& title, int height=40,int width=120);
    /// Standard destructor
    virtual ~ProcessDisplay();
    /// Update display content
    virtual void update(const void* data);
    /// Update display content
    void updateContent(const ProcFarm& pf);
  };

  /**@class CPUDisplay ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   Display showing all processes on a given node.
   *
   *   @author M.Frank
   */
  class CPUDisplay : public InternalDisplay {
  public:
    /// Initializing constructor
    CPUDisplay(FarmDisplay* parent, const std::string& title, int height=40,int width=120);
    /// Standard destructor
    virtual ~CPUDisplay();
    /// Update display content
    virtual void update(const void* data);
    /// Update display content
    void updateContent(const CPUfarm& pf);
  };

  /**@class BufferDisplay ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   Internal MBM monitor display, when spying on individual nodes.
   *
   *   @author M.Frank
   */
  class BufferDisplay : public InternalDisplay {
  protected:
    /// Node number in node set
    int m_node;
  public:
    /// Initializing constructor
    BufferDisplay(FarmDisplay* parent, const std::string& title);
    /// Standard destructor
    virtual ~BufferDisplay() {}
    /// Set the node number for the display
    void setNode(int which) { m_node = which; }
    /// Update display content
    virtual void update(const void* data);
  };

  /**@class PartitionListener ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   Listen to RunInfo datapoints published by DIM
   *
   *   @author M.Frank
   */
  class PartitionListener {
  protected:
    Interactor* m_parent;
    std::string m_name;
    int         m_subFarmDP;
  public:
    /// Standard constructor with object setup through parameters
    PartitionListener(Interactor* parent,const std::string& name);
    /// Standard destructor
    virtual ~PartitionListener();
    /// DIM command service callback
    static void subFarmHandler(void* tag, void* address, int* size);
  };

  /**@class FarmDisplay ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   Monitoring display for the LHCb storage system.
   *
   *   @author M.Frank
   */
  class FarmDisplay : public InternalDisplay, public Interactor  {
  protected:
    typedef std::map<std::string, FarmSubDisplay*> SubDisplays;
    typedef std::vector<std::string> Farms;
    SubDisplays                      m_farmDisplays;
    std::auto_ptr<PartitionListener> m_listener;
    SubfarmDisplay*                  m_subfarmDisplay;
    std::auto_ptr<ProcessDisplay>    m_procDisplay;
    std::auto_ptr<BufferDisplay>     m_mbmDisplay;
    std::auto_ptr<HelpDisplay>       m_helpDisplay;
    std::auto_ptr<CPUDisplay>        m_cpuDisplay;

    std::string        m_partition;
    /// vector with all farm displays
    Farms              m_farms;
    int                m_height;
    int                m_width;
    int                m_dense;
    /// Cursor position in sub display array
    size_t             m_posCursor;
    size_t             m_subPosCursor;

    /// Keyboard rearm action
    static int key_rearm (unsigned int fac, void* param);
    /// Keyboard action
    static int key_action(unsigned int fac, void* param);

public:
    /// Standard constructor
    FarmDisplay(int argc, char** argv);

    /// Standard destructor
    virtual ~FarmDisplay();

    /// Show subfarm display
    int showSubfarm();

    /// Handle keyboard interrupts
    int handleKeyboard(int key);

    /// Get farm display from cursor position
    FarmSubDisplay* currentDisplay();

    SubDisplays& subDisplays() {  return m_farmDisplays; }

    /// Set cursor to position
    void set_cursor();

    /// Interactor overload: Display callback handler
    virtual void handle(const Event& ev);

    /// Connect to data sources
    void connect(const std::vector<std::string>& farms);

    /// DIM command service callback
    virtual void update(const void* data);
  };
}      // End namespace ROMon
#endif /* ROMON_FARMDISPLAY_H */

