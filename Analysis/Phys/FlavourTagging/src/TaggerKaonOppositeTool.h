// $Id: TaggerKaonOppositeTool.h,v 1.9 2008-11-11 20:47:12 musy Exp $
#ifndef USER_TAGGERKAONOPPOSITETOOL_H 
#define USER_TAGGERKAONOPPOSITETOOL_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolFactory.h"
// from Event
#include "Event/FlavourTag.h"
#include "Kernel/ITagger.h"
#include "INNetTool.h"

#include "ITaggingUtils.h"

/** @class TaggerKaonOppositeTool TaggerKaonOppositeTool.h 
 *
 *  Tool to tag the B flavour with a KaonOpposite Tagger
 *
 *  @author Marco Musy
 *  @date   30/06/2005
 */

class TaggerKaonOppositeTool : public GaudiTool, 
                               virtual public ITagger {

public: 
  /// Standard constructor
  TaggerKaonOppositeTool( const std::string& type,
			  const std::string& name,
			  const IInterface* parent );
  virtual ~TaggerKaonOppositeTool( ); ///< Destructor
  StatusCode initialize();    ///<  initialization
  StatusCode finalize  ();    ///<  finalization

  //-------------------------------------------------------------
  virtual LHCb::Tagger tag( const LHCb::Particle*, const LHCb::RecVertex*,
                            std::vector<const LHCb::Vertex*>&, 
                            LHCb::Particle::ConstVector&);
  //-------------------------------------------------------------

private:
  
  ITaggingUtils* m_util;
  INNetTool* m_nnet;
  std::string m_CombinationTechnique, m_NeuralNetName;

  //properties 
  double m_Pt_cut_kaon;
  double m_P_cut_kaon ;
  double m_IP_cut_kaon ;
  double m_IPs_cut_kaon ;
  double m_lcs_kl, m_lcs_ku;
  double m_IP_kl, m_IP_ku;
  double m_AverageOmega;
  double m_PIDkp_extracut;
  double m_ghost_cut;

};

//===============================================================//
#endif // USER_TAGGERKAONOPPOSITETOOL_H
