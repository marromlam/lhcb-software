set TAN_PORT=YES
set test_exe=%ONLINEKERNELROOT%\win32_vc71_dbg\test.exe
set gaudi_exe=%GAUDIONLINEROOT%\win32_vc71_dbg\Gaudi.exe GaudiOnline.dll OnlineTask 
set UTGID=MBMMon_0  && start "Monitor" %test_exe% mbm_mon
set UTGID=MBMInit_0 && start %gaudi_exe% -main=..\options\MBMinit.opts  -opt=..\options\Daemon.opts -a
set UTGID=TAN_0     && start %gaudi_exe% -main=..\options\TanAlloc.opts -opt=..\options\Daemon.opts -a
set UTGID=TANMON_0  && start "Tanmon"  %test_exe% tanmon -c
sleep 3
set UTGID=EvtProd_0 && start %gaudi_exe% -opt=..\options\MEPConverter.opts -a
set UTGID=Holder_0  && start %gaudi_exe% -opt=..\options\MEPHolder.opts    -a
set UTGID=Moore_0   && start %gaudi_exe% -opt=..\options\ReadMBM.opts
set UTGID=Moore_1   && start %gaudi_exe% -opt=..\options\ReadMBM.opts
set UTGID=Send_0    && start %gaudi_exe% -opt=..\options\MBM2Network.opts  -a

rem start "prod_0" ..\win32_vc71_dbg\Gaudi.exe GaudiOnline mep_producer -n=prod_0 -p=333 -s=500 -r=2
