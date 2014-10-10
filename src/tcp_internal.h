
#pragma once

#include "platform.h"

namespace luc {

enum PR_RESULT
{
	PR_SUCCESS = 0,
	PR_RETRY,
	PR_TO_DISCONNECT,
	PR_FATAL_ERROR
};

typedef	void (*funcOnAccept)(int clientID);
typedef	PR_RESULT (*funcOnReceive)(int clientID, char* data, int len, int& proced);
typedef	void (*funcOnDisconnect)(int clientID);

//this buffer is not thread safe
class quick_buf
{
	friend class tcp_client;
	friend class tcp_server;

public:
	quick_buf(int size = 8192);
	~quick_buf();

	bool put_data(char* data, int len);
	void peek_data(char*& data, int& len);
	bool remove_data(int len);

	int cur_free_space();
	int data_len();

private:
	DISALLOW_COPY_AND_ASSIGN(quick_buf);
	void try_truncate();

protected:
	char*	_buf;
	int		_size;
	int		_offset;		//start of the valid data
	int		_len;		//length of the valid data
};


}
