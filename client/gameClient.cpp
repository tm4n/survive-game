#include "gameClient.h"

#include "net_cl.h"
#include "helper.h"
#include "level_cl.h"
#include "player_cl.h"
#include "box_cl.h"


gameClient::gameClient(gameRenderer *arenderer)
{
	renderer = arenderer;
	local_state = 0;

	input = 0;
	input_enable = true;
	cam_bob_offset = 0.f;
}

gameClient::~gameClient()
{
	if (net_client != NULL) delete net_client;
}



void gameClient::handle_netevent(ENetEvent *event)
{
	switch (event->type)
    {
		case ENET_EVENT_TYPE_CONNECT:
		{
			// not called on local network
			log(LOG_DEBUG, "Connection successfull, waiting for server version.");

			break;
		}

		case ENET_EVENT_TYPE_DISCONNECT:
		{
			// not called on local network
			
			event->peer->data = NULL;

			log(LOG_DEBUG, "disconnect event.");

			// hide interface
			// unload level

			// display main menu
	    
			break;
		}

		case ENET_EVENT_TYPE_RECEIVE:
		{
			/*printf ("A packet of length %lu, event %d, was received on channel %u.\n",
					event->packet -> dataLength,
					*((short*) event->packet->data),
					event->channelID);*/


			// check minimal packet size
			if (event->packet->dataLength >= 2)
			{

				// extract packet data
				short net_type = *((short*) event->packet->data);
				char *data = (char*)(event->packet -> data) +sizeof(short);

				////////////////////////////
				// differentiate net type
				//

				switch(net_type)
				{
					case NET_VERSION:
					{
						s_net_version *d = (s_net_version*)data;

						printf("Connected to Server, version %i\n", d->version);

						if (d->version != DEF_VERSION)
						{
							log(LOG_ERROR, "Tried to connect to server with different version number.");
						}
						else
						{
							// send my name
							net_client->send_sync_client("Player1", event->peer);
						}

						break;
					}

					case NET_SYNC_SERVER:
					{
						s_net_sync_server *d = (s_net_sync_server*)data;

						// load level
						lvl_cl = new level_cl((char*)d->mapfile, renderer);
						lvl = lvl_cl;

						break;
					}

					case NET_SYNC_PLAYER:
					{
						s_net_sync_player *d = (s_net_sync_player*)data;

						if (lvl != NULL)
						{
							// create a player at given position
							new player_cl(lvl, d->actor_id, &d->pos, &d->ang, d->health, (char*)d->name, d->weapon, d->input, d->object_taken, renderer);
						}
						else log(LOG_ERROR, "Receiver NET_SYNC_PLAYER without level");

						break;
					}

					case NET_SYNC_BOX:
					{
						s_net_sync_box *d = (s_net_sync_box*)data;

						if (lvl != NULL)
						{
							// create a box at given position
							new box_cl(lvl, d->actor_id, d->box_type, &d->pos, d->health, renderer);
						}
						else log(LOG_ERROR, "Received NET_SYNC_BOX without level");

						break;
					}

					case NET_SYNC_FINISH:
					{
						// mark client as joinable
						local_state = 1;

						break;
					}

					case NET_REMOVE_ACTOR:
					{
						s_net_remove_actor *d = (s_net_remove_actor*)data;

						if (lvl != NULL)
						{
							// get actor at position
							actor *ac = lvl->actorlist.at(d->actor_id);

							if (ac != NULL)
							{
								delete ac;
							}
							else log (LOG_ERROR, "Received NET_REMOVE_ACTOR with invalid actor_id");
						}
						else log (LOG_ERROR, "Received NET_REMOVE_ACTOR without loaded level");

						break;
					}

					case NET_JOIN:
					{
						s_net_join *d = (s_net_join *)data;

						if (local_state == 1)
						{
							// switch to gaming mode
							own_actor_id = d->own_actor_id;
							local_state = 2;

						}
						else log (LOG_ERROR, "Received NET_JOIN while not spectating");

						break;
					}
					
					case NET_TAKE:
					{
						s_net_take *d = (s_net_take*)data;
						
						player_cl *pl = lvl_cl->get_player(d->actor_id);
						
						if (pl != NULL)
						{
							if (d->taken_id < 0)
							{
								box_cl *box = lvl_cl->get_box((uint)pl->object_taken);
								if (box != NULL)
								{
									pl->object_taken = -1;
									box->taker_id = -1;
									box->state = BOX_STATE_DEFAULT;
								}
								else
								{
									pl->object_taken = -1;
									log(LOG_ERROR, "Received NET_TAKE, and player had invalid object in hand");
								}
							}
							else
							{
								box_cl *box = lvl_cl->get_box((uint)d->taken_id);
								if (box != NULL)
								{
									pl->object_taken = d->taken_id;
									box->taker_id = d->actor_id;
									box->state = BOX_STATE_TAKEN;
								}
								else log(LOG_ERROR, "Received NET_TAKE with invalid box");
							}
						}
						else log(LOG_ERROR, "Received NET_TAKE invalid player");
						
						break;
					}
					
					case NET_UPDATE_ANG:
					{
						// get send data
						s_net_update_ang *d = (s_net_update_ang *)data;
						
						player_cl *pl= lvl_cl->get_player(d->actor_id);
						if (pl != NULL)
						{
							printf("setting angle for %i \n", d->actor_id);
							pl->angle.x = d->ang;
							pl->ang_interp_dir = d->ang_interp_dir;
						}
						else log(LOG_ERROR, "Received NET_UPDATE_ANG for non-player actor");
						
						break;
					}

					default:
					{
						log(LOG_WARNING, "Packet with invalid net_type received");
					}
				}

			}
			/* Clean up the packet now that we're done using it. */
			enet_packet_destroy (event->packet);

			break;
		}
	}

}

