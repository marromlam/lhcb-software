#
#   Stripping selections 
#
#   @author A. Poluektov
#   @date 2009-05-15
#
from Gaudi.Configuration import *

#
# Import stripping selections
#

# Angelo's selection
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingB2Charged2Body.py")

# Vava and Jacopo's B->Dh selections
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingB2DPi.py")

# Rob's selection
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingBd2KstarMuMu.py")

# Greig's selections
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingBd2JpsiKstar.py")
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingBd2JpsiKS.py")
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingBs2JpsiPhi.py")
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingBu2JpsiK.py")
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingBs2JpsiPhiPrescaledAndDetached.py")

# Greig and Marta's selection
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingBd2DstarMuNu.py")

# Basem's selection :
#importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingBiasedBs2JpsiPhi.py")
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingBiasedBs2JpsiPhi.py")

# Topological selections
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingTopo.py")

# Andrew Powell's RICH calibration selections
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingDstarNoPID.py")
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingRICHLambdaNoPID.py")

# Muon ID calibration selection
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingMuIDCalib.py")
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingJpsiInc.py")

# LiMing's Bs -> Jpsi f0 selection
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingBs2Jpsif0.py")

# Sebastian's Bs -> Phi Phi selection
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingBs2PhiPhi.py")

# Diego's selections
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingBd2JpsiKstarDetached.py")
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingBd2JpsiKstarDetachedR.py")
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingBu2JpsiKDetached.py")
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingBu2JpsiKDetachedR.py")

# Susan's selection
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingBu2D0K_D02KSPiPi.py")

# Jussara's selection
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingBu2hhh.py")

# Fernando's selection
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingBd2JpsiKSBiased.py")
