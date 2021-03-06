#include "BffCommon.hpp"
#include "BffContext.hpp"
#include <assert.h>
#include <sstream>
#include <unordered_set>

static std::wstring TokenStrip(const std::wstring& token)
{
	std::wstring out;
	if (token[0] == L'"')
	{ // "..\..\a.txt" => ..\..\a.txt
		out = token.substr(1, token.length()-2);
	}
	else
	{
		out = token;
	}
	return out;
}

static std::wstring RelativePathToAbsolutePath(const std::wstring& rpath, const std::wstring& workingDir)
{
	if (rpath.length() > 1 && rpath[1] == L':')
	{
		return rpath;
	}
	return workingDir + L"\\" + rpath;
}

static std::wstring PathRemoveFileName(const std::wstring& path)
{
	// Debug\a.txt => Debug\
	// a.txt => ""
	std::wstring::size_type i = path.find_last_of(L'\\');
	if (i != std::wstring::npos)
	{
		return path.substr(0, i+1);
	}
	else
	{
		return std::wstring();
	}
}

static std::wstring PathToFileName(const std::wstring& path)
{
	std::wstring::size_type i = path.find_last_of(L'\\');
	if (i != std::wstring::npos)
	{
		return path.substr(i);
	}
	else
	{
		return path;
	}
}

static std::wstring FileNameRemovePostfix(const std::wstring& s)
{
	std::wstring::size_type i = s.find_last_of(L'.');
	if (i != std::wstring::npos)
	{
		return s.substr(0, i);
	}
	else
	{
		return s;
	}
}

static BffObjectPCH ObjectPCH_VStoBFF(const VSObjectList& in)
{
	// D:\myprj\vs2bff\bin\Debug\CL.exe /c /ZI /nologo /W3 /WX- /Od /Oy- /D WIN32 /D _DEBUG
	// /D _CONSOLE /D _UNICODE /D UNICODE /Gm /EHsc /RTC1 /MDd /GS /fp:precise /Zc:wchar_t
	// /Zc:forScope /Yc"stdafx.h" /Fp"Debug\ConsolePCH.pch" /Fo"Debug\\" /Fd"Debug\vc110.pdb"
	// /Gd /TP /analyze- /errorReport:queue stdafx.cpp
	//
	// /Yc and /Yu
	
	BffObjectPCH out;
	VSBFF::Parser::StringUtil su;

	VSBFF::Parser::Tokenizer t(in.cmdline);
	for (unsigned int i = 0; i < t.count(); ++i)
	{
		const std::wstring& token = t[i];
		if (token[0] == L'/')
		{
			if (token == L"/D") // /D XXX
			{
				out.options += L" ";
				out.options += token;

				++i;
				out.options += L" ";
				out.options += t[i];
			}
			else if (su.startswith(token, L"/Fo"))
			{
				// /Fo"..." => ...
				/*
				std::wstring outDir = token.substr(4, token.length()-1-4);
				outDir = RelativePathToAbsolutePath(outDir, in.workingDir);
				out.options += L" /Fo\"";
				out.options += outDir;
				out.options += L"\"";
				*/
				out.options += L" /Fo\"%%3\""; // .obj name must be 'PreCompile.pch.obj', fastbuild needs it
			}
			else if (su.startswith(token, L"/Fp"))
			{
				// /Fp"..." => ...
				out.outputFile = token.substr(4, token.length()-1-4);
				out.outputFile = RelativePathToAbsolutePath(out.outputFile, in.workingDir);
				out.options += L" /Fp\"$PCHOutputFile$\"";
			}
			else if (su.startswith(token, L"/Fd"))
			{
				// /Fd"..." => ...
				std::wstring pdbFile = token.substr(4, token.length()-1-4);
				pdbFile = RelativePathToAbsolutePath(pdbFile, in.workingDir);
				out.options += L" /Fd\"";
				out.options += pdbFile;
				out.options += L"\"";
			}
			else
			{
				out.options += L" ";
				out.options += token;
			}
		}
		else
		{
			// ..\src\xx.cpp
			// "..\src\xx.cpp"
			std::wstring file = TokenStrip(token);
			out.inputFile = RelativePathToAbsolutePath(file, in.workingDir);
		}
	}
	out.options += L" %%1";

	assert(!out.inputFile.empty() &&
		!out.options.empty() &&
		!out.outputFile.empty());
	return out;
}

