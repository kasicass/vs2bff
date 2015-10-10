// bff_logger --logfile <filename> --dbgview
//
// --logfile, write to logfile
// --dbgview, output to dbgview
// --help, print usage

#include "BffCommon.hpp"
#include <zmq.h>
#include <stdlib.h>
#include <string>

#define LOG_BUFSIZE 1024

static std::wstring g_logfilename;
static bool g_outputToDbgview = false;

void parseArgs(int argc, wchar_t *argv[])
{
	for (int i = 1; i < argc; ++i)
	{
		if (wcscmp(argv[i], L"--logfile") == 0 && i+1 < argc) {
			g_logfilename = argv[i+1];
			++i;
		}
		else if (wcscmp(argv[i], L"--dbgview") == 0) {
			g_outputToDbgview = true;
		}
		else if (wcscmp(argv[i], L"--help") == 0) {
			printf("bff_logger --logfile <filename> --dbgview\n");
			printf("  --logfile, write to logfile\n");
			printf("  --dbgview, output to dbgview\n");
			exit(0);
		}
	}
}

class WriteToFileThread : public VSBFF::OS::Thread {
public:
	WriteToFileThread(void *context) : context_(context) {}
	virtual ~WriteToFileThread() {}

private:
	virtual void workFunc() {
		void *in = zmq_socket(context_, ZMQ_PULL);
		zmq_connect(in, "inproc://write-to-file");

		FILE *fp = NULL;
		if (!g_logfilename.empty()) {
			fp = _wfopen(g_logfilename.c_str(), L"w+,ccs=UTF-8");
		}

		wchar_t *buf = (wchar_t *)malloc(LOG_BUFSIZE);
		while (1) {
			int bytes = zmq_recv(in, buf, LOG_BUFSIZE-2, 0);
			if (bytes == -1)
				break;
		
			buf[bytes/2] = 0;

			if (fp) { fwrite(buf, bytes, 1, fp); fflush(fp); }
			if (g_outputToDbgview) { OutputDebugString(buf); }
			wprintf(buf);
		}
		free(buf);

		if (fp) fclose(fp);
		zmq_close(in);
	}

private:
	void *context_;
};



int wmain(int argc, wchar_t *argv[])
{
	parseArgs(argc, argv);

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