#include "BffCommon.hpp"
#include "BffContext.hpp"

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

// begin
// platform
// vsInstallDir
// PATH
// INCLUDE
// SystemRoot
void handleBegin(VSContext &vscontext, void *conv)
{
	vscontext.compiler.platform     = recvOneMsg(conv);
	vscontext.compiler.vsInstallDir = recvOneMsg(conv);
	
	vscontext.settings.PATH         = recvOneMsg(conv);
	vscontext.settings.INCLUDE      = recvOneMsg(conv);
	vscontext.settings.LIB          = recvOneMsg(conv);
	vscontext.settings.SystemRoot   = recvOneMsg(conv);
}

// cl
// workingDir
// cl args
void handleCL(VSContext &vscontext, void *conv)
{
	VSObjectList o;
	o.workingDir = recvOneMsg(conv);
	o.cmdline = recvOneMsg(conv);

	vscontext.lastObjs.push_back(o);
}

// link
// workingDir
// link.exe args
void handleLink(VSContext &vscontext, void *conv)
{
	VSLink l;
	l.workingDir = recvOneMsg(conv);
	l.cmdline = recvOneMsg(conv);

	VSExecutable exe;
	exe.objs = vscontext.lastObjs;
	exe.link = l;

	vscontext.lastObjs.clear();
	vscontext.exes.push_back(exe);
}

// lib
// workingDir
// lib.exe args
void handleLib(VSContext &vscontext, void *conv)
{
	VSLib l;
	l.workingDir = recvOneMsg(conv);
	l.cmdline = recvOneMsg(conv);

	VSLibrary library;
	library.objs = vscontext.lastObjs;
	library.lib = l;

	vscontext.lastObjs.clear();
	vscontext.libs.push_back(library);
}

// end
void handleEnd(VSContext &vscontext, void *conv)
{
	BffContext bffcontext;
	VStoBFF(vscontext, bffcontext);
	wprintf(BFFtoOutput(bffcontext).c_str());
}


int wmain(int argc, wchar_t *argv[])
{
	//Tokenizer t(L"D:\myprj\vs2bff\bin\Debug\CL.exe /c /ZI /nologo /W3 /WX- /Od /Oy- /D WIN32 /D _DEBUG /D _CONSOLE /D _UNICODE /D UNICODE /Gm /EHsc /RTC1 /MDd /GS /fp:precise /Zc:wchar_t /Zc:forScope /Fo"D:\myprj\vs2bff\tests\01_simple\vs2012\..\build\HelloWorld\Debug\\" /Fd"D:\myprj\vs2bff\tests\01_simple\vs2012\..\build\HelloWorld\Debug\vc110.pdb" /Gd /TP /analyze- /errorReport:queue ..\..\src\HelloWorld.cpp");
	
	const char *address = getenv("VSBFF_CONVERTER_ADDRESS");
	if (!address) address = VSBFF_CONVERTER_ADDRESS;

	void *context = zmq_ctx_new();
	void *conv = zmq_socket(context, ZMQ_PULL);
	zmq_bind(conv, address);

	VSContext vscontext;
	while (1)
	{
		std::wstring s = recvOneMsg(conv);
		VSBFF::Parser::Tokenizer t(s);

		//wprintf(L"RECV: %s\n", t[0].c_str());
		if (t[0] == L"begin")
		{
			handleBegin(vscontext, conv);
		}
		else if (t[0] == L"cl")
		{
			handleCL(vscontext, conv);
		}
		else if (t[0] == L"link")
		{
			handleLink(vscontext, conv);
		}
		else if (t[0] == L"lib")
		{
			handleLib(vscontext, conv);
		}
		else if (t[0] == L"end")
		{
			handleEnd(vscontext, conv);
			break;
		}
	}

	zmq_close(conv);
	zmq_ctx_destroy(context);
	return 0;
}
