copy /Y bin\Debug\bff_printnull.exe bin\Debug\cl.exe
copy /Y bin\Debug\bff_printnull.exe bin\Debug\lib.exe
copy /Y bin\Debug\bff_printnull.exe bin\Debug\link.exe
copy /Y bin\Debug\bff_printnull.exe bin\Debug\rc.exe
REM msbuild.exe /t:Clean,Build /p:ExecutablePath="%~dp0bin\Debug" /v:detailed vs2012\apps\bff_printargs.vcxproj
msbuild.exe /t:Clean,Build vs2012\vs2bff.sln
