// send command to bff_converter.exe
// bff_submit.exe start
// bff_submit.exe end

#include "BffCommon.hpp"

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

	zmq_send(conv, argv[1], wcslen(argv[1])*2, 0);

	zmq_close(conv);
	zmq_ctx_destroy(context);
	return 0;
}
