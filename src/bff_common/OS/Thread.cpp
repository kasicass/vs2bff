#include "Thread.hpp"
#include <process.h>
#include <assert.h>

namespace VSBFF { namespace OS {

Thread::Thread() : hThread_(NULL)
{
}

Thread::~Thread()
{
	wait();
}

unsigned int __stdcall Thread::threadFunc(void* arg)
{
	Thread *thr = (Thread *)arg;
	thr->workFunc();
	return 0;
}

void Thread::run()
{
	assert(hThread_ == NULL);
	hThread_ = HANDLE( _beginthreadex(NULL, 0, Thread::threadFunc, this, 0, NULL) );
}

void Thread::wait()
{
	if (hThread_)
	{
		::WaitForSingleObject(hThread_, INFINITE);
		::CloseHandle(hThread_);
		hThread_ = NULL;
	}
}

void Thread::terminate()
{
	if (hThread_)
	{
		::TerminateThread(hThread_, 0);
	}
}

}}
