
#include "thread.h"

namespace luc {

//////////////////////////////////////////////////////////////////////////
// thread_x::buffer_x

thread_x::buffer_x::buffer_x()
{

}

thread_x::buffer_x::~buffer_x()
{

}


//////////////////////////////////////////////////////////////////////////
// thread_x

thread_x* thread_x::start_thread(funcThreadProc func, void* para)
{
	thread_x* th = new thread_x(func, para);
	th->start();
	return th;
}

thread_x::thread_x(funcThreadProc func, void* para)
{
	this->_thread_proc = func;
	this->_para = para;
}

thread_x::~thread_x()
{

}

void thread_x::start()
{
	_th = (HANDLE)_beginthreadex(NULL, 0, thread_x::internal_thread_proc, this, 0, NULL);
}

unsigned __stdcall thread_x::internal_thread_proc(void* para)
{
	thread_x* th = (thread_x*)para;

	if (th->_thread_proc)
	{
		th->_thread_proc(th->_para);
	}

	return 0;
}

void thread_x::stop()
{
	this->_quit_flag = true;
	this->_work_event.set();
	this->join();
}

void thread_x::join()
{
	WaitForSingleObject(_th, INFINITE);
}

}
