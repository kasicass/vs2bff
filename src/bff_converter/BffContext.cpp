#include "BffCommon.hpp"
#include "BffContext.hpp"
#include <assert.h>
#include <sstream>

static BffObjectList ObjectList_VStoBFF(const VSObjectList& in, const std::wstring& platform)
{
	// cl.exe /c /ZI /nologo /W3 /WX- /Od /Oy- /D WIN32 /D _DEBUG /D _CONSOLE
	// /D _UNICODE /D UNICODE /Gm /EHsc /RTC1 /MDd /GS /fp:precise /Zc:wchar_t
	// /Zc:forScope /Fo"d:\myprj\vs2bff\tests\01_simple\vs2012\..\build\HelloWorld\Debug\\"
	// /Fd"d:\myprj\vs2bff\tests\01_simple\vs2012\..\build\HelloWorld\Debug\vc110.pdb"
	// /Gd /TP /analyze- /errorReport:queue ..\..\src\HelloWorld.cpp

	BffObjectList out;
	out.compiler = L"Compiler-" + platform;
	
	enum {
		IN_FLAG,
		IN_SOURCE,
	} state = IN_FLAG;

	out.compilerOptions = L"%%1 /Fo%%2";
	VSBFF::Parser::Tokenizer t(in.cmdline);
	for (unsigned int i = 0; i < t.count(); ++i)
	{
		if (state == IN_FLAG)
		{
			const std::wstring& token = t[i];
			if (token[0] != L'/')
			{
				state = IN_SOURCE;
				out.compilerInputFiles.push_back(in.workingDir + L"\\" + t[i]);
			}
			else if (token == L"/D") // /D XXX
			{
				out.compilerOptions += L" ";
				out.compilerOptions += token;

				++i;
				out.compilerOptions += L" ";
				out.compilerOptions += t[i];
			}
			else if (token.length() > 3 && token[1] == L'F' && token[2] == L'o')  // /Fo"..."
			{
				// /Fo"..." => ...
				out.compilerOutputPath = token.substr(4, token.length()-1-4);
			}
			else
			{
				out.compilerOptions += L" ";
				out.compilerOptions += token;
			}
		}
		else if (state == IN_SOURCE)
		{
			out.compilerInputFiles.push_back(in.workingDir + L"\\" + t[i]);
		}
	}

	assert(!out.compilerInputFiles.empty() &&
		!out.compilerOptions.empty() &&
		!out.compilerOutputPath.empty());
	return out;
}

static BffLink Link_VStoBFF(const VSLink& link)
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
			}
		}
	}

	out.linkerOptions += L" %%1";
	return out;
}


void VStoBFF(const VSContext& vs, BffContext& bff)
{
	bff.compiler = vs.compiler;
	bff.settings = vs.settings;
	
	for (auto it = vs.exes.cbegin(); it != vs.exes.cend(); ++it)
	{
		BffExecutable exe;
		exe.link = Link_VStoBFF(it->link);

		exe.obj = ObjectList_VStoBFF(it->obj, bff.compiler.platform);
		exe.obj.name = exe.link.name + L"_0";

		exe.link.libraries.push_back(exe.obj.name);
		
		bff.exes.push_back(exe);
	}
}


void BffObjectList_Output(std::wstringstream &wss, const BffObjectList &o)
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
		<< L"{"                                                              << L"\n"
		<< L"  .Compiler = '" << o.compiler << L"'"                          << L"\n"
		<< L"  .CompilerOptions = '" << o.compilerOptions << "'"             << L"\n"
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

