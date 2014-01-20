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


	////////////////////////////////////////////////////
	// Initialize enet host, no connections is done yet
    // Create enet client
	serverpeer = NULL;

    gEhost = enet_host_create (NULL /* create a client host */,
                1 /* only allow 1 outgoing connection */,
                2 /* allow up 2 channels to be used, 0 and 1 */,
                0 /* no downstream limit */,
                0 /* no upstream limit */);

    if (gEhost == NULL)
    {
        puts("An error occurred while trying to create an ENet client host.\n");
        exit (EXIT_FAILURE);
    }
}



void gameClient::handle_netevent(ENetEvent *event)
{
	switch (event->type)
    {
		case ENET_EVENT_TYPE_CONNECT:
		{
			log(LOG_DEBUG, "Connection successfull, waiting for server version.");

			break;
		}

		case ENET_EVENT_TYPE_DISCONNECT:
		{
			event->peer->data = NULL;

			log(LOG_DEBUG, "disconnect event.");

			// hide interface
			// unload level

			// display main menu
	    
			break;
		}

		case ENET_EVENT_TYPE_RECEIVE:
		{
			printf ("A packet of length %lu, event %d, was received from %s on channel %u.\n",
					event->packet -> dataLength,
					*((short*) event->packet->data),
					(char*)(event->peer -> data),
					event->channelID);


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
							net_send_sync_client("Player1", event->peer);
						}

						break;
					}

					case NET_SYNC_SERVER:
					{
						s_net_sync_server *d = (s_net_sync_server*)data;

						// load level
						lvl = new level_cl((char*)d->mapfile, renderer);

						break;
					}

					case NET_SYNC_PLAYER:
					{
						s_net_sync_player *d = (s_net_sync_player*)data;

						if (lvl != NULL)
						{
							// create a box at given position
							new player_cl(lvl, d->actor_id, &d->pos, &d->ang, d->health, (char*)d->name, d->state, d->input, renderer);
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


bool gameClient::connect(const char *ip, int port)
{
	//v(mouse_mode) = 4;

	ENetAddress address;

	///////////////////////////////////////////////////
    /* Connect to a server */
	state = 0;

    enet_address_set_host (&address, ip);
    address.port = port;

	log(LOG_DEBUG, "Connecting to server...");

    /* Initiate the connection, allocating one channel. */
    serverpeer = enet_host_connect (gEhost, &address, 1, 0);

    if (serverpeer == NULL)
    {
       log(LOG_ERROR, "No available peers for initiating an ENet connection.");
	   return(false);
       //exit (EXIT_FAILURE);
    }
	return(true);
}


void gameClient::disconnect()
{
	// disconnect from server after all packages been send
	enet_peer_disconnect_later(serverpeer, 0);

	// wait for event
}


void gameClient::frame(double time_delta)
{
	// Handle packages
	ENetEvent event;
	while (enet_host_service (gEhost, &event, 0) > 0)
			handle_netevent(& event);

	// move spectator camera
	if (local_state == 1)
	{
		int key_vely = (input & INPUT_BACK ? 1 : 0) - (input & INPUT_FORW ? 1 : 0);
		int key_velx = (input & INPUT_RIGHT ? 1 : 0) - (input & INPUT_LEFT ? 1 : 0);

		renderer->CameraPos.x -= (float) (cos(toRadians(renderer->CameraAngle.x))*cos(toRadians(renderer->CameraAngle.y))) * key_vely;
		renderer->CameraPos.y -= (float) (sin(toRadians(renderer->CameraAngle.x))*cos(toRadians(renderer->CameraAngle.y))) * key_vely;
		renderer->CameraPos.z -= (float) (sin(toRadians(renderer->CameraAngle.y))) * key_vely;

		renderer->CameraPos.x += (float) (cos(toRadians(renderer->CameraAngle.x-90.f))*1) * key_velx;
		renderer->CameraPos.y += (float) (sin(toRadians(renderer->CameraAngle.x-90.f))*1) * key_velx;
	}
	if (local_state == 2)
	{
		player_cl *pl = get_own_player();
		if (pl == NULL) {log(LOG_ERROR, "Could not retreive own player!"); return;}

		// give input to player
		if (input != pl->input)
		{
			// update for this player and all others
			pl->input = input;
			net_send_input_keys(pl->id, input, serverpeer);
		}


		// stick camera to player
		pl->ro->visible = false;

		renderer->CameraPos.x = pl->position.x;
		renderer->CameraPos.y = pl->position.y;
		renderer->CameraPos.z = pl->position.z + CAMERA_VIEW_HEIGHT + cam_bob_offset;
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
				net_send_request_join(serverpeer);
			}
			if (local_state == 2)
			{
				// shoot
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
