#include "MyLib.hpp"
#include "MyLibPCH.h"

int main()
{
	printMe("Baby!");

	std::string v = "VVV";
	printMe(v);

	printMePCH();
}