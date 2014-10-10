
#pragma once


#ifdef _WIN32

#ifndef _WIN32_WINNT
#define _WIN32_WINNT	0x0500
#endif
#define _HAS_ITERATOR_DEBUGGING 0
//#define _SECURE_SCL 0

#include <process.h>
#include <Windows.h>
#include <time.h>
#include <map>
#include <vector>
#include <string>
#include <deque>
#include <list>

using namespace std;

#pragma warning(disable: 4267)
#pragma warning(disable: 4996)
#pragma warning(disable: 4311)

#define int64	__int64

#else	//Linux

#include <pthread.h>
#define int64 long long

#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&); \
	void operator=(const TypeName&)


