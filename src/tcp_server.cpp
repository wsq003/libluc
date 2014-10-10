
#include "tcp_server.h"

namespace luc {

//////////////////////////////////////////////////////////////////////////
// tcp_server

tcp_server::tcp_server(funcOnAccept oa, funcOnReceive or, funcOnDisconnect od)
{
#ifdef _WIN32
	WSADATA data;
	WSAStartup(MAKEWORD(2,2), &data);
#else
	//linux
	sigaction(SIG_PIPE, ignore);
#endif

	on_accept = oa;
	on_recv = or;
	on_disconnect = od;

	th_accept = NULL;
	th_recv = NULL;
	th_send = NULL;

	lsock = INVALID_SOCKET;

	next_client_id = 1;
}

tcp_server::~tcp_server()
{
	this->stop();
}

bool tcp_server::start(int port)
{
	//socket listen ok, then start threads
	th_accept = thread_x::start_thread(&tcp_server::accept_thread, this);
	th_recv = thread_x::start_thread(&tcp_server::recv_thread, this);
	th_send = thread_x::start_thread(&tcp_server::send_thread, this);

	return false;
}

void tcp_server::stop()
{
	if (lsock != INVALID_SOCKET)
	{
		lockit lock(&sync_clients);

		clients.get_start_pos();
		int clientID;
		sock_op* op;
		while (clients.get_next_assoc(clientID, (int&)op))
		{
			this->kickout(clientID);
		}

		lock.unlock();	//unlock to prevent deadlock

		if (th_accept)
		{
			th_accept->stop();
			delete th_accept;
			th_accept = NULL;
		}

		if (th_recv)
		{
			th_recv->stop();
			delete th_recv;
			th_recv = NULL;
		}

		if (th_send)
		{
			th_send->stop();
			delete th_send;
			th_send = NULL;
		}

		closesocket(lsock);
		lsock = INVALID_SOCKET;
	}
}

void tcp_server::kickout(int clientID)
{
	sock_op* op = find_cln(clientID);
	if (op != NULL)
	{
		closesocket(op->sock);
	}
}

tcp_server::sock_op* tcp_server::create_cln(int clientID)
{
	lockit lock(&sync_clients);

	tcp_server::sock_op* op = new tcp_server::sock_op();
	op->clientID = clientID;
	
	clients.set(clientID, (int)op);

	return op;
}

tcp_server::sock_op* tcp_server::find_cln(int clientID)
{
	lockit lock(&sync_clients);

	tcp_server::sock_op* op = NULL;
	if (clients.find(clientID, (int&)op))
	{
		return op;
	}
	else
	{
		return NULL;
	}
}

unsigned tcp_server::accept_thread(void* para)
{
	tcp_server* svr = (tcp_server*)para;

	while (svr->lsock != INVALID_SOCKET)
	{
		SOCKET s = accept(svr->lsock, NULL, NULL);
		
		tcp_server::sock_op* op = svr->create_cln(svr->next_client_id);
		op->sock = s;

		svr->next_client_id++;
	}

	return 0;
}

unsigned tcp_server::recv_thread(void* para)
{
	tcp_server* svr = (tcp_server*)para;

	return 0;
}

unsigned tcp_server::send_thread(void* para)
{
	tcp_server* svr = (tcp_server*)para;

	return 0;
}

}
