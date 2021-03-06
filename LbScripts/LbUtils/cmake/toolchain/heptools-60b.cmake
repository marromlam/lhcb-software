cmake_minimum_required(VERSION 2.8.5)

# Declare the version of HEP Tools we use
# (must be done before including heptools-common to allow evolution of the
# structure)
set(heptools_version  60b)

include(${CMAKE_CURRENT_LIST_DIR}/heptools-common.cmake)

# please keep alphabetic order and the structure (tabbing).
# it makes it much easier to edit/read this file !

LCG_AA_project(COOL   COOL_2_8_9a)
LCG_AA_project(CORAL  CORAL_2_3_15)
LCG_AA_project(POOL   POOL_2_9_13)
LCG_AA_project(RELAX  RELAX_1_2_2b)
LCG_AA_project(ROOT   5.28.00b)

LCG_compiler(gcc43 gcc 4.3.2)

# externals:
LCG_external_package(AIDA             3.2.1)

# keep the follwing two in sync !
LCG_external_package(4suite           1.0.2p1)
LCG_external_package(bjam             3.1.13)
LCG_external_package(blas             20070405)
LCG_external_package(Boost            1.44.0)
LCG_external_package(bz2lib           1.0.2)
LCG_external_package(CASTOR           2.1.8-10            castor)
LCG_external_package(cernlib          2006a)
LCG_external_package(cgal             3.3.1)
LCG_external_package(cgsigsoap        1.3.3-1)
LCG_external_package(CLHEP            1.9.4.7             clhep)
LCG_external_package(cmake            2.6.4)
LCG_external_package(cmt              v1r20p20081118)
LCG_external_package(coin3d           3.1.3.p1)
LCG_external_package(CppUnit          1.12.1_p1)
LCG_external_package(cx_oracle        5.0.1)
LCG_external_package(Cygwin           1.5)
LCG_external_package(david            1_36a)
LCG_external_package(dawn             3_88a)
LCG_external_package(dcache_client    1.9.3p1)
LCG_external_package(dcache_dcap      1.9.3-7)
LCG_external_package(dcache_srm       1.8.0-15p7)
LCG_external_package(doxygen          1.7.0)
LCG_external_package(dpm              1.7.4-7sec)
LCG_external_package(EDGRLS           2.3.3)
LCG_external_package(Expat            1.95.8)
LCG_external_package(fftw             3.1.2               fftw3)
LCG_external_package(Frontier_Client  2.8.0               frontier_client)
LCG_external_package(fastjet          2.4.2p5)
LCG_external_package(GCCXML           0.9.0_20100114      gccxml)
LCG_external_package(genshi           0.6)
LCG_external_package(gfal             1.11.8-2)
LCG_external_package(globus           4.0.7-VDT-1.10.1)
LCG_external_package(graphviz         2.24.0)
LCG_external_package(GSL              1.10)
LCG_external_package(HepMC            2.03.11)
LCG_external_package(HepPDT           2.06.01)
LCG_external_package(HippoDraw        1.9.0)
LCG_external_package(ipython          0.8.4)
LCG_external_package(javasdk          1.6.0)
LCG_external_package(javajni          ${javasdk_config_version})
LCG_external_package(json             2.1.1)
LCG_external_package(kcachegrind      0.4.6)
LCG_external_package(ktjet            1.08)
LCG_external_package(lapack           3.1.1)
LCG_external_package(lcgdmcommon      1.7.4-7sec)
LCG_external_package(lcginfosites     2.6.2-1)
LCG_external_package(lcgutils         1.7.6-1)
LCG_external_package(lcov             1.9)
LCG_external_package(lfc              1.7.4-7sec          Grid/LFC)
LCG_external_package(libsvm           2.86)
LCG_external_package(libunwind        0.99)
LCG_external_package(matplotlib       0.99.1.2)
LCG_external_package(minuit           5.27.02)
LCG_external_package(MathLib          1_0_0)
LCG_external_package(mock             0.6.0)
LCG_external_package(multiprocessing  2.6.2.1)
LCG_external_package(myproxy          4.2-VDT-1.10.1)
LCG_external_package(mysql            5.1.45)
LCG_external_package(mysql_python     1.2.2)
LCG_external_package(neurobayes       10.8)
LCG_external_package(neurobayes_expert 10.8)
LCG_external_package(numpy            1.3.0)
LCG_external_package(oracle           11.2.0.1.0p2)
LCG_external_package(processing       0.51)
LCG_external_package(py               1.2.1)
LCG_external_package(pydot            1.0.2)
LCG_external_package(pyanalysis       1.2)
LCG_external_package(pygraphics       1.1p1)
LCG_external_package(pyminuit         0.0.1)
LCG_external_package(pytools          1.4)
LCG_external_package(Python           2.6.5)
LCG_external_package(pyqt             4.7)
LCG_external_package(pyparsing        1.5.1)
LCG_external_package(pyxml            0.8.4p1)
LCG_external_package(QMtest           2.4.1)
LCG_external_package(Qt               4.6.3p2              qt)
LCG_external_package(qwt              5.2.1)
LCG_external_package(readline         2.5.1p1)
LCG_external_package(roofit           3.10p1)
LCG_external_package(rx               1.5)
LCG_external_package(setuptools       0.6c11)
LCG_external_package(scipy            0.7.1)
LCG_external_package(scons            0.96.1)
LCG_external_package(sip              4.10)
LCG_external_package(siscone          1.3.3)
LCG_external_package(slc4_compat      1.0)
LCG_external_package(soqt             1.5.0.p1)
LCG_external_package(sqlalchemy       0.6.3)
LCG_external_package(sqlite           3.6.22)
LCG_external_package(storm            0.16.0)
LCG_external_package(sympy            0.6.7)
LCG_external_package(tcmalloc         1.5)
LCG_external_package(tcsh             6.16.00)
LCG_external_package(uuid             1.38p1)
LCG_external_package(valgrind         3.6.0)
LCG_external_package(vdt              1.2.3_globusrls215b)
LCG_external_package(vomsapic         1.9.17-1)
LCG_external_package(vomsapicpp       1.9.17-1)
LCG_external_package(vomsapi_noglobus 1.9.17-1)
LCG_external_package(vomsclients      1.9.17-1)
LCG_external_package(XercesC          3.1.1p1)
LCG_external_package(xqilla           2.2.4)
LCG_external_package(zlib             1.2.3p1)

# Prepare the search paths according to the versions above
LCG_prepare_paths()
