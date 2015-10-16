// send command to bff_converter.exe
// bff_submit.exe start
// bff_submit.exe end

#include "BffCommon.hpp"

void sendWStr(void *s, const std::wstring& wstr)
{
	zmq_send(s, wstr.c_str(), wstr.length()*2, 0);
}

// start Platform VSINSTALLDIR
void sendBegin(void *conv)
{
	zmq_send(conv, L"begin", 5*2, 0);

	zmq_send(conv, L"x86", 3*2, 0);

	wchar_t *value = _wgetenv(L"VSINSTALLDIR");
	if (value) sendWStr(conv, value);
	else sendWStr(conv, L"none");

	value = _wgetenv(L"PATH");
	if (value) sendWStr(conv, value);
	else sendWStr(conv, L"none");

	value = _wgetenv(L"INCLUDE");
	if (value) sendWStr(conv, value);
	else sendWStr(conv, L"none");

	value = _wgetenv(L"LIB");
	if (value) sendWStr(conv, value);
	else sendWStr(conv, L"none");

	value = _wgetenv(L"SystemRoot");
	if (value) sendWStr(conv, value);
	else sendWStr(conv, L"none");
}

void sendEnd(void *conv)
{
	zmq_send(conv, L"end", 3*2, 0);
}

int wmain(int argc, wchar_t *argv[])
{
	if (argc < 2)
	{
		printf("bff_submit.exe <command>\n");
		return 0;
	}

	const char *address = getenv("VSBFF_CONVERTER_ADDRESS");
	if (!address) address = VSBFF_CONVERTER_ADDRESS;

	void *context = zmq_ctx_new();
	void *conv = zmq_socket(context, ZMQ_PUSH);
	zmq_connect(conv, address);

	if (wcscmp(argv[1], L"begin") == 0) sendBegin(conv);
	else if (wcscmp(argv[1], L"end") == 0) sendEnd(conv);

	zmq_close(conv);
	zmq_ctx_destroy(context);
	return 0;
}
