#include "BffCommon.hpp"
#include <zmq.h>
#include <stdlib.h>

#define LOG_BUFSIZE 1024

class WriteToFileThread : public VSBFF::OS::Thread {
public:
	WriteToFileThread(void *context) : context_(context) {}
	virtual ~WriteToFileThread() {}

private:
	virtual void workFunc() {
		void *in = zmq_socket(context_, ZMQ_PULL);
		zmq_connect(in, "inproc://write-to-file");

		wchar_t *buf = (wchar_t *)malloc(LOG_BUFSIZE);
		while (1) {
			int bytes = zmq_recv(in, buf, LOG_BUFSIZE-2, 0);
			if (bytes == -1)
				break;
		
			buf[bytes/2] = 0;
			if (bytes == 6 && wcscmp(buf, L"END") == 0)
				break;

			wprintf(buf);
		}
		free(buf);

		zmq_close(in);
	}

private:
	void *context_;
};

int main()
{
	void *context = zmq_ctx_new();
	void *logger = zmq_socket(context, ZMQ_PULL);
	zmq_bind(logger, BFF_LOGGER_ADDRESS);

	void *out = zmq_socket(context, ZMQ_PUSH);
	zmq_bind(out, "inproc://write-to-file");

	WriteToFileThread *thr = new WriteToFileThread(context);
	thr->run();

	void *buf = malloc(LOG_BUFSIZE);
	while (1)
	{
		int bytes = zmq_recv(logger, buf, LOG_BUFSIZE-2, 0);
		if (bytes == -1)
			break;
		
		zmq_send(out, buf, bytes, 0);
	}
	free(buf);

	delete thr;
	zmq_close(out);
	zmq_close(logger);
	zmq_ctx_destroy(context);
	return 0;
}