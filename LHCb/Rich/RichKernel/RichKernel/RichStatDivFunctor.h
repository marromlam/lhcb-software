
//--------------------------------------------------------------------------------------
/** @file RichStatDivFunctor.h
 *
 *  Header file for poisson efficiency functor : RichStatDivFunctor
 *
 *  CVS Log :-
 *  $Id: RichStatDivFunctor.h,v 1.3 2005-10-31 13:26:11 jonrob Exp $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2003-09-08
 */
//--------------------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHSTATDIVFUNCTOR_H
#define RICHKERNEL_RICHSTATDIVFUNCTOR_H 1

// STL
#include <functional>

// Gaudi
#include "GaudiKernel/MsgStream.h"

class RichStatDivFunctor;

//--------------------------------------------------------------------------------------
/** @class RichStatDivFunctorResult RichStatDivFunctor.h RichKernel/RichStatDivFunctor.h
 *
 *  Result object for efficency and error calculator RichStatDivFunctor.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-20
 */
//--------------------------------------------------------------------------------------

class RichStatDivFunctorResult : private std::pair<double,double>
{

public:

  /** Default constructor
   *
   *  @param result The result of the calculation
   *  @param error  The error on the result
   *  @param parent Point to the parent calculator
   */
  RichStatDivFunctorResult( const double result,
                            const double error,
                            const RichStatDivFunctor * parent )
    : std::pair<double,double>(result,error),
      m_parent ( parent ) { }

  /// Access the result of the calculation
  inline double result() const { return this->first; }

  /// Access the error on the result of the calculation
  inline double error() const { return this->second; }

  /// Access the parent calculator
  inline const RichStatDivFunctor * parent() const { return m_parent; }

private:

  /// Pointer to parent calculator
  const RichStatDivFunctor * m_parent;

};

//------------------------------------------------------------------------------------
/** @class RichStatDivFunctor RichStatDivFunctor.h RichKernel/RichStatDivFunctor.h
 *
 *  Simple utility class to provide an easy way to produce a formated output division
 *  calculation, with the associated statistical error.
 *
 *  @code
 *  RichStatDivFunctor result("%8.2f +-%6.2f");
 *  info() << "Result = " << result(a,b) << endreq;
 *  @endcode
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-20
 */
//------------------------------------------------------------------------------------

class RichStatDivFunctor
  : public std::binary_function< const double,
                                 const double,
                                 RichStatDivFunctorResult >
{

public:

  /** Constructor with print format string
   *
   *  @param format The Printing format
   */
  RichStatDivFunctor( const std::string & format = "%8.2f +-%6.2f" )
    : m_format( format ) { }

  /** The efficiency calculation operator
   *
   *  @param top The numerator
   *  @param bot The denominator
   *
   *  @return The poisson efficiency and error
   */
  inline RichStatDivFunctorResult operator() ( const double top, 
                                               const double bot ) const
  {
    return RichStatDivFunctorResult( ( bot>0 ? top/bot       : 0 ),
                                     ( bot>0 ? sqrt(top)/bot : 0 ),
                                     this );
  }

  /** Access the print format
   *
   *  @return The print format string
   */
  inline const std::string & printFormat() const { return m_format; }

private:

  /// The print format
  std::string m_format;

};

/// overloaded output to MsgStream
inline MsgStream & operator << ( MsgStream & os,
                                 const RichStatDivFunctorResult & res )
{
  return os << format( res.parent()->printFormat().c_str(), res.result(), res.error() );
}

#endif // RICHKERNEL_RICHSTATDIVFUNCTOR_H
