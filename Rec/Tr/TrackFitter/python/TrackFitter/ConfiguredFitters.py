
# ====================================================================
#  Track fitting options
# ====================================================================

from TrackSys.Configuration import TrackSys
from Gaudi.Configuration import allConfigurables

from Configurables import ( TrackEventFitter, TrackMasterFitter, TrackKalmanFilter,
                            TrackProjectorSelector, TrajOTProjector, TrackMasterExtrapolator,
                            TrackSimpleExtraSelector, TrackDistanceExtraSelector,
                            TrackHerabExtrapolator,
                            SimplifiedMaterialLocator, DetailedMaterialLocator,
                            MeasurementProvider, StateDetailedBetheBlochEnergyCorrectionTool)

def ConfiguredMasterFitter( Name,
                            FieldOff = TrackSys().fieldOff(),
                            SimplifiedGeometry = TrackSys().simplifiedGeometry(),
                            NoDriftTimes       = TrackSys().noDrifttimes(),
                            KalmanSmoother     = TrackSys().kalmanSmoother(),
                            LiteClusters       = False ):
    if isinstance(Name,TrackMasterFitter) :
        fitter = Name
    else :
        if allConfigurables.get( Name ) :
            raise ValueError, 'ConfiguredMasterFitter: instance with name '+Name+' already exists'
        fitter = TrackMasterFitter(Name)
        
    # add the tools that need to be modified
    fitter.addTool( TrackMasterExtrapolator, name = "Extrapolator" )
    fitter.addTool( TrackKalmanFilter, name = "NodeFitter" )

    # set up the material locator
    if SimplifiedGeometry:
        fitter.addTool(SimplifiedMaterialLocator, name="MaterialLocator")
        fitter.Extrapolator.addTool(SimplifiedMaterialLocator, name="MaterialLocator")
        
    # not yet used for DC09 production    
    # else:
    #    fitter.addTool(DetailedMaterialLocator(), name="MaterialLocator")
    #    fitter.MaterialLocator.GeneralDedxToolName="StateDetailedBetheBlochEnergyCorrectionTool"
    #    fitter.Extrapolator.addTool(DetailedMaterialLocator(), name="MaterialLocator")
    #    fitter.Extrapolator.GeneralDedxToolName = "StateDetailedBetheBlochEnergyCorrectionTool"
    #    fitter.Extrapolator.addTool(DetailedMaterialLocator(), name="MaterialLocator")
    #    fitter.Extrapolator.MaterialLocator.GeneralDedxToolName = "StateDetailedBetheBlochEnergyCorrectionTool"
        
    # special settings for field-off
    if FieldOff:
        fitter.Extrapolator.addTool(TrackSimpleExtraSelector, name="ExtraSelector")
        fitter.Extrapolator.ExtraSelector.ExtrapolatorName = "TrackLinearExtrapolator"
        fitter.Extrapolator.ApplyEnergyLossCorr = False
        fitter.Extrapolator.ApplyElectronEnergyLossCorr = False
        fitter.ApplyEnergyLossCorr = False
        fitter.NodeFitter.DoF = 4

    # change the smoother
    if KalmanSmoother:
        fitter.NodeFitter.BiDirectionalFit = False

    # don't use drift times
    if NoDriftTimes:
        # set up the NoDriftTimeProjector in the toolsvc
        defaultOTNoDriftTimeProjector = TrajOTProjector("OTNoDrifttimesProjector")
        defaultOTNoDriftTimeProjector.UseDrift = False
        fitter.NodeFitter.addTool( TrackProjectorSelector(), "Projector" )
        fitter.NodeFitter.Projector.OT = defaultOTNoDriftTimeProjector

    # use lite clusters for velo and ST
    if LiteClusters:
        fitter.addTool( MeasurementProvider(), name = "MeasProvider")
        from Configurables import (MeasurementProviderT_MeasurementProviderTypes__VeloLiteR_,
                                   MeasurementProviderT_MeasurementProviderTypes__VeloLitePhi_,
                                   MeasurementProviderT_MeasurementProviderTypes__TTLite_,
                                   MeasurementProviderT_MeasurementProviderTypes__ITLite_)
        fitter.MeasProvider.VeloRProvider = MeasurementProviderT_MeasurementProviderTypes__VeloLiteR_()
        fitter.MeasProvider.VeloPhiProvider = MeasurementProviderT_MeasurementProviderTypes__VeloLitePhi_()
        fitter.MeasProvider.TTProvider = MeasurementProviderT_MeasurementProviderTypes__TTLite_()
        fitter.MeasProvider.ITProvider = MeasurementProviderT_MeasurementProviderTypes__ITLite_()

    if TrackSys().cosmics():
        fitter.addTool( MeasurementProvider(), name = "MeasProvider")
        fitter.MeasProvider.OTProvider.RawBankDecoder = 'OTMultiBXRawBankDecoder'

    return fitter


