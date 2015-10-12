#include "Tokenizer.hpp"

namespace VSBFF { namespace Parser {

enum TOKEN_STATE {
	T_NORMAL,
	T_LEFT_QUOT,
	T_RIGHT_QUOT,
};

static void tokenSplit(const std::wstring& s, std::vector<std::wstring>& ret)
{
	// D:\myprj\vs2bff\bin\Debug\CL.exe /c /ZI /nologo /W3 /WX- /Od /Oy- /D WIN32 /D _DEBUG /D _CONSOLE /D _UNICODE /D UNICODE /Gm /EHsc /RTC1 /MDd /GS /fp:precise /Zc:wchar_t /Zc:forScope /Fo"D:\myprj\vs2bff\tests\01_simple\vs2012\..\build\HelloWorld\Debug\\" /Fd"D:\myprj\vs2bff\tests\01_simple\vs2012\..\build\HelloWorld\Debug\vc110.pdb" /Gd /TP /analyze- /errorReport:queue ..\..\src\HelloWorld.cpp
	TOKEN_STATE state = T_NORMAL;
	std::wstring word;
	for (size_t i = 0; i < s.length(); ++i)
	{
		wchar_t c = s[i];
		if (state == T_NORMAL) {
			if (iswspace(c)) { ret.push_back(word); word.clear(); }
			else if (c == L'"') { state = T_LEFT_QUOT; word.append(1, c); }
			else word.append(1, c);
		}
		else if (state == T_LEFT_QUOT) {
			word.append(1, c);
			if (c == L'"') state = T_NORMAL;
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