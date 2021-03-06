################################################################################
# Package: HltCommon
################################################################################
gaudi_subdir(HltCommon v10r7)

gaudi_depends_on_subdirs(Det/VeloDet
                         Event/DAQEvent
                         Event/DigiEvent
                         Event/HltEvent
                         Event/L0Event
                         Event/TrackEvent
                         GaudiAlg
                         Hlt/HltBase
                         Kernel/HltInterfaces
                         Phys/LoKi
                         Phys/LoKiHlt
                         Phys/LoKiTracks
                         Phys/LoKiTrigger
                         Tr/TrackInterfaces
                         Tf/TfTools)

find_package(Boost COMPONENTS system regex iostreams)

gaudi_add_module(HltCommon
                 src/*.cpp
                 INCLUDE_DIRS Boost Event/DigiEvent Phys/LoKi Tr/TrackInterfaces Tf/TfTools
                 LINK_LIBRARIES Boost VeloDetLib DAQEventLib HltEvent L0Event TrackEvent GaudiAlgLib HltBase HltInterfaces LoKiHltLib LoKiTracksLib LoKiTriggerLib)

gaudi_install_python_modules()

gaudi_add_test(QMTest QMTEST)
