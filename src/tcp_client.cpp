
#include "tcp_client.h"

namespace luc {

//////////////////////////////////////////////////////////////////////////
// tcp_client

tcp_client::tcp_client(funcOnReceive or, funcOnDisconnect od)
{
#ifdef _WIN32
	WSADATA data;
	WSAStartup(MAKEWORD(2,2), &data);
#else
	//linux
	sigaction(SIG_PIPE, ignore);
#endif

	on_recv = or;
	on_disconnect = od;

	th_recv = NULL;
	th_send = NULL;

	recv_buf = new quick_buf();
	send_buf = new quick_buf();

	sock = INVALID_SOCKET;

	clientID = 0;
}

tcp_client::~tcp_client()
{
	this->stop();

	if (recv_buf)
	{
		delete recv_buf;
		recv_buf = NULL;
	}

	if (send_buf)
	{
		delete send_buf;
		send_buf = NULL;
	}
}

bool tcp_client::start(char* ip, int port, int timeout_milli/* =1000 */)
{
	this->stop();

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr(ip);
	addr.sin_port = htons(port);

	set_non_block(sock);

	struct timeval tv;
	tv.tv_sec = timeout_milli/1000;
	tv.tv_usec = (timeout_milli%1000)*1000;
	fd_set f_read, f_write;
	FD_ZERO(&f_read);
	FD_ZERO(&f_write);
	int nResult, nError;

	nResult = connect(sock, (const sockaddr*)&addr, sizeof(addr));
	if (nResult == SOCKET_ERROR)
	{
		char* lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError(), 0, (LPTSTR)&lpMsgBuf, 0, NULL);

		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			char info[1000];
			sprintf(info, "Connection failed:%d\n", WSAGetLastError());
			err(info);
			closesocket(sock);
			return false;
		}
		else // need select
		{
			FD_SET(sock, &f_read);
			f_write = f_read;
			nResult = select((int)sock+1, &f_read, &f_write, 0, &tv);
			if (nResult == 0)
			{
				err("Connect timeout\n");
				closesocket(sock);
				return false;
			}
			if (FD_ISSET(sock, &f_read) || FD_ISSET(sock, &f_write))
			{
				int nLen = sizeof(nError);
				if (getsockopt(sock, SOL_SOCKET, SO_ERROR, (char*)&nError, &nLen) < 0)
				{
					char info[1000];
					sprintf(info, "Connect error %d\n", nError);
					err(info);
					closesocket(sock);
					return false;
				}
			}
			else
			{
				err("Unknown err in connect\n");
				closesocket(sock);
				return false;
			}
		}
	}

	//socket connected, then start threads
	th_recv = thread_x::start_thread(&tcp_client::recv_thread, this);
	th_send = thread_x::start_thread(&tcp_client::send_thread, this);

	return true;
}

void tcp_client::stop()
{
	if (sock)
	{
		closesocket(sock);
		sock = INVALID_SOCKET;
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
}

bool tcp_client::send(char* data, int len)
{
	lockit lock(&sync_send);

	return false;
}

unsigned tcp_client::recv_thread(void* para)
{
	tcp_client* cln = (tcp_client*)para;

	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 10*1000;

	bool retry = false;

	while (true)
	{
		fd_set f_read;
		FD_ZERO(&f_read);
		FD_SET(cln->sock, &f_read);
		int ret = 0;
		
		if (retry)
		{
			ret = 1;
		}
		else
		{
			ret = select((int)cln->sock+1, &f_read, 0, 0, &tv);
		}
		
		if (ret == 0)
		{
			//time out
			continue;
		}
		else
		{
			int space = cln->recv_buf->cur_free_space();
			if (space > 1)
			{
				//last byte set to zero for safety
				int read = recv(cln->sock, cln->recv_buf->_buf, space - 1, 0);
				if (read <= 0)
				{
					int code = GetLastError();
					if (code != WSAEWOULDBLOCK)
					{
						//disconnected
						cln->on_disconnect(cln->clientID);

						cln->internal_close();
						break;
					}
				}
				else
				{
					cln->recv_buf->_len += read;
					cln->recv_buf->_buf[cln->recv_buf->_offset + cln->recv_buf->_len] = 0;
				}
			}

			//received data
			int proced = 0;
			PR_RESULT ret = cln->on_recv(cln->clientID, cln->recv_buf->_buf + cln->recv_buf->_offset, cln->recv_buf->_len, proced);
			switch (ret)
			{
			case PR_SUCCESS:
				cln->recv_buf->remove_data(proced);
				break;
			case PR_RETRY:
				cln->recv_buf->remove_data(proced);
				retry = true;
				Sleep(10);
				break;
			case PR_TO_DISCONNECT:
				cln->internal_close();
				break;
			case PR_FATAL_ERROR:
				{
					cln->internal_close();
					err("fatal error");
				}
				break;
			}
		}
	}

	return 0;
}

unsigned tcp_client::send_thread(void* para)
{
	tcp_client* cln = (tcp_client*)para;
	while (cln->sock != INVALID_SOCKET)
	{
		cln->send_event.wait_one(10);

		cln->continue_send();
	}

	return 0;
}

void tcp_client::internal_close()
{
	closesocket(sock);
	sock = INVALID_SOCKET;
}

void tcp_client::continue_send()
{
	lockit lock(&sync_send);

	if (send_buf->data_len() > 0)
	{
		int sent = ::send(sock, send_buf->_buf, send_buf->data_len(), 0);
		send_buf->remove_data(sent);
	}
}

}