bool gameClient::connect(std::list<ENetPacket*> *in_queue, std::mutex *mutex_in_queue, std::list<ENetPacket*> *out_queue, std::mutex *mutex_out_queue)
{
	// connect to local server
	
	////////////////////////////////////////////////////
	// Initialize enet host, no connections is done yet
    // Create enet client
    net_client = new net_cl(in_queue, mutex_in_queue, out_queue, mutex_out_queue);

    net_client->host_create (NULL /* create a client host */,
                1 /* only allow 1 outgoing connection */,
                2 /* allow up 2 channels to be used, 0 and 1 */,
                0 /* no downstream limit */,
                0 /* no upstream limit */);

    if (net_client->eHost == NULL)
    {
        puts("An error occurred while trying to create an local client host.\n");
        exit (EXIT_FAILURE);
    }

	///////////////////////////////////////////////////
	/* Connect to a server */
	state = 0;

	log(LOG_DEBUG, "Connecting to local server...");

	/* Initiate the connection, allocating one channel. */
	net_client->host_connect (NULL, 1, 0);

	return(true);
}


bool gameClient::connect(const char *ip, int port)
{
	// connect to networked server
	
	////////////////////////////////////////////////////
	// Initialize enet host, no connections is done yet
    // Create enet client
    net_client = new net_cl();

    net_client->host_create (NULL /* create a client host */,
                1 /* only allow 1 outgoing connection */,
                2 /* allow up 2 channels to be used, 0 and 1 */,
                0 /* no downstream limit */,
                0 /* no upstream limit */);

    if (net_client->eHost == NULL)
    {
        puts("An error occurred while trying to create an ENet client host.\n");
        exit (EXIT_FAILURE);
    }
	
	//v(mouse_mode) = 4;

	ENetAddress address;

	///////////////////////////////////////////////////
	/* Connect to a server */
	state = 0;

	enet_address_set_host (&address, ip);
	address.port = port;

	log(LOG_DEBUG, "Connecting to server...");

	/* Initiate the connection, allocating one channel. */
	net_client->host_connect (&address, 1, 0);

	if (net_client->serverpeer == NULL)
	{
	   log(LOG_ERROR, "No available peers for initiating an ENet connection.");
	   return(false);
	   //exit (EXIT_FAILURE);
	}
	return(true);
}


void gameClient::disconnect()
{
	if (net_client->local_only == false)
	{
		// disconnect from server after all packages been send
		enet_peer_disconnect_later(net_client->serverpeer, 0);

		// wait for event
	}
}


