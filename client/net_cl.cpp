#include "net_cl.h"

#include <sstream>



// special send functions for client
int net_send_sync_client(const char *name, ENetPeer *receiver)
{
	return net_send_event(NET_SYNC_CLIENT, name, strlen(name), receiver);
}

int net_send_request_join(ENetPeer *receiver)
{
	return net_send_event(NET_REQUEST_JOIN, NULL, 0, receiver);
}


int net_send_chat(const char* msg, uint len, ENetPeer *receiver)
{
	return net_send_event(NET_CHAT, msg, len, receiver);
}
