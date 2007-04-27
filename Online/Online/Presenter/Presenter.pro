# Automatically generated by qmake (1.07a) Fri Feb 16 16:33:42 2007
######################################################################
WIN32: QMAKESPEC = w32-msvc.net
win32: QMAKE_CXXFLAGS += /EHsc
# /GR /GX should come with Qt
win32: QMAKE_LFLAGS_WINDOWS += /nologo /machine:ix86 /debug \
/verbose:lib  /map /nodefaultlib kernel32.lib user32.lib ws2_32.lib \
advapi32.lib shell32.lib msvcrt.lib msvcprt.lib oldnames.lib    
MAKE_ARGS+= -j2

# Boost likes this:
#DEFINES += BOOST_TEST_NO_AUTO_LINK
DEFINES += BOOST_ALL_DYN_LINK
#"WIN32" "_MBCS"  "_WINDOWS" FD c nologo W3 GX MDd GR Zm500

# TODO: create an AFS/Z: prefix and make better paths

TEMPLATE = app
DEPENDPATH +=  src gui/presenter
win32: INCLUDEPATH += inc/ gui/presenter $(ROOTSYS)/include \
Z:/cern.ch/sw/lcg/external/Boost/1.33.1/win32_vc71/include/boost-1_33_1 \
Z:/cern.ch/sw/lcg/external/oracle/10.2.0.2/win32_vc71/include/ \
Z:\cern.ch\lhcb\software\releases\ONLINE\ONLINE_v3r1\Online\DIM\v16r7/dim \
../../OnlineHistDB/v2r0/
unix: INCLUDEPATH += inc/ gui/presenter $(ROOTSYS)/include \
/afs/cern.ch/sw/lcg/external/Boost/1.33.1/$(CMTCONFIG)/include/boost-1_33_1 \
/afs/cern.ch/sw/lcg/external/oracle/10.2.0.2/$(CMTCONFIG)/include \
$(DIMROOT)/dim \
../../OnlineHistDB/v2r0/

win32: LIBS  +=  -L$(ROOTSYS)/lib libHist.lib libGraf.lib libGraf3d.lib \
libGpad.lib libTree.lib libRint.lib libGui.lib libQtGSI.lib libCore.lib libCint.lib \
-LZ:/cern.ch/sw/lcg/external/Boost/1.33.1/win32_vc71/lib \
boost_date_time-vc71-mt-1_33_1.lib \
-LZ:/cern.ch/sw/lcg/external/oracle/10.2.0.2/win32_vc71/lib oraocci10.lib \
-LZ:\cern.ch\lhcb\software\releases\ONLINE\ONLINE_v3r1\Online\DIM\v16r7/$(CMTCONFIG) -ldim \  
-L../../OnlineHistDB/v2r0/$(CMTCONFIG) OnlineHistDB.lib  

unix: LIBS  +=  -L$(ROOTSYS)/lib -lHist -lGraf -lGraf3d -lGpad -lTree -lRint \
-lGui -lQtGSI -lCore -lCint \
-L/afs/cern.ch/sw/lcg/external/Boost/1.33.1/slc4_ia32_gcc34/lib -lboost_date_time-gcc-mt \
-L/afs/cern.ch/sw/lcg/external/oracle/10.2.0.2/slc4_ia32_gcc34/lib -lclntsh \
-L$(DIMROOT)/$(CMTCONFIG) -ldim \
-L../../OnlineHistDB/v2r0/$(CMTCONFIG) -lOnlineHistDB

CONFIG  += qt warn_on thread debug
MOC_DIR = $(PRESENTERROOT)/tmp/
unix: UIC_DIR = $(PRESENTERROOT)/tmp/ #- buggy in win32
OBJECTS_DIR = $(PRESENTERROOT)/$(CMTCONFIG)/

# Input
IMAGES = gui/images/h1_t.png \
				gui/images/h2_t.png \
				gui/images/splash2.png
HEADERS +=  gui/presenter/PageEditorMainWindow.ui.h \
					 	gui/presenter/DatabaseLoginDialog.ui.h
INTERFACES += gui/presenter/PageEditorMainWindow.ui \
							gui/presenter/DatabaseLoginDialog.ui
SOURCES +=  src/main.cpp src/DimProxy.cpp
