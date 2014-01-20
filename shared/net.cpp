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

int net_send_event(uint16_t evtype, const char *data, uint32_t size, ENetPeer *peer)
{
	if (net_local_only)
	{
		// TODO: add to queue
	}
	else
	{
		// create packet to send
		ENetPacket *packet = enet_packet_create(NULL, size+sizeof(uint16_t), ENET_PACKET_FLAG_RELIABLE);

		if (packet == NULL)
		{
			return -1;
		}


		memcpy(packet->data, &evtype, sizeof(uint16_t));
		if (size > 0) memcpy(packet->data + sizeof(uint16_t), data, size);

		// send packet
		if (enet_peer_send(peer, 0, packet) != 0)
		{
			return -2;
		}
	}
	

	return 0;
}

int net_broadcast_event(uint16_t evtype, const char *data, uint32_t size, ENetHost *host)
{
	if (net_local_only)
	{
		// TODO: add to queue
	}
	else
	{
		// create packet to send
		ENetPacket *packet = enet_packet_create(NULL, size+sizeof(uint16_t), ENET_PACKET_FLAG_RELIABLE);

		if (packet == NULL)
		{
			return -1;
		}


		memcpy(packet->data, &evtype, sizeof(uint16_t));
		if (size > 0) memcpy(packet->data + sizeof(uint16_t), data, size);

		// send packet
		enet_host_broadcast(host, 0, packet);
	}

	return 0;
}



// send functions used by both server and client
int net_send_input_keys(uint actor_id, int input, ENetPeer *receiver)
{
	s_net_input_keys s;

    s.input = input;

    return net_send_event(NET_INPUT_KEYS, (const char *)&s, sizeof(s_net_input_keys), receiver);
}

int net_send_update_ang(uint actor_id, float ang, ENetPeer *receiver)
{
	s_net_update_ang s;

    s.ang = ang;

    return net_send_event(NET_UPDATE_ANG, (const char *)&s, sizeof(s_net_update_ang), receiver);
}