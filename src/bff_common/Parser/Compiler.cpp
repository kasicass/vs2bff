/*
.VSBasePath         = 'D:\Program Files (x86)\Microsoft Visual Studio 12.0\'

Compiler( 'Compiler-x86' )
{
	.Root		= '$VSBasePath$\VC\bin'
	.Executable	= '$Root$\cl.exe'
	.ExtraFiles = { '$Root$\c1.dll'
					'$Root$\c1ast.dll',
					'$Root$\c1xx.dll',
					'$Root$\c1xxast.dll',
					'$Root$\c2.dll',
					'$Root$\msobj120.dll'
					'$Root$\mspdb120.dll'
					'$Root$\mspdbsrv.exe'
					'$Root$\mspdbcore.dll'
					'$Root$\mspft120.dll'
					'$Root$\1033\clui.dll'
					'$VSBasePath$\VC\redist\x86\Microsoft.VC120.CRT\msvcp120.dll'
					'$VSBasePath$\VC\redist\x86\Microsoft.VC120.CRT\msvcr120.dll'
					'$VSBasePath$\VC\redist\x86\Microsoft.VC120.CRT\vccorlib120.dll'
				  }
}

.Compiler         = 'Compiler-x86'
.Librarian        = '$VSBasePath$\VC\bin\lib.exe'
.Linker           = '$VSBasePath$\VC\bin\link.exe'
*/

#include "Compiler.hpp"
#include <sstream>

namespace VSBFF { namespace Parser {

Compiler::Compiler(const std::wstring& platform, const std::wstring& vsInstallDir) :
	platform_(platform),
	vsInstallDir_(vsInstallDir)
{

}

Compiler::~Compiler()
{
}

std::wstring Compiler::getString()
{
	std::wstring compilerTag(L"Compiler-" + platform_);

	std::wstringstream wss;
	wss << ".VSBasePath = '" << vsInstallDir_ << "'"                              << "\n"
		<< "Compiler('" << compilerTag << "')"                                    << "\n"
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
		<< "}"                                                                    << "\n";

	return wss.str();
}

}}