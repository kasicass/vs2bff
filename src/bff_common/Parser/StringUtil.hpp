#pragma once

#include <string>

namespace VSBFF { namespace Parser {

class StringUtil {
public:
	void tolower(std::wstring& s);
	void toupper(std::wstring& s);
};

}}