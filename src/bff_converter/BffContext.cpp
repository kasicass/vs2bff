#include "BffCommon.hpp"
#include "BffContext.hpp"
#include <assert.h>
#include <sstream>

static BffObjectList ObjectList_VStoBFF(const BffContext& bff, const VSObjectList& in)
{
	// cl.exe /c /ZI /nologo /W3 /WX- /Od /Oy- /D WIN32 /D _DEBUG /D _CONSOLE
	// /D _UNICODE /D UNICODE /Gm /EHsc /RTC1 /MDd /GS /fp:precise /Zc:wchar_t
	// /Zc:forScope /Fo"d:\myprj\vs2bff\tests\01_simple\vs2012\..\build\HelloWorld\Debug\\"
	// /Fd"d:\myprj\vs2bff\tests\01_simple\vs2012\..\build\HelloWorld\Debug\vc110.pdb"
	// /Gd /TP /analyze- /errorReport:queue ..\..\src\HelloWorld.cpp

	BffObjectList out;
	out.name = L"test01";
	out.compiler = L"Compiler-" + bff.compiler.platform;
	
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
				out.compilerOutputPath = token.substr(4, token.length()-1);
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

void VStoBFF(const VSContext& vs, BffContext& bff)
{
	bff.compiler = vs.compiler;
	bff.settings = vs.settings;
	
	for (auto it = vs.objlists.cbegin(); it != vs.objlists.cend(); ++it)
	{
		bff.objlists.push_back(ObjectList_VStoBFF(bff, *it));
	}
}

/*
ObjectList('name')
{
	.Compiler = 'Compiler-x86'
	.CompilerOptions = '...'
	.CompilerOutputPath = '...'
	.CompilerInputFiles = { 'file1', 'file2', ... }
}
*/

std::wstring BFFtoOutput(const BffContext& bff)
{
	std::wstringstream wss;

	// Settings
	wss << L"Settings"                                            << "\n"
		<< L"{"                                                   << "\n"
		<< L"  .Environment = {"                                  << "\n"
		<< L"    'PATH=" << bff.settings.PATH << "'"              << "\n"
		<< L"    'INCLUDE=" << bff.settings.INCLUDE << "'"        << "\n"
		<< L"    'TMP=" << bff.settings.SystemRoot << "\\Temp'"   << "\n"
		<< L"    'SystemRoot=" << bff.settings.SystemRoot << "'"  << "\n"
		<< L"  }"                                                 << "\n"
		<< L"  .CachePath = 'C:\\.fbuild.cache'"                  << "\n"
		<< L"}"                                                   << "\n\n";

	// Compiler
	std::wstring compilerTag(L"'Compiler-" + bff.compiler.platform + L"'");
	wss << ".VSBasePath = '" << bff.compiler.vsInstallDir << "'"                  << "\n"
		<< "Compiler(" << compilerTag << ")"                                      << "\n"
		<< "{"                                                                    << "\n"
		<< "  .Executable = '$VSBasePath$\\VC\\bin\\cl.exe'"                      << "\n"
		<< "  .ExtraFiles = {"                                                    << "\n"
		<< "    '$VSBasePath$\\VC\\bin\\c1.dll'"                                  << "\n"
		<< "    '$VSBasePath$\\VC\\bin\\c1ast.dll'"                               << "\n"
		<< "    '$VSBasePath$\\VC\\bin\\c1xx.dll'"                                << "\n"
		<< "    '$VSBasePath$\\VC\\bin\\c1xxast.dll'"                             << "\n"
		<< "    '$VSBasePath$\\VC\\bin\\c2.dll'"                                  << "\n"
		<< "    '$VSBasePath$\\VC\\bin\\msobj120.dll'"                            << "\n"
		<< "    '$VSBasePath$\\VC\\bin\\mspdb120.dll'"                            << "\n"
		<< "    '$VSBasePath$\\VC\\bin\\mspdbsrv.exe'"                            << "\n"
		<< "    '$VSBasePath$\\VC\\bin\\mspdbcore.dll'"                           << "\n"
		<< "    '$VSBasePath$\\VC\\bin\\mspft120.dll'"                            << "\n"
		<< "    '$VSBasePath$\\VC\\bin\\1033\\clui.dll'"                          << "\n"
		<< "    '$VSBasePath$\\VC\\redist\\x86\\Microsoft.VC120.CRT\\msvcp120.dll'"    << "\n"
		<< "    '$VSBasePath$\\VC\\redist\\x86\\Microsoft.VC120.CRT\\msvcr120.dll'"    << "\n"
		<< "    '$VSBasePath$\\VC\\redist\\x86\\Microsoft.VC120.CRT\\vccorlib120.dll'" << "\n"
		<< "  }"                                                                  << "\n"
		<< "}"                                                                    << "\n"
		<< ".Compiler  = " << compilerTag                                         << "\n"
		<< ".Librarian = '$VSBasePath$\\VC\\bin\\lib.exe'"                        << "\n"
		<< ".Linker    = '$VSBasePath$\\VC\\bin\\link.exe'"                       << "\n\n";

	// ObjectList
	BffObjectList o = bff.objlists[0];
	wss << L"ObjectList('" << o.name << "')"                               << "\n"
		<< L"{"                                                            << "\n"
		<< L"  .Compiler = " << compilerTag                                  << "\n"
		<< L"  .CompilerOptions = '" << o.compilerOptions << "'"             << "\n"
		<< L"  .CompilerOutputPath = '" << o.compilerOutputPath << "'"       << "\n"
		<< L"  .CompilerInputFiles = {"                                      << "\n";
		
	for (auto file = o.compilerInputFiles.cbegin(); file != o.compilerInputFiles.cend(); ++file)
	{
		wss << L"    '" << *file << L"'\n";
	}
	
	wss << L"  }"                                                          << "\n"
		<< L"}"                                                            << "\n\n";

	return wss.str();
}
