copy /Y ..\bin\Debug\bff_toolsethook.exe ..\bin\Debug\cl.exe
copy /Y ..\bin\Debug\bff_toolsethook.exe ..\bin\Debug\lib.exe
copy /Y ..\bin\Debug\bff_toolsethook.exe ..\bin\Debug\link.exe
copy /Y ..\bin\Debug\bff_toolsethook.exe ..\bin\Debug\rc.exe

REM set VSBFF_CONVERTER_ADDRESS="tcp://127.0.0.1:5550"
..\bin\Debug\bff_submit.exe begin x86
msbuild.exe /t:Rebuild /p:ExecutablePath="%~dp0..\bin\Debug" 02_ConsolePCH\ConsolePCH.sln
..\bin\Debug\bff_submit.exe end