def ConfiguredEventFitter( Name,
                           TracksInContainer,
                           FieldOff = TrackSys().fieldOff(),
                           SimplifiedGeometry = TrackSys().simplifiedGeometry(),
                           NoDriftTimes       = TrackSys().noDrifttimes(),
                           KalmanSmoother     = TrackSys().kalmanSmoother(),
                           LiteClusters = False ):
    # make sure the name is unique
    if allConfigurables.get( Name ) :
        raise ValueError, 'ConfiguredEventFitter: instance with name '+Name+' already exists'
    # create the event fitter
    eventfitter = TrackEventFitter(Name)
    eventfitter.TracksInContainer = TracksInContainer
    # add the tools that need to be modified
    fittername = Name + ".Fitter"
    eventfitter.addTool( ConfiguredMasterFitter( fittername,
                                                 FieldOff=FieldOff,
                                                 SimplifiedGeometry=SimplifiedGeometry,
                                                 NoDriftTimes=NoDriftTimes,
                                                 KalmanSmoother=KalmanSmoother,
                                                 LiteClusters=LiteClusters ), name = "Fitter")
    return eventfitter

def ConfiguredPrefitter( Name = "DefaultEventFitter",
                         TracksInContainer = "Rec/Tracks/Best",
                         FieldOff = TrackSys().fieldOff(),
                         SimplifiedGeometry = TrackSys().simplifiedGeometry(),
                         LiteClusters = False ):
    eventfitter = ConfiguredEventFitter(Name,TracksInContainer,
                                        FieldOff=FieldOff,
                                        SimplifiedGeometry=SimplifiedGeometry,
                                        NoDriftTimes=True,
                                        LiteClusters=LiteClusters)
    eventfitter.Fitter.NumberFitIterations = 1
    eventfitter.Fitter.MaxNumberOutliers = 0
    eventfitter.Fitter.ErrorY2 = 10000
    return eventfitter

def ConfiguredFitVelo( Name = "FitVelo",
                       TracksInContainer = "Rec/Track/PreparedVelo"):
    # note that we ignore curvatue in velo. in the end that seems the
    # most sensible thing to do.
    eventfitter = ConfiguredEventFitter(Name,TracksInContainer,
                                        FieldOff=True )
    eventfitter.Fitter.NumberFitIterations = 2
    #eventfitter.Fitter.ErrorP= [0.01, 5e-08]
    #eventfitter.Fitter.ErrorX2 = 100
    #eventfitter.Fitter.ErrorY2 = 100
    #eventfitter.Fitter.ErrorP= [0,0.01]
    #eventfitter.Fitter.Extrapolator.ApplyEnergyLossCorr = False
    return eventfitter

def ConfiguredFitVeloTT( Name = "FitVeloTT",
                         TracksInContainer = "Rec/Track/VeloTT" ):
    eventfitter = ConfiguredEventFitter(Name,TracksInContainer)
    eventfitter.Fitter.NumberFitIterations = 2
    eventfitter.Fitter.MaxNumberOutliers = 1
    eventfitter.Fitter.Extrapolator.ApplyEnergyLossCorr = False
    # Make the nodes even though this is a refit, to add StateAtBeamLine
    # (PatVeloTT also fits but does not make nodes)
    eventfitter.Fitter.MakeNodes = True
    return eventfitter

def ConfiguredFitSeed( Name = "FitSeed",
                       TracksInContainer = "Rec/Track/Seed" ):
    eventfitter = ConfiguredEventFitter(Name,TracksInContainer)
    eventfitter.Fitter.StateAtBeamLine = False
    eventfitter.Fitter.ErrorQoP = [0.04, 5e-08]
    return eventfitter

