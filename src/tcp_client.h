
#pragma once

#include "platform.h"
#include "tools.h"
#include "thread.h"
#include "sync_obj.h"
#include "tcp_internal.h"

namespace luc {

//an easy use wrapper for TCP socket.
//the send() function is thread safe
//callback functions will be called in a receiver thread.
class tcp_client
{
public:
	tcp_client(funcOnReceive or, funcOnDisconnect od);
	~tcp_client();

	bool start(char* ip, int port, int timeout_milli=1000);
	void stop();

	//will fail if send buffer is full. if you want to simplify the things, just 
	//allocate a big-enough buffer, then the send() is not likely to fail.
	//FYI: there is no grantee successful send() in the real world. there always failure. unless the receiver told you, you never know that the send is really finished.
	//the successful result here just means in normal cases, the data will be send out.
	bool send(char* data, int len);

private:
	DISALLOW_COPY_AND_ASSIGN(tcp_client);
	static unsigned recv_thread(void* para);
	static unsigned send_thread(void* para);

	void internal_close();
	void continue_send();

private:
	funcOnReceive		on_recv;
	funcOnDisconnect	on_disconnect;

	thread_x* th_recv;
	thread_x* th_send;

	event_x	send_event;

	quick_buf* send_buf;
	quick_buf* recv_buf;

	sync_obj sync_send;

	SOCKET	sock;

	int	clientID;
};

}
