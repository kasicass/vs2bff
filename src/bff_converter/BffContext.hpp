#pragma once

#include <string>
#include <vector>

struct Compiler {
	std::wstring platform;
	std::wstring vsInstallDir;
};

struct Settings {
	std::wstring PATH;
	std::wstring INCLUDE;
	std::wstring SystemRoot;
};

struct VSObjectList {
	std::wstring workingDir;
	std::wstring cmdline;
};

struct VSContext {
	Compiler compiler;
	Settings settings;
	std::vector<VSObjectList> objlists;
};

struct BffObjectList {
	std::wstring name;
	std::wstring compiler;
	std::wstring compilerOptions;
	std::wstring compilerOutputPath;
	std::vector<std::wstring> compilerInputFiles;
};

struct BffContext {
	Compiler compiler;
	Settings settings;
	std::vector<BffObjectList> objlists;
};

void VStoBFF(const VSContext& vs, BffContext& bff);
std::wstring BFFtoOutput(const BffContext& bff);
