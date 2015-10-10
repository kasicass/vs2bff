copy /Y bin\Debug\bff_printargs.exe bin\Debug\cl.exe
copy /Y bin\Debug\bff_printargs.exe bin\Debug\lib.exe
copy /Y bin\Debug\bff_printargs.exe bin\Debug\link.exe
copy /Y bin\Debug\bff_printargs.exe bin\Debug\rc.exe
msbuild.exe /t:Clean,Build /p:ExecutablePath="%~dp0bin\Debug" vs2012\apps\bff_printargs.vcxproj
msbuild.exe /t:Clean,Build /p:ExecutablePath="%~dp0bin\Debug" vs2012\apps\bff_logtest.vcxproj