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

bool StringUtil::startswith(const std::wstring& s1, const std::wstring& s2)
{
	return s1.compare(0, s2.size(), s2) == 0;
}

bool StringUtil::endswith(const std::wstring& s1, const std::wstring& s2)
{
	return s1.compare(s1.size() - s2.size(), s2.size(), s2) == 0;
}

}}