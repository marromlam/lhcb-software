
//-----------------------------------------------------------------------------
/** @file RichSmartIDCnv.h
 *
 *  Header file for RichSmartID conversion utilities
 *
 *  CVS Log :-
 *  $Id: RichSmartIDCnv.h,v 1.2 2008-02-07 16:45:25 jonrob Exp $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   05/02/2008
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_RichSmartIDCnv_H
#define RICHKERNEL_RichSmartIDCnv_H 1

// Kernel
#include "Kernel/RichSmartID.h"
#include "RichKernel/RichDAQDefinitions.h"

// forward declarations
class MsgStream;

namespace Rich
{

  /** @class SmartIDGlobalOrdering RichSmartIDCnv.h RichKernel/RichSmartIDCnv.h
   *
   *  Simple class to provide global ordering information for RichSmartIDs.
   *
   *  Useful to enable all hits for a given RICH detector to be drawn on a single
   *  figure.
   *
   *  Example Usage is :-
   *  @code
   *
   *  // Get a RichSmartID from somewhere
   *  LHCb::RichSmartID id = ....;
   *  // Make a local ordering object
   *  const Rich::SmartIDGlobalOrdering order(id);
   *  // Which RICH ?
   *  const std::string RICH = Rich::text( id.rich() );
   *
   *  // Draw plots showing all hits in each RICH
   *  plot2D( order.globalPixelX(), order.globalPixelY(),
   *          RICH, "Hit Map for "+RICH,
   *          order.minGlobalPixelX()-0.5,  order.maxGlobalPixelX()+0.5,
   *          order.minGlobalPixelY()-0.5,  order.maxGlobalPixelY()+0.5,
   *          order.totalNumInGlobalX()+1,  order.totalNumInGlobalY()+1 );
   *
   *  // Can also get the global HPD (x,y) numbers
   *  const int globalHPDX = order.globalHpdX();
   *  const int globalHPDY = order.globalHpdY();
   *
   *  // The constructor above automatically determines on the fly if the
   *  // data is in ALICE or LHCb mode, and adjust the returned data accordingly.
   *  // This normally will do what is correct. However, if you want you can
   *  // force a given mode using :-
   *  const Rich::SmartIDGlobalOrdering order(id,Rich::SmartIDGlobalOrdering::ALICEMode);
   *  const Rich::SmartIDGlobalOrdering order(id,Rich::SmartIDGlobalOrdering::LHCbMode);
   *
   *  @endcode
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   05/02/2008
   */
  class SmartIDGlobalOrdering
  {

  public: // definitions and constants

    /// The mode in operation to determine ALICE or LHCb data
    enum PixelMode
      {
        AutomaticMode = 0, ///< Automatically determine ALICE or LHCb mode from the RichSmartID (default)
        ALICEMode,         ///< Force ALICE mode (i.e. 256 pixel rows per HPD)
        LHCbMode           ///< Force LHCb mode  (i.e. 32  pixel rows per HPD)
      };

  public:

    /// Constructor from a RichSmartID
    explicit
    SmartIDGlobalOrdering( const LHCb::RichSmartID id,
                           const Rich::SmartIDGlobalOrdering::PixelMode mode
                           = Rich::SmartIDGlobalOrdering::AutomaticMode )
      : m_id ( id ), m_mode ( mode ) { }

    /// Access the RichSmartID this object represents
    inline LHCb::RichSmartID smartID() const { return m_id; }

    /// Access the data mode
    inline Rich::SmartIDGlobalOrdering::PixelMode pixelMode() const { return m_mode; }

  private:

    /** Number of HPD columns per HPD panel.
     *
     *  Note here 'column' is used in the RICH sense. I.e. a column is vertical
     *  in RICH2 but horizontal in RICH1.
     */
    inline unsigned int _nHPDCols() const
    {
      // Eventually these numbers should be in RichDAQDefinitions.h ??
      return ( Rich::Rich1 == smartID().rich() ? 7 : 9 );
    }

    /** Number of HPDs per column
     *
     *  Note here 'column' is used in the RICH sense. I.e. a column is vertical
     *  in RICH2 but horizontal in RICH1.
     */
    inline unsigned int _nHPDsPerCol() const
    {
      // Eventually these numbers should be in RichDAQDefinitions.h ??
      return ( Rich::Rich1 == smartID().rich() ? 14 : 16 );
    }

    /// Returns the number of pixel rows per HPD (depending on data mode)
    unsigned int _nPixelRowsPerHPD() const;

    /// Returns the number of pixel columns per HPD
    inline unsigned int _nPixelColsPerHPD() const
    {
      return ( Rich::DAQ::NumPixelColumns );
    }

    /// Returns the HPD column offset number
    unsigned int _panelHPDColOffset() const;

    /// Returns the 'correct' HPD pixel row number (ALICE or LHCb mode)
    int _pixelRowNum() const;

    /// Returns the 'global' HPD number in a RICH column
    inline int _numInCol() const { return _nHPDsPerCol() - 1 - smartID().hpdNumInCol(); }

    /// Returns the 'global' HPD column number
    int _hpdCol() const;

  public:

    /// Returns the minimum 'global' pixel column number
    inline int minGlobalPixelX() const
    {
      return ( 0 );
    }

    /// Returns the maximum 'global' pixel column number
    int maxGlobalPixelX() const;

    /// Returns the minimum 'global' pixel row number
    inline int minGlobalPixelY() const
    {
      return ( 0 );
    }

    /// Returns the maximum 'global' pixel row number
    int maxGlobalPixelY() const;

    /// The total number of 'global' pixel columns
    inline int totalNumInGlobalX() const
    {
      return maxGlobalPixelX() - minGlobalPixelX();
    }

    /// The total number of 'global' pixel rows
    inline int totalNumInGlobalY() const
    {
      return maxGlobalPixelY() - minGlobalPixelY();
    }

  public:

    /** Returns the HPD number in column 'offset'.
     *
     *  Used to get the stagger between neighbouring HPD columns
     */
    inline double globalHpdNumInColOffset() const
    {
      return (smartID().hpdCol()%2) * ( Rich::Rich2 == smartID().rich() ? 0.5 : -0.5 );
    }

    /// Returns a 'global' HPD column number
    inline int globalHpdX() const
    {
      return ( Rich::Rich2 == smartID().rich() ? _hpdCol() : _numInCol() );
    }

    /// Returns a 'global' HPD number in column number
    inline int globalHpdY() const
    {
      return ( Rich::Rich1 == smartID().rich() ? _hpdCol() : _numInCol() );
    }

    /// Returns a 'global' pixel column number
    int globalPixelX() const;

    /// Returns a 'global' pixel row number
    int globalPixelY() const;

  public:

    /// Overload output to MsgStream
    friend inline MsgStream& operator << ( MsgStream& os,
                                           const SmartIDGlobalOrdering & order )
    { return order.fillStream(os); }

  private:

    /// Print in a human readable way
    MsgStream& fillStream( MsgStream& os ) const;

  private:

    LHCb::RichSmartID m_id; ///< The RichSmartID
    PixelMode m_mode;       ///< The data mode (Forced ALICE or LHCb, or automatic based on the data)

  };

}

#endif // RICHKERNEL_RichSmartIDCnv_H
