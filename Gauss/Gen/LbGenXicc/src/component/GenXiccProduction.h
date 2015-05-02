#ifndef LBGENXICC_GENXICCPRODUCTION_H 
#define LBGENXICC_GENXICCPRODUCTION_H 1

// LbHard.
#include "LbHard/HardProduction.h"

// MC interface.
#include "MCInterfaces/QQqBaryons.h"

/** 
 * Production tool to generate events with GenXicc.
 * 
 * GenXicc is a dedicated Monte Carlo generator for the production of
 * double heavy baryons. The generator produces the initial hard
 * processes g g -> X Q-bar Q-bar, g Q -> X Q-bar, and Q Q -> X g.
 * The remainder of the event (shower, MPI, etc.) is generated by one
 * of the shower tools implemented via the HardProduction
 * interface. For information on possible settings, consult the
 * original paper, arXiv:hep-ph/0702054. Further documentation can be
 * found in arXiv:1210.3458.
 *
 * Masses are set by GenXicc (in parameter.F) according to the
 * requested baryon state since for internal consistency the baryon
 * mass must be the sum of the 2 heavy quark masses. Consequently, it
 * is better not to override the internal settings.
 *
 * @class  GenXiccProduction
 * @file   GenXiccProduction.h 
 * @author F. Zhang
 * @author Philip Ilten
 * @date   2011-04-10
 */
class GenXiccProduction : public HardProduction, public QQqBaryons {
 public:

  /// Default constructor.
  GenXiccProduction(const std::string &type, const std::string &name,
		    const IInterface *parent);
  
  /// Initialize the hard process tool.
  StatusCode hardInitialize();

 protected:

  /// Parse the GenXicc settings.
  StatusCode parseSettings(const CommandVector& settings, bool change = false);

 private:

  CommandVector m_defaultSettings;  ///< The default settings.
  std::string m_baryon;             ///< The baryon state to be produced.
  double m_beamMomentum;            ///< The beam momentum.
};

#endif // LBGENXICC_GENXICCPRODUCTION_H
