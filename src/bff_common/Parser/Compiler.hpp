#pragma once

#include <string>

namespace VSBFF { namespace Parser {

class Compiler {
public:
	Compiler(const std::wstring& platform, const std::wstring& vsInstallDir);
	~Compiler();

	std::wstring getString();

private:
	std::wstring platform_;
	std::wstring vsInstallDir_;

private:
	Compiler(const Compiler& rhs);
	Compiler& operator=(const Compiler& rhs);
};

}}