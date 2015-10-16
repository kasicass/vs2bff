#include "Tokenizer.hpp"

namespace VSBFF { namespace Parser {

enum TOKEN_STATE {
	T_NORMAL,
	T_LEFT_QUOT,
	T_RN, // \r\n
};

static void tokenSplit(const std::wstring& s, std::vector<std::wstring>& ret)
{
	// D:\myprj\vs2bff\bin\Debug\CL.exe ... a.cpp\r\n
	// b.cpp\r\n
	// c.cpp\r\n
	TOKEN_STATE state = T_NORMAL;
	std::wstring word;
	for (size_t i = 0; i < s.length(); ++i)
	{
		wchar_t c = s[i];
		if (state == T_NORMAL)
		{
			if (c == L'\r' && i+1 < s.length() && s[i+1] == L'\n')
			{
				state = T_RN;
				++i;
			}
			else if (iswspace(c))
			{
				ret.push_back(word);
				word.clear();
			}
			else if (c == L'"')
			{
				state = T_LEFT_QUOT;
				word.append(1, c);
			}
			else
			{
				word.append(1, c);
			}
		}
		else if (state == T_LEFT_QUOT)
		{
			word.append(1, c);
			if (c == L'"') state = T_NORMAL;
		}
		else if (state == T_RN)
		{
			if (c == L'\r' && i+1 < s.length() && s[i+1] == L'\n')
			{
				ret.push_back(word);
				word.clear();
				++i;
			}
			else
			{
				word.append(1, c);
			}
		}
	}
	
	if (!word.empty())
		ret.push_back(word);
}

Tokenizer::Tokenizer(const std::wstring& s)
{
	tokenSplit(s, tokens_);
}

Tokenizer::~Tokenizer()
{
}

unsigned int Tokenizer::count()
{
	return tokens_.size();
}

std::wstring Tokenizer::operator[](unsigned int n)
{
	return tokens_[n];
}

}}