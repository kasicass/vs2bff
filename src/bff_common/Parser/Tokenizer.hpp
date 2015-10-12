#pragma once

#include <vector>
#include <string>

namespace VSBFF { namespace Parser {

class Tokenizer {
public:
	Tokenizer(const std::wstring& s);
	~Tokenizer();

	unsigned int count();
	std::wstring operator[](unsigned int n);

private:
	std::vector<std::wstring> tokens_;

private:
	Tokenizer(const Tokenizer& rhs);
	Tokenizer& operator=(const Tokenizer& rhs);
};

}}