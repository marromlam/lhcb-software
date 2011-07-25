##
##  File containing options to activate the Em Option3
##  Physics in Geant4 (the default for production is Em
##  Option1)
##

from Gauss.Configuration import *

Gauss().PhysicsList = {"Em":'NoCuts', "Hadron":'LHEP', "GeneralPhys":True, "LHCbPhys":True}
