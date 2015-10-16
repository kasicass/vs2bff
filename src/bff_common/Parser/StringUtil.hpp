#pragma once

#include <string>

namespace VSBFF { namespace Parser {

class StringUtil {
public:
	void tolower(std::wstring& s);
	void toupper(std::wstring& s);

	bool startswith(const std::wstring& s1, const std::wstring& s2);
	bool endswith(const std::wstring& s1, const std::wstring& s2);
};

}}