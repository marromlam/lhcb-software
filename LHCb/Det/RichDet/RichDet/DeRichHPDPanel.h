// $Id: DeRichHPDPanel.h,v 1.5 2003-06-20 14:34:32 papanest Exp $

#ifndef DERICHHPDPANEL_H
#define DERICHHPDPANEL_H 1


// Include files
#include "RichDet/DeRichPDPanel.h"

/** @class DeRichHPDPanel DeRichHPDPanel.h
 *
 * This is the definition of the Rich HPDPanel detector class
 *
 * @author Antonis Papanestis
 */
class DeRichHPDPanel: public  DeRichPDPanel {

public:
  /**
   * Constructor for this class
   */
  DeRichHPDPanel();

  /**
   * Default destructor
   */
  virtual ~DeRichHPDPanel();


  /**
   * This is where most of the geometry is read
   * @return StatusCode
   */
  virtual StatusCode initialize() = 0;


  /**
   * Returns a RichSmartID for a given point in global coordinates.
   * @return StatusCode
   */
  virtual StatusCode smartID( const HepPoint3D& globalPoint,
                              RichSmartID& id ) = 0;

  /**
   * Returns the detection point given a smartID. The point is on the inside
   * of the HPD window, on the photocathode.
   * @return StatusCode
   */
  virtual StatusCode detectionPoint( const RichSmartID& smartID,
                                     HepPoint3D& windowHitGlobal ) = 0; 
  /**
   * Returns the intersection point with an PD window given a vector
   * and a point.
   * @return StatusCode
   */

  virtual StatusCode PDWindowPoint( const HepVector3D& vGlobal, 
                                    const HepPoint3D& pGlobal, 
                                    HepPoint3D& windowPointGlobal, // return
                                    RichSmartID& smartID ) = 0;
  /**
   * Returns the detection plane of the PD panel, defined at the top of the
   * PDs (a plane resting on the PDs touching the window).
   * @return HepPlane3D
   */
  virtual HepPlane3D detectionPlane() const = 0;


  /**
   * Returns a list with all the available readout channels, in form of
   * RichSmartIDs.
   * @return StatusCode
   */
  virtual StatusCode readoutChannelList(std::vector<RichSmartID>&
                                        readoutChannels);


protected:

  virtual unsigned int PDRow(unsigned int PD) = 0;

  virtual unsigned int PDCol(unsigned int PD) = 0;

  int m_PDMax;
  double m_winR;
  double m_winRsq;
  
};

#endif    // DERICHHPDPANEL_H