static BffObjectList ObjectList_VStoBFF(const VSObjectList& in)
{
	// cl.exe /c /ZI /nologo /W3 /WX- /Od /Oy- /D WIN32 /D _DEBUG /D _CONSOLE
	// /D _UNICODE /D UNICODE /Gm /EHsc /RTC1 /MDd /GS /fp:precise /Zc:wchar_t
	// /Zc:forScope /Fo"d:\myprj\vs2bff\tests\01_simple\vs2012\..\build\HelloWorld\Debug\\"
	// /Fd"d:\myprj\vs2bff\tests\01_simple\vs2012\..\build\HelloWorld\Debug\vc110.pdb"
	// /Gd /TP /analyze- /errorReport:queue ..\..\src\HelloWorld.cpp

	BffObjectList out;

	VSBFF::Parser::StringUtil su;
	VSBFF::Parser::Tokenizer t(in.cmdline);
	for (unsigned int i = 0; i < t.count(); ++i)
	{
		const std::wstring& token = t[i];
		if (token[0] == L'/')
		{
			if (token == L"/D") // /D XXX
			{
				out.compilerOptions += L" ";
				out.compilerOptions += token;

				++i;
				out.compilerOptions += L" ";
				out.compilerOptions += t[i];
			}
			else if (su.startswith(token, L"/Fo"))
			{
				// /Fo"..." => ...
				out.compilerOutputPath = token.substr(4, token.length()-1-4);
				out.compilerOutputPath = RelativePathToAbsolutePath(out.compilerOutputPath, in.workingDir);
				out.compilerOptions += L" /Fo%%2";
			}
			else if (su.startswith(token, L"/Fp"))
			{
				// /Fp"..." => ...
				out.compilerOptions += L" /Fp\"$PCHOutputFile$\"";
			}
			else if (su.startswith(token, L"/Fd"))
			{
				// /Fd"..." => ...
				std::wstring pdbFile = token.substr(4, token.length()-1-4);
				pdbFile = RelativePathToAbsolutePath(pdbFile, in.workingDir);
				out.compilerOptions += L" /Fd\"";
				out.compilerOptions += pdbFile;
				out.compilerOptions += L"\"";
			}
			else
			{
				out.compilerOptions += L" ";
				out.compilerOptions += token;
			}
		}
		else
		{
			std::wstring file = TokenStrip(token);
			file = RelativePathToAbsolutePath(file, in.workingDir);
			out.compilerInputFiles.push_back(file);
		}
	}
	out.compilerOptions += L" %%1";

	assert(!out.compilerInputFiles.empty() &&
		!out.compilerOptions.empty() &&
		!out.compilerOutputPath.empty());
	return out;
}

