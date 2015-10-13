// send command to bff_converter.exe
// bff_submit.exe start
// bff_submit.exe end

#include "BffCommon.hpp"

// start Platform VSINSTALLDIR
void sendStart(void *conv)
{
	zmq_send(conv, L"start", 5*2, 0);

	zmq_send(conv, L"x86", 3*2, 0);

	wchar_t *value = _wgetenv(L"VSINSTALLDIR");
	if (value)
		zmq_send(conv, value, wcslen(value)*2, 0);
	else
		zmq_send(conv, L"none", 4*2, 0);
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

	if (wcscmp(argv[1], L"start") == 0) sendStart(conv);
	else if (wcscmp(argv[1], L"end") == 0) sendEnd(conv);

	zmq_close(conv);
	zmq_ctx_destroy(context);
	return 0;
}
