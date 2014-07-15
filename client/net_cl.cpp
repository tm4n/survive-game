#include "net_cl.h"

#include <sstream>
#include <cstring>

net_cl *net_client = NULL;

net_cl::net_cl()
 : net()
{
	serverpeer = NULL;
}

net_cl::net_cl(std::list<ENetPacket*> *in_queue, std::mutex *mutex_in_queue, std::list<ENetPacket*> *out_queue, std::mutex *mutex_out_queue)
   : net(in_queue, mutex_in_queue, out_queue, mutex_out_queue)
{
	serverpeer = NULL;
}


void net_cl::host_connect (const ENetAddress *address, size_t channelCount, enet_uint32 data)
{
	if (local_only)
	{
		// TODO:
	}
	else
	{
		serverpeer = enet_host_connect(eHost, address, channelCount, data);
	}
}


// special send functions for client
int net_cl::send_sync_client(const char *name, ENetPeer *receiver)
{
	return send_event(NET_SYNC_CLIENT, name, strlen(name)+1, receiver);
}

int net_cl::send_request_join(ENetPeer *receiver)
{
	return send_event(NET_REQUEST_JOIN, NULL, 0, receiver);
}


int net_cl::send_chat(const char* msg, uint len, ENetPeer *receiver)
{
	return send_event(NET_CHAT, msg, len, receiver);
}


int net_cl::send_reload(uint actor_id, ENetPeer *receiver)
{
	s_net_reload s;

	s.actor_id = actor_id;

	return send_event(NET_RELOAD, (const char*)&s, sizeof(s_net_reload), receiver);
}