#include "net_cl.h"

#include <sstream>



// special send functions for client

int net_send_chat(const char* msg, uint len, ENetPeer *receiver)
{
	return net_send_event(NET_CHAT, msg, len, receiver);
}
