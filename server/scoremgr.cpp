#include "scoremgr.h"
#include "net_sv.h"

#include <sstream>

void scoremgr::add_points(uint actor_id, int num_points)
{
	// iterator through players
	int ct = 0;
	for (uint i = 0; i < net_server->eHost->peerCount; i++)
	{
		ENetPeer *p = &net_server->eHost->peers[i];
		if (p->state == ENET_PEER_STATE_CONNECTED)
		{
			s_peer_data *pd = (s_peer_data *)p->data;
			if (pd != NULL)
			{
				if (pd->player_actor_id == actor_id)
				{
					pd->score += num_points;
					return;
				}
			}
		}
	}
}

void scoremgr::add_points(ENetPeer *peer, int num_points)
{
	s_peer_data *pd = (s_peer_data *)peer->data;
	if (pd != NULL)
	{
		pd->score += num_points;
	}
}

void scoremgr::remove_points_death(uint actor_id)
{
	// iterator through players
	int ct = 0;
	for (uint i = 0; i < net_server->eHost->peerCount; i++)
	{
		ENetPeer *p = &net_server->eHost->peers[i];
		if (p->state == ENET_PEER_STATE_CONNECTED)
		{
			s_peer_data *pd = (s_peer_data *)p->data;
			if (pd != NULL)
			{
				if (pd->player_actor_id == actor_id)
				{
					pd->score = (uint)((double)pd->score * 2./3.);
					return;
				}
			}
		}
	}
}

void scoremgr::clear_all()
{
	// iterator through players
	int ct = 0;
	for (uint i = 0; i < net_server->eHost->peerCount; i++)
	{
		ENetPeer *p = &net_server->eHost->peers[i];
		if (p->state == ENET_PEER_STATE_CONNECTED)
		{
			s_peer_data *pd = (s_peer_data *)p->data;
			if (pd != NULL)
			{
				pd->score = 0;
			}
		}
	}
}

void scoremgr::update_points(uint actor_id)
{
	// iterator through players
	int ct = 0;
	for (uint i = 0; i < net_server->eHost->peerCount; i++)
	{
		ENetPeer *p = &net_server->eHost->peers[i];
		if (p->state == ENET_PEER_STATE_CONNECTED)
		{
			s_peer_data *pd = (s_peer_data *)p->data;
			if (pd != NULL)
			{
				net_server->send_update_score(pd->score, p);
			}
		}
	}
}

void scoremgr::update_points(ENetPeer *p)
{
	s_peer_data *pd = (s_peer_data *)p->data;
	if (pd != NULL)
	{
		net_server->send_update_score(pd->score, p);
	}

}

void scoremgr::get_full_string(std::string *str)
{
	std::ostringstream ss;

	int ct = 0;
	for (uint i = 0; i < net_server->eHost->peerCount; i++)
	{
		ENetPeer *p = &net_server->eHost->peers[i];
		if (p->state == ENET_PEER_STATE_CONNECTED)
		{
			s_peer_data *pd = (s_peer_data *)p->data;
			if (pd != NULL)
			{
				ss << pd->player_name << "§" << pd->score << "§" << p->roundTripTime << "§";
			}
		}
	}
	ss << "Record goes here!";

	str->assign(ss.str());
}