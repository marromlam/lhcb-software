if exist %CMTINSTALLAREA% rd /q/s %CMTINSTALLAREA%
set QUIET=1
cmt broadcast if exist ..\Visual rd /q/s ..\Visual
cmt broadcast if exist ..\%CMTCONFIG% rd /q/s ..\%CMTCONFIG%
cmt broadcast cmt config
cmt broadcast cmt build vsnet
cmt broadcast cmt make > build_%CMTCONFIG%.log