void BffExecutable_Output(std::wstringstream &wss, const BffExecutable &exe)
{
	BffObjectList_Output(wss, exe.obj);
	BffLink_Output(wss, exe.link);
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


/* VS2012
					'$VSBasePath$\Common7\IDE\msobj110.dll'
					'$VSBasePath$\Common7\IDE\mspdb110.dll'
					'$VSBasePath$\Common7\IDE\mspdbsrv.exe'
					'$VSBasePath$\Common7\IDE\mspdbcore.dll'
					'$Root$\mspft110.dll'
					'$Root$\1033\clui.dll'
					'$VSBasePath$\VC\redist\x86\Microsoft.VC110.CRT\msvcp110.dll'
					'$VSBasePath$\VC\redist\x86\Microsoft.VC110.CRT\msvcr110.dll'
					'$VSBasePath$\VC\redist\x86\Microsoft.VC110.CRT\vccorlib110.dll'
*/

	// Compiler
	std::wstring compilerTag(L"'Compiler-" + bff.compiler.platform + L"'");
	wss << ".VSBasePath = '" << bff.compiler.vsInstallDir << "'"                  << L"\n"
		<< "Compiler(" << compilerTag << ")"                                      << L"\n"
		<< "{"                                                                    << L"\n"
		<< "  .Executable = '$VSBasePath$\\VC\\bin\\cl.exe'"                      << L"\n"
		<< "  .ExtraFiles = {"                                                    << L"\n"
		<< "    '$VSBasePath$\\VC\\bin\\c1.dll'"                                  << L"\n"
		<< "    '$VSBasePath$\\VC\\bin\\c1ast.dll'"                               << L"\n"
		<< "    '$VSBasePath$\\VC\\bin\\c1xx.dll'"                                << L"\n"
		<< "    '$VSBasePath$\\VC\\bin\\c1xxast.dll'"                             << L"\n"
		<< "    '$VSBasePath$\\VC\\bin\\c2.dll'"                                  << L"\n"
		<< "    '$VSBasePath$\\Common7\\IDE\\msobj110.dll'"                       << L"\n"
		<< "    '$VSBasePath$\\Common7\\IDE\\mspdb110.dll'"                       << L"\n"
		<< "    '$VSBasePath$\\Common7\\IDE\\mspdbsrv.exe'"                       << L"\n"
		<< "    '$VSBasePath$\\Common7\\IDE\\mspdbcore.dll'"                      << L"\n"
		<< "    '$VSBasePath$\\VC\\bin\\mspft110.dll'"                            << L"\n"
		<< "    '$VSBasePath$\\VC\\bin\\1033\\clui.dll'"                          << L"\n"
		<< "    '$VSBasePath$\\VC\\redist\\x86\\Microsoft.VC110.CRT\\msvcp110.dll'"    << L"\n"
		<< "    '$VSBasePath$\\VC\\redist\\x86\\Microsoft.VC110.CRT\\msvcr110.dll'"    << L"\n"
		<< "    '$VSBasePath$\\VC\\redist\\x86\\Microsoft.VC110.CRT\\vccorlib110.dll'" << L"\n"
		<< "  }"                                                                  << L"\n"
		<< "}"                                                                    << L"\n"
		<< ".Compiler  = " << compilerTag                                         << L"\n"
		<< ".Librarian = '$VSBasePath$\\VC\\bin\\lib.exe'"                        << L"\n"
		<< ".Linker    = '$VSBasePath$\\VC\\bin\\link.exe'"                       << L"\n\n";

#if 0  // VS2013
	// Compiler
	std::wstring compilerTag(L"'Compiler-" + bff.compiler.platform + L"'");
	wss << ".VSBasePath = '" << bff.compiler.vsInstallDir << "'"                  << L"\n"
		<< "Compiler(" << compilerTag << ")"                                      << L"\n"
		<< "{"                                                                    << L"\n"
		<< "  .Executable = '$VSBasePath$\\VC\\bin\\cl.exe'"                      << L"\n"
		<< "  .ExtraFiles = {"                                                    << L"\n"
		<< "    '$VSBasePath$\\VC\\bin\\c1.dll'"                                  << L"\n"
		<< "    '$VSBasePath$\\VC\\bin\\c1ast.dll'"                               << L"\n"
		<< "    '$VSBasePath$\\VC\\bin\\c1xx.dll'"                                << L"\n"
		<< "    '$VSBasePath$\\VC\\bin\\c1xxast.dll'"                             << L"\n"
		<< "    '$VSBasePath$\\VC\\bin\\c2.dll'"                                  << L"\n"
		<< "    '$VSBasePath$\\VC\\bin\\msobj120.dll'"                            << L"\n"
		<< "    '$VSBasePath$\\VC\\bin\\mspdb120.dll'"                            << L"\n"
		<< "    '$VSBasePath$\\VC\\bin\\mspdbsrv.exe'"                            << L"\n"
		<< "    '$VSBasePath$\\VC\\bin\\mspdbcore.dll'"                           << L"\n"
		<< "    '$VSBasePath$\\VC\\bin\\mspft120.dll'"                            << L"\n"
		<< "    '$VSBasePath$\\VC\\bin\\1033\\clui.dll'"                          << L"\n"
		<< "    '$VSBasePath$\\VC\\redist\\x86\\Microsoft.VC120.CRT\\msvcp120.dll'"    << L"\n"
		<< "    '$VSBasePath$\\VC\\redist\\x86\\Microsoft.VC120.CRT\\msvcr120.dll'"    << L"\n"
		<< "    '$VSBasePath$\\VC\\redist\\x86\\Microsoft.VC120.CRT\\vccorlib120.dll'" << L"\n"
		<< "  }"                                                                  << L"\n"
		<< "}"                                                                    << L"\n"
		<< ".Compiler  = " << compilerTag                                         << L"\n"
		<< ".Librarian = '$VSBasePath$\\VC\\bin\\lib.exe'"                        << L"\n"
		<< ".Linker    = '$VSBasePath$\\VC\\bin\\link.exe'"                       << L"\n\n";
#endif

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
