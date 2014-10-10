
/*

sample:

unsigned worker_thread(void* para)
{
	thread_x* th = (thread_x*)para;
	while (th->work_event.wait_one(10))
	{
		if (th->quit_flag)
		{
			break;
		}

		char* data = NULL;
		int len = 0;
		while (th->buffer.peek_data(data, len))
		{
			//do something

			th->buffer.remove_data(len);
		}
	}

	return 0;
}

int main()
{
	thread_x* th = thread_x::start_thread(worker_thread);
	
	Sleep(10000);

	th->stop();

	return 0;
}

*/


#pragma once

#include "tools.h"
#include "event.h"

namespace luc {

//
typedef unsigned (*funcThreadProc)(void* para);

//thread extended class
class thread_x
{
	//
	class buffer_x
	{
	public:
		buffer_x();
		~buffer_x();

		bool put_data(char* data, int len);
		bool peek_data(char*&data, int& len);
		bool remove_data(int len);
	};

public:
	static thread_x* start_thread(funcThreadProc func, void* para);
private:
	static unsigned __stdcall internal_thread_proc(void* para);

private:
	DISALLOW_COPY_AND_ASSIGN(thread_x);
	thread_x(funcThreadProc func, void* para);
	void start();
	void join();

private:
	funcThreadProc _thread_proc;
	HANDLE _th;
	void* _para;

public:
	~thread_x();

	void stop();

	bool		_quit_flag;
	event_x		_work_event;
	buffer_x	_buffer;
};

}
