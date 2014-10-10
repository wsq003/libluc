// test.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"

#include "../include/libluc.h"

using namespace luc;

void OnAccept(int clientID)
{
	write_normal_log("accepted");
}

PR_RESULT OnReceive(int clientID, char* data, int len, int& proced)
{
	proced = len;
	return PR_SUCCESS;
}

void OnDisconnect(int clientID)
{
	write_normal_log("disconnected");
}

void test1()
{
	tcp_client cln(OnReceive, OnDisconnect);
	if (cln.start("127.0.0.1", 9995))
	{
		write_normal_log("connected");
	}
	else
	{
		write_normal_log("fail to connect");
	}

	printf("press enter to stop test1\n");
	getchar();

	cln.stop();
}

void test2()
{
	tcp_server svr(OnAccept, OnReceive, OnDisconnect);
	svr.start(8001);

	printf("press enter to stop test2\n");
	getchar();

	svr.stop();
}

void test3()
{
	//beware of:
	//#define _HAS_ITERATOR_DEBUGGING 0

	map<int, int> mm;
	mm.find(1);
}

void test4()
{
	char buf[100] = "\11\12 a ";
	char sub[20][100];
	int count = easy_split(buf, sub[0], 20, 100, ' ');
	assert(count == 2);
	assert(strcmp(sub[0], "\11\12") == 0);
	assert(strcmp(sub[1], "a") == 0);

	count = easy_split(buf, sub[0], 20, 100, 'x');
	assert(count == 1);
	assert(strcmp(sub[0], buf) == 0);

	trim(buf);
	assert(strcmp(buf, "a") == 0);

}

int _tmain(int argc, _TCHAR* argv[])
{
	write_normal_log("");
	write_normal_log("app start");

	sync_obj so;

	lockit lock(&so);

	double begin = get_milli_seconds();
	Sleep(43);
	double end = get_milli_seconds();

	printf("used %.2f milli\n", end - begin);

	char* msg = "hello\n";
	quick_append_file(msg, strlen(msg), "test.log");

	map_ii mm;
	mm.set(1, 100);
	int val = 0;
	mm.find(1, val);
	assert(val == 100);

	test1();

	test2();

	test3();

	test4();

	write_normal_log("app quit");

	return 0;
}

