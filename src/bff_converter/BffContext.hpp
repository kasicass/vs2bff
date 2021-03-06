#pragma once

#include <string>
#include <vector>

struct Compiler {
	std::wstring platform;
	std::wstring vsInstallDir;
	std::wstring windowsSdkDir;
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

struct VSLib { // lib.exe
	std::wstring workingDir;
	std::wstring cmdline;
};

struct VSLibrary {
	std::vector<VSObjectList> objs;
	VSLib lib;
};

struct VSLink { // link.exe
	std::wstring workingDir;
	std::wstring cmdline;
};

struct VSResourceCompile { // rc.exe
	std::wstring workingDir;
	std::wstring cmdline;

	bool available() const { return !cmdline.empty(); }
};

struct VSExecutable {
	std::vector<VSObjectList> objs;
	VSResourceCompile rc;
	VSLink link;
};

struct VSContext {
	Compiler compiler;
	Settings settings;

	std::vector<VSObjectList> lastObjs;
	VSResourceCompile lastRc;

	std::vector<VSLibrary> libs;
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
	std::wstring compilerOptions;
	std::wstring compilerOutputPath;
	std::vector<std::wstring> compilerInputFiles;
};

struct BffLib { // lib.exe
	std::wstring name;
	std::wstring librarianOptions;
	std::wstring librarianOutput;
};

struct BffLibrary {
	BffObjectPCH pch;
	BffObjectList obj;
	BffLib lib;
};

struct BffLink {
	std::wstring name;
	std::wstring linkerOutput;
	std::wstring linkerOptions;
	std::vector<std::wstring> libraries;
};

struct BffResourceCompile { // rc.exe
	std::wstring name;
	std::wstring compilerOptions;
	std::wstring compilerOutputPath;
	std::vector<std::wstring> compilerInputFiles;

	bool available() const { return !compilerInputFiles.empty(); }
};

struct BffExecutable {
	BffObjectPCH pch;
	BffObjectList obj;
	BffResourceCompile rc;
	BffLink link;
};

struct BffContext {
	Compiler compiler;
	Settings settings;
	std::vector<BffLibrary> libs;
	std::vector<BffExecutable> exes;
};

void VStoBFF(const VSContext& vs, BffContext& bff);
std::wstring BFFtoOutput(const BffContext& bff);
