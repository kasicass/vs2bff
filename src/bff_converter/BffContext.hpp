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
	std::wstring LIB;
	std::wstring SystemRoot;
};

struct VSObjectList {
	std::wstring workingDir;
	std::wstring cmdline;
};

struct VSLink {
	std::wstring workingDir;
	std::wstring cmdline;
};

struct VSExecutable {
	std::vector<VSObjectList> objs;
	VSLink link;
};

struct VSContext {
	Compiler compiler;
	Settings settings;
	std::vector<VSObjectList> lastObjs;
	std::vector<VSExecutable> exes;
};

// ----------------

struct BffObjectPCH {
	std::wstring inputFile;
	std::wstring outputFile;
	std::wstring options;
};

struct BffObjectList {
	std::wstring name;
	std::wstring compiler;
	std::wstring compilerOptions;
	std::wstring compilerOutputPath;
	std::vector<std::wstring> compilerInputFiles;
};

struct BffLink {
	std::wstring name;
	std::wstring linkerOutput;
	std::wstring linkerOptions;
	std::vector<std::wstring> libraries;
};

struct BffExecutable {
	BffObjectPCH pch;
	BffObjectList obj;
	BffLink link;
};

struct BffContext {
	Compiler compiler;
	Settings settings;
	std::vector<BffExecutable> exes;
};

void VStoBFF(const VSContext& vs, BffContext& bff);
std::wstring BFFtoOutput(const BffContext& bff);
