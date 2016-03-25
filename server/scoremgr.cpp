#include "scoremgr.h"
#include "net_sv.h"
#include "helper.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <exception>
#ifdef _WIN32
#include "direct.h"
#endif

uint scoremgr::highscore_points = 0;
std::string scoremgr::scoreholder = {};

void scoremgr::add_points(uint actor_id, int num_points)
{
	// iterator through players
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
	if (highscore_points == 0) ss << "No highscore archieved"
	else ss << "Highscore: " << highscore_points << " points by " << scoreholder;

	str->assign(ss.str());
}


bool scoremgr::determine_highscore()
{
	bool new_highscore = false;

	// iterator through players
	for (uint i = 0; i < net_server->eHost->peerCount; i++)
	{
		ENetPeer *p = &net_server->eHost->peers[i];
		if (p->state == ENET_PEER_STATE_CONNECTED)
		{
			s_peer_data *pd = (s_peer_data *)p->data;
			if (pd != NULL)
			{
				if (pd->score > highscore_points)
				{
					new_highscore = true;
					highscore_points = pd->score;
					scoreholder.assign(pd->player_name);
				}
			}
		}
	}

	return new_highscore;
}

void scoremgr::load_highscore()
{
	std::string path = get_settings_file_path();
	if (path.size() > 0)
	{
		#ifdef _WIN32
		path.append("\\highscore.cfg");
		#else
		path.append("/highscore.cfg");
		#endif
		
		std::cout << "opening file " << path << std::endl;

		std::ifstream sfile;
		sfile.open(path);
		
		if (sfile.is_open())
		{
			std::cout << "\nLoading Highscore: " << path << "\n";

			std::string l;
			std::vector<std::string> lines;
			while (std::getline(sfile, l))
			{
				lines.push_back(l);
			}
			sfile.close();

			if (lines.size() == 2)
			{
				try
				{
					highscore_points = std::stoi(lines.at(0));
					scoreholder.assign(lines.at(1));
				}
				catch (std::exception& e)
				{
					log(LOG_ERROR, "Failed to parse highscore file.");
					log(LOG_ERROR, e.what());
				}

				return;
			}
			else std::cout << "\n Highscore file had invalid number of lines \n";
		}
	}
}

void scoremgr::save_highscore()
{
	std::string path = get_settings_file_path();
	if (path.length() > 0)
	{
		// Create directory if needed
		struct stat st;
		if (stat(path.c_str(), &st) != 0)
		{
			#ifdef _WIN32
			int mkdirResult = _mkdir(path.c_str());
			#else
			int mkdirResult = mkdir(path.c_str(), 0777);
			#endif
			if (mkdirResult != 0)
			{
				log(LOG_ERROR, "Directory creation failed");
				return;
			}
		}
		
		#ifdef _WIN32
		path.append("\\highscore.cfg");
		#else
		path.append("/highscore.cfg");
		#endif

		std::ofstream sfile;
		sfile.open(path, std::fstream::out | std::fstream::trunc);

		if (!sfile.fail())
		{
			std::cout << "\nSaving highscore: " << path << "\n";

			sfile << highscore_points << '\n';
			sfile << scoreholder;

			sfile.close();
			return;
		}
	}
	log(LOG_ERROR, "Error saving highscore file");
}