static BffLink Link_VStoBFF(const VSLink& link, const std::unordered_set<std::wstring>& librarySet)
{
	// d:\myprj\vs2bff\bin\Debug\link.exe /ERRORREPORT:QUEUE 
	// /OUT:"d:\myprj\vs2bff\tests\01_simple\vs2012\..\bin\Debug\HelloWorld.exe"
	// /INCREMENTAL /NOLOGO kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib 
	// advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib 
	// /MANIFEST /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /manifest:embed /DEBUG 
	// /PDB:"d:\myprj\vs2bff\tests\01_simple\vs2012\..\bin\Debug\HelloWorld.pdb" /SUBSYSTEM:CONSOLE 
	// /TLBID:1 /DYNAMICBASE /NXCOMPAT 
	// /IMPLIB:"d:\myprj\vs2bff\tests\01_simple\vs2012\..\bin\Debug\HelloWorld.lib" /MACHINE:X86 
	// d:\myprj\vs2bff\tests\01_simple\vs2012\..\build\HelloWorld\Debug\HelloWorld.obj

	BffLink out;
	VSBFF::Parser::StringUtil su;

	VSBFF::Parser::Tokenizer t(link.cmdline);
	for (unsigned int i = 0; i < t.count(); ++i)
	{
		const std::wstring& token = t[i];
		if (token[0] == L'/')
		{
			if (su.startswith(token, L"/OUT"))
			{
				// /OUT:"..." => ...
				out.linkerOutput = token.substr(6, token.length()-1-6);

				out.linkerOptions += L" /OUT:\"%%2\"";

				// xx\yy\zz.exe => zz
				auto pos = out.linkerOutput.find_last_of(L"\\");
				if (pos != std::wstring::npos)
				{
					out.name = out.linkerOutput.substr(pos+1, out.linkerOutput.length()-4-(pos+1));
				}
				else
				{
					out.name = out.linkerOutput.substr(0, out.linkerOutput.length()-4);
				}
			}
			else if (su.startswith(token, L"/MANIFESTUAC"))
			{
				std::wstringstream tmpss;
				for (auto ch = token.cbegin(); ch != token.cend(); ++ch)
				{
					if (*ch == L'\'') { tmpss << L"^'"; }
					else tmpss.put(*ch);
				}

				out.linkerOptions += L" ";
				out.linkerOptions += tmpss.str();
			}
			else
			{
				out.linkerOptions += L" ";
				out.linkerOptions += token;
			}
		}
		else
		{
			if (su.endswith(token, L".lib"))
			{
				out.linkerOptions += L" ";
				out.linkerOptions += token;

				std::wstring name = FileNameRemovePostfix(PathToFileName(token));
				if (librarySet.find(name) != librarySet.end())
				{
					out.libraries.push_back(name);
				}
			}
		}
	}

	out.linkerOptions += L" %%1";
	return out;
}

BffLib VSLib_To_BffLib(const VSLib& in)
{
	// lib.exe /OUT:"D:\myprj\vs2bff\tests\04_LibAndExe\vs2012\..\lib\Debug\MyLib.lib" /NOLOGO
	// D:\myprj\vs2bff\tests\04_LibAndExe\vs2012\..\build\MyLib\Debug\MyLib.obj

	BffLib out;
	VSBFF::Parser::StringUtil su;

	VSBFF::Parser::Tokenizer t(in.cmdline);
	for (unsigned int i = 0; i < t.count(); ++i)
	{
		const std::wstring& token = t[i];
		if (token[0] == L'/')
		{
			if (su.startswith(token, L"/OUT"))
			{
				// /OUT:"..." => ...
				out.librarianOutput  = token.substr(6, token.length()-1-6);
				out.librarianOptions += L" /OUT:\"%%2\"";

				// xx\yy\zz.lib => zz
				auto pos = out.librarianOutput.find_last_of(L"\\");
				if (pos != std::wstring::npos)
				{
					out.name = out.librarianOutput.substr(pos+1, out.librarianOutput.length()-4-(pos+1));
				}
				else
				{
					out.name = out.librarianOutput.substr(0, out.librarianOutput.length()-4);
				}
			}
			else
			{
				out.librarianOptions += L" ";
				out.librarianOptions += token;
			}
		}
		else
		{
			// ignore *.obj
			/*
			if (su.endswith(token, L".obj"))
			{
				out.librarianOptions += L" ";
				out.librarianOptions += token;
			}
			*/
		}
	}

	out.librarianOptions += L" %%1";
	return out;
}

