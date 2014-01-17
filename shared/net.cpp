#include "net.h"

#include "string.h" // for memcpy, strncpy

// for debugging
#include <iostream>
#include <sstream>
#include <string>



// global variables
bool net_local_only = false;

// ehost
ENetHost *gEhost = NULL;

int net_send_event(short evtype, const char *data, int size, ENetPeer *peer)
{
	if (net_local_only)
	{
		// TODO: add to queue
	}
	else
	{
		// create packet to send
		ENetPacket *packet = enet_packet_create(NULL, size+sizeof(short), ENET_PACKET_FLAG_RELIABLE);

		if (packet == NULL)
		{
			return -1;
		}


		memcpy(packet->data, &evtype, sizeof(short));
		if (size > 0) memcpy(packet->data + sizeof(short), data, size);

		// send packet
		if (enet_peer_send(peer, 0, packet) != 0)
		{
			return -2;
		}
	}
	

	return 0;
}

int net_broadcast_event(short evtype, const char *data, int size, ENetHost *host)
{
	if (net_local_only)
	{
		// TODO: add to queue
	}
	else
	{
		// create packet to send
		ENetPacket *packet = enet_packet_create(NULL, size+sizeof(short), ENET_PACKET_FLAG_RELIABLE);

		if (packet == NULL)
		{
			return -1;
		}


		memcpy(packet->data, &evtype, sizeof(short));
		if (size > 0) memcpy(packet->data + sizeof(short), data, size);

		// send packet
		enet_host_broadcast(host, 0, packet);
	}

	return 0;
}



// send functions used by both server and client
