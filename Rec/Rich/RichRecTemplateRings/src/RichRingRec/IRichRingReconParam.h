// $Id: IRichRingReconParam.h,v 1.5 2009-12-16 13:42:49 seaso Exp $
#ifndef RICHRINGREC_IRICHRINGRECONPARAM_H
#define RICHRINGREC_IRICHRINGRECONPARAM_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace Rich
{
  namespace Rec
  {
    namespace TemplateRings
    {

      static const InterfaceID IID_IRichRingReconParam ( "IRichRingReconParam", 1, 0 );

      /** @class IRichRingReconParam IRichRingReconParam.h RichRingRec/IRichRingReconParam.h
       *
       *
       *  @author Sajan EASO
       *  @date   2007-06-12
       */
      class IRichRingReconParam : virtual public IAlgTool {
      public:

        // Return the interface ID
        static const InterfaceID& interfaceID() { return IID_IRichRingReconParam; }

        virtual double YAgelShift()=0;
        virtual int MinRadiator()=0;
        virtual int MaxRadiator()=0;
        virtual bool  ActivateSingleEvHisto()=0;
        virtual int MinTrackNumForDebug()=0;
        virtual int MaxTrackNumForDebug()=0;
        virtual bool ActivateSingleTrackDebug()=0;
        virtual StatusCode  initialize()=0;
        virtual std::string RingRecHistoPath()=0;
        virtual bool ActivateRandomPhiLocalScaling()=0;
        virtual bool   ActivateMCCompareMassPrint()=0;
        virtual bool ActivateMaxNumberOfTrackSegmentsInRadiator()=0;
        virtual int MaxNumTrackSegmentsInAerogel() =0;
        virtual int MaxNumTrackSegmentsInR1gas() =0;
        virtual int MaxNumTrackSegmentsInR2gas() =0;
        virtual double MinTrackMomentumSelectInAerogel() =0;
        virtual double MinTrackMomentumSelectInRich1Gas() =0;
        virtual double MinTrackMomentumSelectInRich2Gas() =0;
        virtual bool ActivateWithoutTrackMomentumInfo() =0;
        

        //  virtual bool WriteOutAuxiliaryDebugHisto()=0;
        // virtual std::string RichDebugAuxHistoFileName()=0;




      protected:

      private:

      };

    }
  }
}

#endif // RICHRINGREC_IRICHRINGRECONPARAM_H
