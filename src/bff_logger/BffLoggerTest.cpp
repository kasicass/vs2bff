#include "BffCommon.hpp"
#include <zmq.h>
#include <assert.h>

int main()
{
	void *context = zmq_ctx_new();
	void *logger = zmq_socket(context, ZMQ_PUSH);
	zmq_connect(logger, BFF_LOGGER_ADDRESS);

	const wchar_t *text = L"Hello Boy!\n";
	int r = zmq_send(logger, text, wcslen(text)*2, 0);
	assert(r != -1);

	zmq_close(logger);
	zmq_ctx_destroy(context);
	return 0;
}