BffResourceCompile VSResourceCompile_To_BffResourceCompile(const VSResourceCompile& in)
{
	// rc.exe /D _UNICODE /D UNICODE /l0x0409 /nologo
	// /foDebug\Win32Project1.res Win32Project1.rc 

	BffResourceCompile out;
	VSBFF::Parser::StringUtil su;

	VSBFF::Parser::Tokenizer t(in.cmdline);
	for (unsigned int i = 0; i < t.count(); ++i)
	{
		const std::wstring& token = t[i];
		if (token[0] == L'/')
		{
			if (su.startswith(token, L"/fo"))
			{
				/* /fo??\??\xxyy.res => ??\??\ */
				std::wstring path = PathRemoveFileName(token.substr(3));
				out.compilerOutputPath = RelativePathToAbsolutePath(path, in.workingDir);
				out.compilerOptions += L" /fo\"%%2\"";
			}
			else if (token == L"/D") // /D XXX
			{
				out.compilerOptions += L" ";
				out.compilerOptions += token;

				++i;
				out.compilerOptions += L" ";
				out.compilerOptions += t[i];
			}
			else
			{
				out.compilerOptions += L" ";
				out.compilerOptions += token;
			}
		}
		else
		{
			std::wstring file = TokenStrip(token);
			file = RelativePathToAbsolutePath(file, in.workingDir);
			out.compilerInputFiles.push_back(file);
		}
	}

	out.compilerOptions += L" %%1";
	return out;
}

void VStoBFF(const VSContext& vs, BffContext& bff)
{
	std::unordered_set<std::wstring> librarySet;

	bff.compiler = vs.compiler;
	bff.settings = vs.settings;
	
	for (auto it = vs.libs.cbegin(); it != vs.libs.cend(); ++it)
	{
		BffLibrary library;
		library.lib = VSLib_To_BffLib(it->lib);

		if (it->objs.size() == 1)
		{
			library.obj = ObjectList_VStoBFF(it->objs[0]);
			library.obj.name = library.lib.name + L"_0";
		}
		else
		{
			// PCH
			library.pch = ObjectPCH_VStoBFF(it->objs[0]);
			library.obj = ObjectList_VStoBFF(it->objs[1]);
			library.obj.name = library.lib.name + L"_0";
		}

		librarySet.insert(library.lib.name);
		bff.libs.push_back(library);
	}

	for (auto it = vs.exes.cbegin(); it != vs.exes.cend(); ++it)
	{
		BffExecutable exe;
		exe.link = Link_VStoBFF(it->link, librarySet);

		if (it->objs.size() == 1)
		{
			exe.obj = ObjectList_VStoBFF(it->objs[0]);
			exe.obj.name = exe.link.name + L"_0";
		}
		else
		{
			// PCH
			exe.pch = ObjectPCH_VStoBFF(it->objs[0]);
			exe.obj = ObjectList_VStoBFF(it->objs[1]);
			exe.obj.name = exe.link.name + L"_0";
		}

		exe.link.libraries.push_back(exe.obj.name);

		if (it->rc.available())
		{
			exe.rc = VSResourceCompile_To_BffResourceCompile(it->rc);
			exe.rc.name = exe.link.name + L"_rc_0";
			exe.link.libraries.push_back(exe.rc.name);
		}
		
		bff.exes.push_back(exe);
	}
}


void BffObjectList_Output(std::wstringstream &wss, const BffObjectList &o, const BffObjectPCH &pch)
{
/*
ObjectList('name')
{
	.Compiler = 'Compiler-x86'
	.CompilerOptions = '...'
	.CompilerOutputPath = '...'
	.CompilerInputFiles = { 'file1', 'file2', ... }
}
*/

	wss << L"ObjectList('" << o.name << "')"                                 << L"\n"
		<< L"{"                                                              << L"\n";

	if (!pch.inputFile.empty())
	{
		wss << L"  .PCHInputFile = '" << pch.inputFile << L"'"             << L"\n"
			<< L"  .PCHOutputFile = '" << pch.outputFile << L"'"           << L"\n"
			<< L"  .PCHOptions = '" << pch.options << L"'"                 << L"\n";
	}

	wss << L"  .CompilerOptions = '" << o.compilerOptions << "'"             << L"\n"
		<< L"  .CompilerOutputPath = '" << o.compilerOutputPath << "'"       << L"\n"
		<< L"  .CompilerInputFiles = {"                                      << L"\n";
		
	for (auto file = o.compilerInputFiles.cbegin(); file != o.compilerInputFiles.cend(); ++file)
	{
		wss << L"    '" << *file << L"'\n";
	}
	
	wss << L"  }"                                                          << L"\n"
		<< L"}"                                                            << L"\n\n";
}