void gameClient::frame(double time_delta)
{
	// Handle packages
	ENetEvent event;
	while (net_client->host_service (&event, 0) > 0)
			handle_netevent(& event);

	// move spectator camera
	if (local_state == 1)
	{
		int key_vely = (input & INPUT_BACK ? 1 : 0) - (input & INPUT_FORW ? 1 : 0);
		int key_velx = (input & INPUT_RIGHT ? 1 : 0) - (input & INPUT_LEFT ? 1 : 0);

		renderer->CameraPos.x -= (float) (cos(toRadians(renderer->CameraAngle.x))*cos(toRadians(renderer->CameraAngle.y))) * key_vely * 20.f;
		renderer->CameraPos.y -= (float) (sin(toRadians(renderer->CameraAngle.x))*cos(toRadians(renderer->CameraAngle.y))) * key_vely * 20.f;
		renderer->CameraPos.z -= (float) (sin(toRadians(renderer->CameraAngle.y))) * key_vely * 20.f;

		renderer->CameraPos.x += (float) (cos(toRadians(renderer->CameraAngle.x-90.f))) * key_velx * 15.f;
		renderer->CameraPos.y += (float) (sin(toRadians(renderer->CameraAngle.x-90.f))) * key_velx * 15.f;
	}
	if (local_state == 2)
	{
		player_cl *pl = get_own_player();
		if (pl == NULL) {log(LOG_ERROR, "Could not retreive own player!"); return;}

		// stick camera to player
		pl->ro->visible = false;

		renderer->CameraPos.x = pl->position.x;
		renderer->CameraPos.y = pl->position.y;
		renderer->CameraPos.z = pl->position.z + pl->bb_max.z - CAMERA_VIEW_HEIGHT + cam_bob_offset;

		// only when player is alive
		if (pl->health > 0.f)
		{
			// give input to player
			if (input != pl->input)
			{
				// update for this player and all others
				pl->input = input;
				net_client->send_input_keys(pl->id, input, net_client->serverpeer);
			}

		}
	}


	if (lvl != NULL)
	{
		// callback to entity actions!
		for (uint i = 0; i < lvl->actorlist.size; i++)
		{
			if (lvl->actorlist.elem[i] != 0) lvl->actorlist.elem[i]->frame(time_delta);
		}
	}

}

void gameClient::event_mouse(SDL_Event *evt)
{
	if (evt->type == SDL_MOUSEMOTION)
	{
		renderer->CameraAngle.x -= evt->motion.xrel*0.05f;
		renderer->CameraAngle.y -= evt->motion.yrel*0.05f;
	}
	if (evt->type == SDL_MOUSEBUTTONDOWN)
	{
		if (evt->button.button == SDL_BUTTON_LEFT)
		{
			if (local_state == 1)
			{
				// send request to join
				net_client->send_request_join(net_client->serverpeer);
			}
			if (local_state == 2)
			{
				// TODO: shoot
			}
		}
		if (evt->button.button == SDL_BUTTON_RIGHT)
		{
			if (local_state == 2)
			{
				// take
				net_client->send_take(own_actor_id, 1, net_client->serverpeer);
			}
		}
	}
	if (evt->type == SDL_KEYDOWN)
	{
		switch( evt->key.keysym.sym )
		{
		case SDLK_a:
        case SDLK_LEFT:
            input |= INPUT_LEFT;
            break;
		case SDLK_d:
        case SDLK_RIGHT:
            input |= INPUT_RIGHT;
            break;
		case SDLK_w:
        case SDLK_UP:
            input |= INPUT_FORW;
            break;
		case SDLK_s:
        case SDLK_DOWN:
            input |= INPUT_BACK;
            break;

		case SDLK_RSHIFT:
		case SDLK_LSHIFT:
			input |= INPUT_SPRINT;
			break;
		case SDLK_SPACE:
			input |= INPUT_JUMP;
			break;
		}
	}
	if (evt->type == SDL_KEYUP)
	{
		switch( evt->key.keysym.sym )
		{
			case SDLK_a:
			case SDLK_LEFT:
				input &= ~INPUT_LEFT;
				break;
			case SDLK_d:
			case SDLK_RIGHT:
				input &= ~INPUT_RIGHT;
				break;
			case SDLK_w:
			case SDLK_UP:
				input &= ~INPUT_FORW;
				break;
			case SDLK_s:
			case SDLK_DOWN:
				input &= ~INPUT_BACK;
				break;

			case SDLK_RSHIFT:
			case SDLK_LSHIFT:
				input &= ~INPUT_SPRINT;
				break;
			case SDLK_SPACE:
				input &= ~INPUT_JUMP;
			break;
		}
	}
}


player_cl *gameClient::get_own_player()
{
	if (local_state != 2) return NULL;
	actor *ac = lvl->actorlist.at(own_actor_id);
	if (ac != NULL) return (player_cl*)ac;
	return NULL;
}
