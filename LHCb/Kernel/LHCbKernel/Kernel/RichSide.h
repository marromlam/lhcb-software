
//-----------------------------------------------------------------------------
/** @file RichSide.h
 *
 *  Header file for RICH particle ID enumeration : Rich::Side
 *
 *  CVS Log :-
 *  $Id: RichSide.h,v 1.10 2007-02-20 16:42:02 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------

#ifndef LHCBKERNEL_RICHSIDE_H
#define LHCBKERNEL_RICHSIDE_H 1

// Include files
#include <string>
#include <vector>
#include <iostream>

namespace Rich 
{

  /// Total number of HPD panels
  static const unsigned int NTotalHPDPanels = 4;

  /// Number of HPD panels per RICH detector
  static const unsigned int NHPDPanelsPerRICH = 2;

  /** @enum Rich::Side
   *
   *  RICH detector side enumeration. Used to indentify the "side" of
   *  each of the RICH detectors.
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  enum Side
    {
      InvalidSide  = -1, ///< Invalid side
      top          = 0,  ///< Upper panel in RICH1
      bottom       = 1,  ///< Lower panel in RICH1
      left         = 0,  ///< Left panel in RICH2
      right        = 1   ///< Right panel in RICH2
    };

  /** Text conversion for Rich::Side enumeration
   *
   *  @param side Rich Side enumeration
   *  @return Rich Side as an std::string
   */
  std::string text( const Rich::Side side );

  /// Std Vector typedef
  typedef std::vector<Side> RichSideVector;

  /// Shortcut to a list of Side Types
  typedef RichSideVector Sides;

  /// Access a vector of valid particle ID types
  const Sides & sides();

}

/// Implement textual ostream << method for Rich::Side enumeration
inline std::ostream& operator << ( std::ostream & s,
                                   const Rich::Side & side ) 
{
  return s << Rich::text( side );
}

#endif // LHCBKERNEL_RICHSIDE_H
