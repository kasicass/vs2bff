#include "BffCommon.hpp"

#define CONV_BUFSIZE (1024*1024)

int wmain(int argc, wchar_t *argv[])
{
	//Tokenizer t(L"D:\myprj\vs2bff\bin\Debug\CL.exe /c /ZI /nologo /W3 /WX- /Od /Oy- /D WIN32 /D _DEBUG /D _CONSOLE /D _UNICODE /D UNICODE /Gm /EHsc /RTC1 /MDd /GS /fp:precise /Zc:wchar_t /Zc:forScope /Fo"D:\myprj\vs2bff\tests\01_simple\vs2012\..\build\HelloWorld\Debug\\" /Fd"D:\myprj\vs2bff\tests\01_simple\vs2012\..\build\HelloWorld\Debug\vc110.pdb" /Gd /TP /analyze- /errorReport:queue ..\..\src\HelloWorld.cpp");

	void *context = zmq_ctx_new();
	void *conv = zmq_socket(context, ZMQ_PULL);
	zmq_bind(conv, BFF_CONVERTER_ADDRESS);

	void *buf = malloc(CONV_BUFSIZE);
	while (1)
	{
		int n = zmq_recv(conv, buf, CONV_BUFSIZE, 0);
		if (n == -1)
			break;

		wprintf(L"\n-----\n");
		std::wstring s((wchar_t*)buf, n/2);
		VSBFF::Parser::Tokenizer t(s);

		for (unsigned int i = 0; i < t.count(); ++i)
		{
			wprintf((t[i] + L"\n").c_str());
		}
	}
	free(buf);

	zmq_close(conv);
	zmq_ctx_destroy(context);
	return 0;
}
