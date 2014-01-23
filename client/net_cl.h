#ifndef __NET_CL_H__
#define __NET_CL_H__

#include "net.h"

class net_cl : public net
{
public:
	net_cl();
	net_cl(std::list<ENetPacket*> *in_queue, std::mutex *mutex_in_queue, std::list<ENetPacket*> *out_queue, std::mutex *mutex_out_queue);

	void host_connect (const ENetAddress *address, size_t channelCount, enet_uint32 data);

	int send_sync_client(const char *name, ENetPeer *receiver);
	int send_request_join(ENetPeer *receiver);

	int send_chat(const char *msg, uint len, ENetPeer *receiver);
	
	ENetPeer *serverpeer;
};

extern net_cl *net_client;

#endif