def ConfiguredFitForward( Name = "FitForward",
                         TracksInContainer = "Rec/Track/Forward" ):
    eventfitter = ConfiguredEventFitter(Name,TracksInContainer)
    return eventfitter

def ConfiguredFitMatch( Name = "FitMatch",
                        TracksInContainer = "Rec/Track/Match" ):
    eventfitter = ConfiguredEventFitter(Name,TracksInContainer)
    return eventfitter
 
def ConfiguredFitDownstream( Name = "FitDownstream",
                             TracksInContainer = "Rec/Track/Downstream" ):
    eventfitter = ConfiguredEventFitter(Name,TracksInContainer)
    return eventfitter

def ConfiguredPreFitForward( Name = "PreFitForward",
                             TracksInContainer = "Rec/Track/Forward" ):
    return ConfiguredPrefitter(Name,TracksInContainer)

def ConfiguredPreFitMatch( Name = "PreFitMatch",
                           TracksInContainer = "Rec/Track/Match" ):
    return ConfiguredPrefitter(Name,TracksInContainer)

def ConfiguredPreFitDownstream( Name = "PreFitDownstream",
                                TracksInContainer = "Rec/Track/Downstream" ):
    eventfitter = ConfiguredPrefitter(Name,TracksInContainer)
    return eventfitter

def ConfiguredFastFitter( Name, FieldOff = TrackSys().fieldOff(), LiteClusters = True,
                          ForceUseDriftTime = True ):
    fitter = ConfiguredMasterFitter(Name,
                                    FieldOff=FieldOff,
                                    SimplifiedGeometry = True,
                                    LiteClusters = LiteClusters)
    fitter.NumberFitIterations = 1
    fitter.MaxNumberOutliers = 0
    fitter.AddDefaultReferenceNodes = False
    fitter.NodeFitter.BiDirectionalFit = False
    fitter.NodeFitter.Smooth = False
    if ForceUseDriftTime:
        from Configurables import TrajOTProjector, TrackProjectorSelector
        otprojector = TrajOTProjector('OTFastFitProjector')
        otprojector.SkipDriftTimeZeroAmbiguity = False
        fitter.NodeFitter.addTool( TrackProjectorSelector(), "Projector" )
        fitter.NodeFitter.Projector.OT = otprojector
        
    # at some point, need to switch to analytic evaluation
    # TrackHerabExtrapolator().extrapolatorID = 4
    return fitter

def ConfiguredFastEventFitter( Name, TracksInContainer,
                               ForceUseDriftTime = True ):
    eventfitter = TrackEventFitter(Name)
    eventfitter.TracksInContainer = TracksInContainer
    fittername = Name + ".Fitter"
    eventfitter.addTool( ConfiguredFastFitter( Name = fittername, ForceUseDriftTime = ForceUseDriftTime ), name = "Fitter")
    return eventfitter

def ConfiguredFastVeloOnlyEventFitter( Name, TracksInContainer ):
    eventfitter = ConfiguredFastFitter( Name, TracksInContainer,FieldOff=True )
    eventfitter.Fitter.addTool( MeasurementProvider, name = 'MeasProvider')
    eventfitter.Fitter.MeasProvider.IgnoreIT = True
    eventfitter.Fitter.MeasProvider.IgnoreOT = True
    eventfitter.Fitter.MeasProvider.IgnoreTT = True
    eventfitter.Fitter.MeasProvider.IgnoreMuon = True
    return eventfitter

def ConfiguredStraightLineFit( Name, TracksInContainer,
                               NoDriftTimes =  TrackSys().noDrifttimes()  ):
    eventfitter = ConfiguredEventFitter(Name,TracksInContainer,
                                        FieldOff=True,
                                        NoDriftTimes=NoDriftTimes)
    eventfitter.Fitter.ApplyMaterialCorrections = False
    eventfitter.Fitter.Extrapolator.ApplyMultScattCorr = False
    eventfitter.Fitter.StateAtBeamLine = False
    eventfitter.Fitter.AddDefaultReferenceNodes = False
    return eventfitter

