#include "StringUtil.hpp"
#include <algorithm>

namespace VSBFF { namespace Parser {

void StringUtil::tolower(std::wstring& s)
{
	std::transform(s.begin(), s.end(), s.begin(), ::towlower);
}

void StringUtil::toupper(std::wstring& s)
{
	std::transform(s.begin(), s.end(), s.begin(), ::towupper);
}

}}