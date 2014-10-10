
#pragma once

#include "platform.h"
#include "tools.h"
#include "thread.h"
#include "sync_obj.h"
#include "map.h"
#include "tcp_internal.h"

namespace luc {

class tcp_server
{
	class sock_op
	{
	public:
		int			clientID;
		quick_buf	send_buf;
		quick_buf	recv_buf;
		sync_obj	sync_send;
		SOCKET		sock;
	};

public:
	tcp_server(funcOnAccept oa, funcOnReceive or, funcOnDisconnect od);
	~tcp_server();

	bool start(int port);
	void stop();

	//this function just close the socket. the callback function will be called by recv_thread.
	void kickout(int clientID);

	//
	bool send(int clientID, char* data, int len);

private:
	DISALLOW_COPY_AND_ASSIGN(tcp_server);
	static unsigned accept_thread(void* para);
	static unsigned recv_thread(void* para);
	static unsigned send_thread(void* para);

	//
	sock_op* create_cln(int clientID);
	//
	sock_op* find_cln(int clientID);

private:
	funcOnAccept		on_accept;
	funcOnReceive		on_recv;
	funcOnDisconnect	on_disconnect;

	thread_x* th_accept;
	thread_x* th_recv;
	thread_x* th_send;

	event_x	send_event;

	SOCKET	lsock;		//socket for listen

	int		next_client_id;

	sync_obj	sync_clients;	//'clients' need to be access in different threads
	map_ii		clients;
};

}
