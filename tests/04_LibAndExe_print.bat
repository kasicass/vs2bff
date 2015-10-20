copy /Y ..\bin\Debug\bff_printargs.exe ..\bin\Debug\cl.exe
copy /Y ..\bin\Debug\bff_printargs.exe ..\bin\Debug\lib.exe
copy /Y ..\bin\Debug\bff_printargs.exe ..\bin\Debug\link.exe
copy /Y ..\bin\Debug\bff_printargs.exe ..\bin\Debug\rc.exe

msbuild.exe /t:Rebuild /p:ExecutablePath="%~dp0..\bin\Debug" 04_LibAndExe\vs2012\LibAndExe.sln
