// class MyThread : Os::Thread
// {
// private:
//   virtual void workFunc() {
//     // do sth.
//   }
// };
// 
// MyThread thr;
// thr.run();
// thr.wait();

#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace VSBFF { namespace OS {

class Thread
{
public:
	Thread();
	virtual ~Thread();

	void run();
	void wait();
	void terminate();

private:
	virtual void workFunc() = 0;

	static unsigned int __stdcall threadFunc(void* arg);

private:
	HANDLE hThread_;
};

}}
