
#include "sync_obj.h"

namespace luc {

//////////////////////////////////////////////////////////////////////////
// sync_obj

sync_obj::sync_obj()
{
#ifdef _WIN32
	InitializeCriticalSection(&sec);
#endif
#ifdef LINUX
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init(&mutex, &attr);
	pthread_mutexattr_destroy(&attr);
#endif
}

sync_obj::~sync_obj()
{
#ifdef _WIN32
	DeleteCriticalSection(&sec);
#endif
#ifdef LINUX
	pthread_mutex_destroy(&mutex);
#endif
}

void sync_obj::lock()
{
#ifdef _WIN32
	EnterCriticalSection(&sec);
#endif
#ifdef LINUX
	pthread_mutex_lock(&mutex);
#endif
}

void sync_obj::unlock()
{
#ifdef _WIN32
	LeaveCriticalSection(&sec);
#endif
#ifdef LINUX
	pthread_mutex_unlock(&mutex);
#endif
}

bool sync_obj::try_lock()
{
#ifdef _WIN32
	bool ret = (TryEnterCriticalSection(&sec) == TRUE);
	return ret;
#endif
#ifdef LINUX
	bool ret = pthread_mutex_try_lock(&mutex);
	return ret;
#endif
}

//////////////////////////////////////////////////////////////////////////
// lockit

lockit::lockit(sync_obj* so)
{
	_so = so;
}

lockit::~lockit()
{
	unlock();
}

void lockit::unlock()
{
	if (_so)
	{
		_so->unlock();
		_so = NULL;
	}
}

}
