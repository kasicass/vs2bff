#include "BffCommon.hpp"

using namespace VSBFF;

#define CONV_BUFSIZE (1024*1024)

std::wstring recvOneMsg(void *s)
{
	std::wstring ret;

	zmq_msg_t msg;
	zmq_msg_init(&msg);
	zmq_msg_recv(&msg, s, 0);
	ret.assign((wchar_t*)zmq_msg_data(&msg), zmq_msg_size(&msg)/2);
	zmq_msg_close(&msg);

	return ret;
}

void handleStart(const Parser::Tokenizer &t, void *conv)
{
	std::wstring platform = recvOneMsg(conv);
	std::wstring vsInstallDir = recvOneMsg(conv);
	Parser::Compiler c(platform, vsInstallDir);
	wprintf(c.getString().c_str());
}

int wmain(int argc, wchar_t *argv[])
{
	//Tokenizer t(L"D:\myprj\vs2bff\bin\Debug\CL.exe /c /ZI /nologo /W3 /WX- /Od /Oy- /D WIN32 /D _DEBUG /D _CONSOLE /D _UNICODE /D UNICODE /Gm /EHsc /RTC1 /MDd /GS /fp:precise /Zc:wchar_t /Zc:forScope /Fo"D:\myprj\vs2bff\tests\01_simple\vs2012\..\build\HelloWorld\Debug\\" /Fd"D:\myprj\vs2bff\tests\01_simple\vs2012\..\build\HelloWorld\Debug\vc110.pdb" /Gd /TP /analyze- /errorReport:queue ..\..\src\HelloWorld.cpp");
	
	const char *address = getenv("VSBFF_CONVERTER_ADDRESS");
	if (!address) address = VSBFF_CONVERTER_ADDRESS;

	void *context = zmq_ctx_new();
	void *conv = zmq_socket(context, ZMQ_PULL);
	zmq_bind(conv, address);

	void *buf = malloc(CONV_BUFSIZE);
	while (1)
	{
		int n = zmq_recv(conv, buf, CONV_BUFSIZE, 0);
		if (n == -1)
			break;

		std::wstring s((wchar_t*)buf, n/2);
		VSBFF::Parser::Tokenizer t(s);

		if (t.count() == 1 && t[0] == L"end")
			break;

		if (t[0] == L"start") handleStart(t, conv);
	}
	free(buf);

	zmq_close(conv);
	zmq_ctx_destroy(context);
	return 0;
}
