#pragma once

#include "platform.h"

namespace luc {

//cross platform CRITICAL_SECTION or pthread_mutex
class sync_obj
{
public:
	sync_obj();
	~sync_obj();

	void lock();
	void unlock();

	bool try_lock();

private:
	DISALLOW_COPY_AND_ASSIGN(sync_obj);

private:
#ifdef _WIN32
	CRITICAL_SECTION sec;
#endif
#ifdef LINUX
	pthread_mutex_t mutex;
#endif
};

//auto lock/unlock helper class
class lockit
{
public:
	lockit(sync_obj* so);
	~lockit();

	void unlock();

private:
	DISALLOW_COPY_AND_ASSIGN(lockit);

private:
	sync_obj* _so;
};

}
