#include "BffCommon.hpp"

std::wstring readFile(const std::wstring& filename)
{
	FILE *fp = _wfopen(filename.c_str(), L"rb");
	if (!fp) return std::wstring();

	fseek(fp, 0, SEEK_END);
	long sz = ftell(fp);

	fseek(fp, 0, SEEK_SET);
	wchar_t *buf = (wchar_t*)malloc(sz);
	fread(buf, sz, 1, fp);

	std::wstring text;
	text.assign(buf+1, sz/2-1);  // buf+1, skip BOM

	free(buf);
	fclose(fp);

	return text;
}

std::wstring getCurrDir()
{
	DWORD n = GetCurrentDirectory(0, NULL);
	std::wstring path;
	path.resize(n-1);
	GetCurrentDirectory(n, (wchar_t*)path.c_str());
	return path;
}

int wmain(int argc, wchar_t *argv[])
{
	const char *address = getenv("VSBFF_CONVERTER_ADDRESS");
	if (!address) address = VSBFF_CONVERTER_ADDRESS;

	void *context = zmq_ctx_new();
	void *logger = zmq_socket(context, ZMQ_PUSH);
	zmq_connect(logger, address);

	std::wstringstream output;
	output << L"PWD:" << getCurrDir() << L"\n";

	for (int i = 0; i < argc; ++i)
	{
		if (argv[i][0] == L'@')
		{
			output << readFile(argv[i] + 1);
		}
		else
		{
			output << argv[i];
			output << L" ";
		}
	}
	output << L"\n";

	std::wstring text = output.str();
	zmq_send(logger, text.c_str(), text.length()*2, 0);

	zmq_close(logger);
	zmq_ctx_destroy(context);
	return 0;
}