void BffLink_Output(std::wstringstream &wss, const BffLink &link)
{
/*
Executable('name')
{
  .Linker = 'xx\yy\link.exe'
  .LinkerOutput = ...
  .LinkerOptions = ...
  .Libraries = { ... }
}
*/

	wss << L"Executable('" << link.name << "')"                              << L"\n"
		<< L"{"                                                              << L"\n"
		<< L"  .LinkerOutput = '" << link.linkerOutput << L"'"               << L"\n"
		<< L"  .LinkerOptions = '" << link.linkerOptions << L"'"             << L"\n"
		<< L"  .Libraries = {"                                               << L"\n";
		
	for (auto name = link.libraries.cbegin(); name != link.libraries.cend(); ++name)
	{
		wss << L"    '" << *name << L"'\n";
	}
	
	wss << L"  }"                                                          << L"\n"
		<< L"}"                                                            << L"\n\n";
}

void BffResourceCompile_Output(std::wstringstream &wss, const BffResourceCompile &rc)
{
	if (!rc.available())
		return;

	wss << L"ObjectList('" << rc.name << "')"                                 << L"\n"
		<< L"{"                                                               << L"\n"
		<< L"  Using( .ResourceCompiler )"                                    << L"\n";

	wss << L"  .CompilerOptions = '" << rc.compilerOptions << "'"             << L"\n"
		<< L"  .CompilerOutputPath = '" << rc.compilerOutputPath << "'"       << L"\n"
		<< L"  .CompilerInputFiles = {"                                       << L"\n";
		
	for (auto file = rc.compilerInputFiles.cbegin(); file != rc.compilerInputFiles.cend(); ++file)
	{
		wss << L"    '" << *file << L"'\n";
	}
	
	wss << L"  }"                                                          << L"\n"
		<< L"}"                                                            << L"\n\n";
}

void BffExecutable_Output(std::wstringstream &wss, const BffExecutable &exe)
{
	BffObjectList_Output(wss, exe.obj, exe.pch);
	BffResourceCompile_Output(wss, exe.rc);
	BffLink_Output(wss, exe.link);
}

void BffLibrary_Output(std::wstringstream &wss, const BffLibrary &library)
{
/*
Library('name')
{
  .PCHInputFile = '...'
  .PCHOutputFile = '...'
  .PCHOptions = '...'

  .Compiler = '...'                  // path to cl.exe
  .CompilerOptions = '...'
  .CompilerOutputPath = '...'
  .CompilerInputFiles = {
    '...'
  }

  .Librarian = '...'                 // path to lib.exe
  .LibrarianOptions = '...'
  .LibrarianOutput  = '...'

}
*/
	const BffObjectList &o  = library.obj;
	const BffObjectPCH &pch = library.pch;
	const BffLib &lib = library.lib;

	wss << L"Library('" << lib.name << L"')"                               << L"\n"
		<< L"{"                                                            << L"\n";

	// pch
	if (!pch.inputFile.empty())
	{
		wss << L"  .PCHInputFile = '" << pch.inputFile << L"'"             << L"\n"
			<< L"  .PCHOutputFile = '" << pch.outputFile << L"'"           << L"\n"
			<< L"  .PCHOptions = '" << pch.options << L"'"                 << L"\n\n";
	}

	// compile
	wss << L"  .CompilerOptions = '" << o.compilerOptions << "'"             << L"\n"
		<< L"  .CompilerOutputPath = '" << o.compilerOutputPath << "'"       << L"\n"
		<< L"  .CompilerInputFiles = {"                                      << L"\n";
		
	for (auto file = o.compilerInputFiles.cbegin(); file != o.compilerInputFiles.cend(); ++file)
	{
		wss << L"    '" << *file << L"'\n";
	}
	
	wss << L"  }"                                                          << L"\n\n";

	// lib
	wss << L"  .LibrarianOptions = '" << lib.librarianOptions << L"'"      << L"\n"
		<< L"  .LibrarianOutput = '" << lib.librarianOutput << L"'"        << L"\n";

	wss << L"}"                                                            << L"\n\n";

}

