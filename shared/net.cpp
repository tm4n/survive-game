#include "net.h"

#include "string.h" // for memcpy, strncpy

// for debugging
#include <iostream>
#include <sstream>
#include <string>

bool enet_initialized;

net::net()
{
	eHost = NULL;
	local_only = false;
	is_server = false;
	
	if (!enet_initialized)
	{
			if (enet_initialize() != 0)
			{
				log(LOG_ERROR, "Could not initialize Enet library!");
				exit(1);
			}
			enet_initialized = true;
	}
}

net::net(std::list<ENetPacket*> *in_queue, std::mutex *mutex_in_queue, std::list<ENetPacket*> *out_queue, std::mutex *mutex_out_queue)
{
	eHost = NULL;
	local_only = true;
	is_server = false;
	
	this->in_queue = in_queue;
	this->mutex_in_queue = mutex_in_queue;
	this->out_queue = out_queue;
	this->mutex_out_queue = mutex_out_queue;
		
	// create local peer
	local_peer = new ENetPeer;
	memset(local_peer, 0, sizeof(ENetPeer));
	local_peer->state = ENET_PEER_STATE_CONNECTING;
}

net::~net()
{
	if (enet_initialized) enet_deinitialize();
}


ENetHost *net::host_create(const ENetAddress *address, size_t peerCount, size_t channelLimit, enet_uint32 incomingBandwidth, enet_uint32 outgoingBandwidth)
{
	if (address != NULL)
	{
		// a server will be created
		is_server = true;
	}
	
	if (local_only)
	{
		eHost = (ENetHost *) enet_malloc (sizeof (ENetHost));
		if (eHost == NULL)
		  return NULL;
		memset (eHost, 0, sizeof (ENetHost));
		
		eHost->connectedPeers = 1;
		if (address)
		{
			eHost->address.host = address->host;
			eHost->address.port = address->port;
		}
		eHost->channelLimit = channelLimit;
		eHost->incomingBandwidth = incomingBandwidth;
		eHost->outgoingBandwidth = outgoingBandwidth;
	}
	else
	{
		eHost = enet_host_create(address, peerCount, channelLimit, incomingBandwidth, outgoingBandwidth);
	}
	
	return eHost;
}

void net::host_destroy()
{
	if (local_only)
	{
		free(eHost);
	}
	else
	{
		enet_host_destroy(eHost);
	}
	eHost = NULL;
}

int net::host_service (ENetEvent *event, enet_uint32 timeout)
{
	if (local_only)
	{
		// connection established packet
		if (is_server == true && local_peer->state == ENET_PEER_STATE_CONNECTING)
		{
			local_peer->state = ENET_PEER_STATE_CONNECTED;
			
			event->channelID = 0;
			event->data = 0;
			event->packet = NULL;
			event->peer = local_peer;
			event->type = ENET_EVENT_TYPE_CONNECT;
			
			puts("Creating connection event for server!");
		
			return 1;
		}
		mutex_in_queue->lock();
		if (in_queue->size() > 0)
		{
			ENetPacket* packet = *in_queue->begin();
			in_queue->pop_front();
			
			event->channelID = 0;
			event->data = 0;
			event->packet = packet;
			event->peer = local_peer;
			event->type = ENET_EVENT_TYPE_RECEIVE;
			
			mutex_in_queue->unlock();
			return 1;
		}
		else
		{
			mutex_in_queue->unlock();
			return 0;
		}
	}
	else
	{
		return enet_host_service(eHost, event, timeout);
	}
}



int net::send_event(uint16_t evtype, const char *data, uint32_t size, ENetPeer *peer)
{
	// create packet to send
	ENetPacket *packet = enet_packet_create(NULL, size+sizeof(uint16_t), ENET_PACKET_FLAG_RELIABLE);

	if (packet == NULL)
	{
		return -1;
	}


	memcpy(packet->data, &evtype, sizeof(uint16_t));
	if (size > 0) memcpy(packet->data + sizeof(uint16_t), data, size);

	if (local_only)
	{
		// add to queue
		mutex_out_queue->lock();
		out_queue->push_back(packet);
		mutex_out_queue->unlock();
	}
	else
	{
		// send packet
		if (enet_peer_send(peer, 0, packet) != 0)
		{
			return -2;
		}
	}
	

	return 0;
}

int net::broadcast_event(uint16_t evtype, const char *data, uint32_t size)
{

	// create packet to send
	ENetPacket *packet = enet_packet_create(NULL, size+sizeof(uint16_t), ENET_PACKET_FLAG_RELIABLE);

	if (packet == NULL)
	{
		return -1;
	}


	memcpy(packet->data, &evtype, sizeof(uint16_t));
	if (size > 0) memcpy(packet->data + sizeof(uint16_t), data, size);

	if (local_only)
	{
		// add to queue
		mutex_out_queue->lock();
		out_queue->push_back(packet);
		mutex_out_queue->unlock();
	}
	else
	{
		// send packet
		enet_host_broadcast(eHost, 0, packet);
	}

	return 0;
}

int net::broadcast_event_except(uint16_t evtype, const char *data, uint32_t size, ENetPeer *expeer)
{

	// create packet to send
	ENetPacket *packet = enet_packet_create(NULL, size+sizeof(uint16_t), ENET_PACKET_FLAG_RELIABLE);

	if (packet == NULL)
	{
		return -1;
	}


	memcpy(packet->data, &evtype, sizeof(uint16_t));
	if (size > 0) memcpy(packet->data + sizeof(uint16_t), data, size);

	if (local_only)
	{
		// Don't add to queue, as we probably want to send this only to other players, not the one connected.
		// local_queue.push_back(packet);
	}
	else
	{
		for (uint i = 0; i < eHost->peerCount; i++)
		{
			ENetPeer *p = &eHost->peers[i];
			if (eHost->peers[i].state == ENET_PEER_STATE_CONNECTED && p != expeer)
			{
				// send packet, works thanks to reference counting
				if (enet_peer_send(p, 0, packet) != 0) puts("ERROR: could not send to peer");
			}
		}
	}

	return 0;
}


// send functions used by both server and client
int net::send_input_keys(uint actor_id, int input, ENetPeer *receiver)
{
	s_net_input_keys s;

	s.actor_id = actor_id;
    s.input = input;

    return send_event(NET_INPUT_KEYS, (const char *)&s, sizeof(s_net_input_keys), receiver);
}

int net::send_take(uint actor_id, int taken_id, ENetPeer *receiver)
{
	s_net_take s;
	
	s.actor_id = actor_id;
	s.taken_id = taken_id;
	
	puts("Sending take");
	
	return send_event(NET_TAKE, (const char *)&s, sizeof(s_net_take), receiver);
}

int net::send_update_ang(uint actor_id, float ang, float ang_interp_dir, ENetPeer *receiver)
{
	s_net_update_ang s;

	s.actor_id = actor_id;
    s.ang = ang;
    s.ang_interp_dir = ang_interp_dir;

    return send_event(NET_UPDATE_ANG, (const char *)&s, sizeof(s_net_update_ang), receiver);
}

int net::send_update_pos(uint actor_id, vec *pos, ENetPeer *receiver)
{
    s_net_update_pos s;

    s.actor_id = actor_id;
    s.pos.x = pos->x;
    s.pos.y = pos->y;
    s.pos.z = pos->z;

    return send_event(NET_UPDATE_POS, (const char*)&s, sizeof(s_net_update_pos), receiver);
}
