
#pragma once

#include "tools.h"

namespace luc {

//auto reset event
class event_x
{
public:
	event_x();
	~event_x();

	bool wait_one(int timeout_milli);
	bool wait_one(int timeout_milli, OUT bool& isTimeout);
	bool set();

private:
	DISALLOW_COPY_AND_ASSIGN(event_x);

private:
	HANDLE _event;
};

}