void BffCompiler_Output(std::wstringstream& wss, const Compiler& compiler)
{
	// Compiler
	std::wstring compilerTag(L"'Compiler-" + compiler.platform + L"'");
	if (compiler.vsInstallDir.find(L"Visual Studio 12") != std::wstring::npos)
	{
		// VS2013-x86
		wss << L".VSBasePath = '" << compiler.vsInstallDir << "'"                      << L"\n"
			<< L".WindowsSDKBasePath = '" << compiler.windowsSdkDir << "'"             << L"\n"
			<< L"Compiler(" << compilerTag << ")"                                      << L"\n"
			<< L"{"                                                                    << L"\n"
			<< L"  .Executable = '$VSBasePath$\\VC\\bin\\cl.exe'"                      << L"\n"
			<< L"  .ExtraFiles = {"                                                    << L"\n"
			<< L"    '$VSBasePath$\\VC\\bin\\c1.dll'"                                  << L"\n"
			<< L"    '$VSBasePath$\\VC\\bin\\c1ast.dll'"                               << L"\n"
			<< L"    '$VSBasePath$\\VC\\bin\\c1xx.dll'"                                << L"\n"
			<< L"    '$VSBasePath$\\VC\\bin\\c1xxast.dll'"                             << L"\n"
			<< L"    '$VSBasePath$\\VC\\bin\\c2.dll'"                                  << L"\n"
			<< L"    '$VSBasePath$\\VC\\bin\\msobj120.dll'"                            << L"\n"
			<< L"    '$VSBasePath$\\VC\\bin\\mspdb120.dll'"                            << L"\n"
			<< L"    '$VSBasePath$\\VC\\bin\\mspdbsrv.exe'"                            << L"\n"
			<< L"    '$VSBasePath$\\VC\\bin\\mspdbcore.dll'"                           << L"\n"
			<< L"    '$VSBasePath$\\VC\\bin\\mspft120.dll'"                            << L"\n"
			<< L"    '$VSBasePath$\\VC\\bin\\1033\\clui.dll'"                          << L"\n"
			<< L"    '$VSBasePath$\\VC\\redist\\x86\\Microsoft.VC120.CRT\\msvcp120.dll'"    << L"\n"
			<< L"    '$VSBasePath$\\VC\\redist\\x86\\Microsoft.VC120.CRT\\msvcr120.dll'"    << L"\n"
			<< L"    '$VSBasePath$\\VC\\redist\\x86\\Microsoft.VC120.CRT\\vccorlib120.dll'" << L"\n"
			<< L"  }"                                                                  << L"\n"
			<< L"}"                                                                    << L"\n"
			<< L".Linker    = '$VSBasePath$\\VC\\bin\\link.exe'"                       << L"\n"
			<< L".ResourceCompiler = ["                                                << L"\n"
			<< L"  .Compiler = '$WindowsSDKBasePath$\\bin\\x86\\rc.exe'"               << L"\n"
			<< L"  .CompilerOutputExtension = '.res'"                                  << L"\n"
			<< L"]"                                                                    << L"\n\n";
	}
	else if (compiler.vsInstallDir.find(L"Visual Studio 14") != std::wstring::npos)
	{
		// TODO
	}
	else
	{
		// VS2012-x86 and default
		wss << L".VSBasePath = '" << compiler.vsInstallDir << "'"                      << L"\n"
			<< L".WindowsSDKBasePath = '" << compiler.windowsSdkDir << "'"             << L"\n"
			<< L"Compiler(" << compilerTag << ")"                                      << L"\n"
			<< L"{"                                                                    << L"\n"
			<< L"  .Executable = '$VSBasePath$\\VC\\bin\\cl.exe'"                      << L"\n"
			<< L"  .ExtraFiles = {"                                                    << L"\n"
			<< L"    '$VSBasePath$\\VC\\bin\\c1.dll'"                                  << L"\n"
			<< L"    '$VSBasePath$\\VC\\bin\\c1ast.dll'"                               << L"\n"
			<< L"    '$VSBasePath$\\VC\\bin\\c1xx.dll'"                                << L"\n"
			<< L"    '$VSBasePath$\\VC\\bin\\c1xxast.dll'"                             << L"\n"
			<< L"    '$VSBasePath$\\VC\\bin\\c2.dll'"                                  << L"\n"
			<< L"    '$VSBasePath$\\Common7\\IDE\\msobj110.dll'"                       << L"\n"
			<< L"    '$VSBasePath$\\Common7\\IDE\\mspdb110.dll'"                       << L"\n"
			<< L"    '$VSBasePath$\\Common7\\IDE\\mspdbsrv.exe'"                       << L"\n"
			<< L"    '$VSBasePath$\\Common7\\IDE\\mspdbcore.dll'"                      << L"\n"
			<< L"    '$VSBasePath$\\VC\\bin\\mspft110.dll'"                            << L"\n"
			<< L"    '$VSBasePath$\\VC\\bin\\1033\\clui.dll'"                          << L"\n"
			<< L"    '$VSBasePath$\\VC\\redist\\x86\\Microsoft.VC110.CRT\\msvcp110.dll'"    << L"\n"
			<< L"    '$VSBasePath$\\VC\\redist\\x86\\Microsoft.VC110.CRT\\msvcr110.dll'"    << L"\n"
			<< L"    '$VSBasePath$\\VC\\redist\\x86\\Microsoft.VC110.CRT\\vccorlib110.dll'" << L"\n"
			<< L"  }"                                                                  << L"\n"
			<< L"}"                                                                    << L"\n"
			<< L".Compiler  = " << compilerTag                                         << L"\n"
			<< L".Librarian = '$VSBasePath$\\VC\\bin\\lib.exe'"                        << L"\n"
			<< L".Linker    = '$VSBasePath$\\VC\\bin\\link.exe'"                       << L"\n"
			<< L".ResourceCompiler = ["                                                << L"\n"
			<< L"  .Compiler = '$WindowsSDKBasePath$\\bin\\x86\\rc.exe'"               << L"\n"
			<< L"  .CompilerOutputExtension = '.res'"                                  << L"\n"
			<< L"]"                                                                    << L"\n\n";
	}
}

