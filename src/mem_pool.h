
#pragma once

#include "sync_obj.h"

namespace luc {

#define DECLARE_MEM_POOL(class_name) \
	private:	\
		static sync_obj	lock_mem_pool;	\
		static list<class_name*> free_chain;	\
	public: \
		static void* operator new(size_t _size);	\
		static void operator delete(void* _ptr);

#define IMPL_MEM_POOL(class_name)


}
