// $Id: CellMatrix2x2.cpp,v 1.3 2004-10-22 09:29:08 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2001/11/22 16:02:34  ibelyaev
//  new utilities
//
// Revision 1.1  2001/11/07 17:24:49  ibelyaev
//  new utilities for determination of relative positions of cells
//
// ============================================================================
// Include files
// CaloDet
#include "CaloDet/DeCalorimeter.h"
// local
#include "CaloUtils/CellMatrix2x2.h"

// ============================================================================
/** @file CellMatrix2x2.cpp
 *
 *  Implementation file for class : CellMatrix2x2
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date  07/11/2001
 */
// ============================================================================

// ============================================================================
/** destructor
 */
// ============================================================================
CellMatrix2x2::~CellMatrix2x2() {}

// ============================================================================
/** if cells belong to different areas
 *  one need to treat them in a different way
 *  @param cell  the cell to be tested
 *  @return fraction of area of the second cell which fits inside
 *          3x3 matrix aroud the seed cell
 */
// ============================================================================
double CellMatrix2x2::treatDifferentAreas( const CaloCellID& seed ,
                                           const CaloCellID& cell ) const
{
  /// valid detector
  if( 0 == det() )
    { Exception("CellMatrix3x3::treatDifferentAreas: DeCalo=0!");}
  /// both cells are valid?
  if( !det()->valid( seed ) || !det()->valid( cell )  ) { return 0 ; }
  /// cell sizes
  const double      sizeMtrx   =  0.5  * det()->cellSize   ( seed ) ;
  const double      sizeCell   =  0.5  * det()->cellSize   ( cell ) ;
  /// cell centers
  const HepPoint3D& centerMtrx =         det()->cellCenter ( seed ) ;
  const HepPoint3D& centerCell =         det()->cellCenter ( cell ) ;
  /// cell area 
  const double      cellArea   = sizeCell * sizeCell * 4.0 ;
  /// effective halfsize
  const double      sizeMin    =  0.01 * mini ( sizeMtrx , sizeCell ) ;
  /// dispatch
  double Area = 0 ;
  switch( type() )
    {
    case LowerLeft :
      Area = area ( centerMtrx +
                    HepPoint3D( -1.0 * sizeMtrx , -1.0 * sizeMtrx , 0 ) ,
                    2.0 * sizeMtrx                                      ,
                    centerCell                                          ,
                    sizeCell                                            );
      ///
      return Area <= sizeMin ? 0 : Area / cellArea ;
      break;
    case LowerRight :
      Area = area ( centerMtrx +
                    HepPoint3D(        sizeMtrx , -1.0 * sizeMtrx , 0 ) ,
                    2.0 * sizeMtrx                                      ,
                    centerCell                                          ,
                    sizeCell                                            );
      ///
      return Area <= sizeMin ? 0 : Area / cellArea ;
      break;
    case UpperLeft :
      Area = area ( centerMtrx +
                    HepPoint3D( -1.0 * sizeMtrx ,        sizeMtrx , 0 ) ,
                    2.0 * sizeMtrx                                      ,
                    centerCell                                          ,
                    sizeCell                                            );
      ///
      return Area <= sizeMin ? 0 : Area / cellArea  ;
      break;
    case UpperRight :
      Area = area ( centerMtrx +
                    HepPoint3D(        sizeMtrx ,        sizeMtrx , 0 ) ,
                    2.0 * sizeMtrx                                      ,
                    centerCell                                          ,
                    sizeCell                                            );
      //
      return Area <= sizeMin ? 0 : Area / cellArea ;
      break;
    default:
      Exception("CellMatrix2x2: unimplemented case!");
      break;
    }
  ///
  return 0;
};

// ============================================================================
// The End
// ============================================================================

