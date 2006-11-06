
//-------------------------------------------------------------------------------------
/** @file RichTrackCreatorFromRecoTracks.h
 *
 *  Header file for tool : RichTrackCreatorFromRecoTracks
 *
 *  CVS Log :-
 *  $Id: RichTrackCreatorFromRecoTracks.h,v 1.4 2006-11-06 18:24:45 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-------------------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichTrackCreatorFromRecoTracks_H
#define RICHRECTOOLS_RichTrackCreatorFromRecoTracks_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichTrackCreatorBase.h"

// interfaces
#include "RichRecBase/IRichExpectedTrackSignal.h"
#include "RichRecBase/IRichMassHypothesisRingCreator.h"
#include "RichKernel/IRichRayTracing.h"
#include "RichKernel/IRichSmartIDTool.h"
#include "RichRecBase/IRichTrSegMaker.h"

// Event
#include "Event/Track.h"

//-------------------------------------------------------------------------------------
/** @class RichTrackCreatorFromRecoTracks RichTrackCreatorFromRecoTracks.h
 *
 *  Tool for the creation and book-keeping of RichRecTrack objects.
 *  Uses reconstructed Tracks as the parent objects.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 *
 *  @todo Tidy up how the ray traced track HPD panel impact points are stored 
 *        in the RichRecSegment class
 */
//-------------------------------------------------------------------------------------

class RichTrackCreatorFromRecoTracks : public RichTrackCreatorBase
{

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichTrackCreatorFromRecoTracks( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent );

  /// Destructor
  virtual ~RichTrackCreatorFromRecoTracks() { }

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from public interface

  // Returns a RichRecTrack object pointer for given ContainedObject.
  // In this implementation the ContainedObject must be a reconstruction 'Track'.
  virtual LHCb::RichRecTrack * newTrack ( const ContainedObject * obj ) const;

  // Form all possible RichRecTracks from input Tracks
  virtual const StatusCode newTracks() const;

  /// Returns the number of tracks in the input Track container.
  virtual const long nInputTracks() const;

protected: // methods

  /// Initialise for a new event
  virtual void InitNewEvent();

private: // methods

  /// Returns a pointer to the Tracks
  const LHCb::Tracks * trTracks() const;

private: // data

  /// Pointer to Tracks
  mutable LHCb::Tracks * m_trTracks;

  /// Pointer to ring creator
  const IRichMassHypothesisRingCreator * m_massHypoRings;

  /// Pointer to track segment maker
  const IRichTrSegMaker * m_segMaker;

  /// Pointer to RichExpectedTrackSignal interface
  const IRichExpectedTrackSignal * m_signal;

  /// Input location of Tracks in TES
  std::string m_trTracksLocation;

  /// Job Option "nickname" of the TrackSegment tool to use
  std::string m_trSegToolNickName;

  // Flag to signify all tracks have been formed for current event
  mutable bool m_allDone;

  /// Flag to turn on the creation of the RichRecRings for the segment mass hypotheses
  bool m_buildHypoRings;

};

#endif // RICHRECTOOLS_RichTrackCreatorFromRecoTracks_H