std::wstring BFFtoOutput(const BffContext& bff)
{
	std::wstringstream wss;

	// Settings
	wss << L"Settings"                                            << L"\n"
		<< L"{"                                                   << L"\n"
		<< L"  .Environment = {"                                  << L"\n"
		<< L"    'PATH=" << bff.settings.PATH << "'"              << L"\n"
		<< L"    'INCLUDE=" << bff.settings.INCLUDE << "'"        << L"\n"
		<< L"    'LIB=" << bff.settings.LIB << "'"                << L"\n"
		<< L"    'TMP=" << bff.settings.SystemRoot << "\\Temp'"   << L"\n"
		<< L"    'SystemRoot=" << bff.settings.SystemRoot << "'"  << L"\n"
		<< L"  }"                                                 << L"\n"
		<< L"  .CachePath = 'C:\\.fbuild.cache'"                  << L"\n"
		<< L"}"                                                   << L"\n\n";

	// Compiler
	BffCompiler_Output(wss, bff.compiler);

	// Library
	for (auto library = bff.libs.cbegin(); library != bff.libs.cend(); ++library)
	{
		BffLibrary_Output(wss, *library);
	}

	// Executable
	for (auto exe = bff.exes.cbegin(); exe != bff.exes.cend(); ++exe)
	{
		BffExecutable_Output(wss, *exe);
	}

	wss << L"Alias('All')"                                                        << L"\n"
		<< L"{"                                                                   << L"\n"
		<< L"  .Targets = {"                                                      << L"\n";
	for (auto exe = bff.exes.cbegin(); exe != bff.exes.cend(); ++exe)
	{
		wss << L"    '" << exe->link.name << "'"                                  << L"\n";
	}
	wss << L"  }"                                                                 << L"\n"
		<< L"}"                                                                   << L"\n";
	return wss.str();
}
