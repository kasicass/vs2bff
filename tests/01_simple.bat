copy /Y ..\bin\Debug\bff_toolsethook.exe ..\bin\Debug\cl.exe
copy /Y ..\bin\Debug\bff_toolsethook.exe ..\bin\Debug\lib.exe
copy /Y ..\bin\Debug\bff_toolsethook.exe ..\bin\Debug\link.exe
copy /Y ..\bin\Debug\bff_toolsethook.exe ..\bin\Debug\rc.exe

REM set VSBFF_CONVERTER_ADDRESS="tcp://127.0.0.1:5550"
REM ..\bin\Debug\bff_submit.exe begin
msbuild.exe /t:Rebuild /p:ExecutablePath="%~dp0..\bin\Debug" 01_simple\vs2012\simple.sln
REM ..\bin\Debug\bff_submit.exe end
