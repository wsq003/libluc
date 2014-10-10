
#pragma once

#include "event.h"

namespace luc {

event_x::event_x()
{
#ifdef _WIN32
	_event = CreateEvent(NULL, FALSE, FALSE, NULL);
#else
	pthread_mutex();
	pthread_condition_wait();
#endif
}

event_x::~event_x()
{
	if (_event)
	{
		CloseHandle(_event);
		_event = NULL;
	}
}

bool event_x::wait_one(int timeout_milli)
{
	int ret = WaitForSingleObject(_event, timeout_milli);
	if (ret == WAIT_FAILED)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool event_x::wait_one(int timeout_milli, OUT bool& isTimeout)
{
	isTimeout = false;

	int ret = WaitForSingleObject(_event, timeout_milli);
	if (ret == WAIT_FAILED)
	{
		return false;
	}
	else
	{
		if (ret == WAIT_TIMEOUT)
		{
			isTimeout = true;
		}
		return true;
	}
}

bool event_x::set()
{
	bool ret = (SetEvent(_event) == TRUE);
	return ret;
}

}
