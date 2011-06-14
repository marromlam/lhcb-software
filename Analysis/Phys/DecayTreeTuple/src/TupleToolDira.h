#ifndef FSOOMRO_TUPLETOOLDIRA_H
#define FSOOMRO_TUPLETOOLDIRA_H 1

// Include files
// from Gaudi
#include "TupleToolBase.h"
#include "Kernel/IParticleTupleTool.h"            // Interface
#include "Math/SMatrix.h"

#include <Kernel/GetDVAlgorithm.h>
#include <Kernel/IDistanceCalculator.h>

class IDVAlgorithm;
class IDistanceCalculator;

namespace LHCb {
  class Particle;
  class VertexBase;
};


typedef ROOT::Math::SMatrix<double, 10, 10, ROOT::Math::MatRepSym<double,10> > SymMatrix10x10;

class TupleToolDira : public TupleToolBase, virtual public IParticleTupleTool {
public:
  /// Standard constructor
  TupleToolDira( const std::string& type,
		     const std::string& name,
		     const IInterface* parent);
  
  virtual ~TupleToolDira( ){}; ///< Destructor
  
  virtual StatusCode initialize();
  
  StatusCode fill( const LHCb::Particle*
		   , const LHCb::Particle*
		   , const std::string&
		   , Tuples::Tuple& );

private:

  StatusCode fillDIRAError( const LHCb::VertexBase* primVtx ,const LHCb::Particle* P , 
			    const std::string head, Tuples::Tuple& tuple )  const ;
  void calculateDIRAError( const LHCb::Particle*, const LHCb::VertexBase*, double&, double&) const ;

  //  IContextTool* m_context;
  DVAlgorithm* m_context;

};
#endif // FSOOMRO_TUPLETOOLDIRA_H
