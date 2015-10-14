
#define MYTEST
#if defined(MYTEST)

#include <Windows.h>
#include <Shlwapi.h>
#include <stdio.h>

#pragma comment(lib, "Shlwapi.lib")

int main()
{
	//printf("%d\n", PathIsRelative(L"d:\\myprj\\vs2bff\\tests\\01_simple\\vs2012\\..\\build\\HelloWorld\\Debug\\vc110.pdb"));
	//printf("%d\n", PathIsRelative(L"..\\..\\src\\HelloWorld.cpp"));
	return 0;
}

#else
int main()
{
	return 0;
}
#endif