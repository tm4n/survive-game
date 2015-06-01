#include "gameClient.h"

#include "net_cl.h"
#include "helper.h"
#include "level_cl.h"
#include "player_cl.h"
#include "box_cl.h"
#include "collectible_cl.h"
#include "npc_cl.h"
#include "backends/b_settings.h"
#include <sstream>

gameClient::gameClient(gameRenderer *arenderer)
{
	renderer = arenderer;
	local_state = 0;
	respawn_timer = -1.f;

	points = 0;
	input = 0;
	input_enable = true;
	cam_bob_offset = 0.f;

	effmgr = new effectmgr(renderer);

	hud = NULL;
}

gameClient::~gameClient()
{
	if (hud != NULL) delete hud;
	if (lvl_cl != NULL) delete lvl_cl;
	delete effmgr;
	if (net_client != NULL) delete net_client;
}



void gameClient::handle_netevent(ENetEvent *event)
{
	switch (event->type)
    {
		case ENET_EVENT_TYPE_NONE: break;
    	
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
				uint32_t net_type = *((uint32_t*) event->packet->data);
				char *data = (char*)(event->packet -> data) + sizeof(uint32_t);
				
				/*std::ostringstream ss;
				ss << "Client received packet " << net_type << std::endl;
				log(LOG_DEBUG, ss.str().c_str());*/

				////////////////////////////
				// differentiate packet type
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
							hud->show_status_wrongver();
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
							new player_cl(lvl, d->actor_id, &d->pos, &d->ang, d->health, (char*)d->name, d->weapon, d->input, d->object_taken, renderer, effmgr);
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
							new box_cl(lvl, d->actor_id, d->box_type, &d->pos, d->health, renderer, hud);
						}
						else log(LOG_ERROR, "Received NET_SYNC_BOX without level");

						break;
					}
					
					case NET_SYNC_COLLECTIBLE:
					{
						s_net_sync_collectible *d = (s_net_sync_collectible*)data;

						if (lvl != NULL)
						{
							// create a box at given position
							new collectible_cl(lvl, d->actor_id, d->collectible_type, &d->pos, renderer);

							if (d->collectible_type < 10) hud->show_message("A new weapon has been dropped!");
						}
						else log(LOG_ERROR, "Received NET_SYNC_COLLECTIBLE without level");

						break;
					}

					case NET_SYNC_NPC:
					{
						s_net_sync_npc *d = (s_net_sync_npc*)data;

						if (lvl != NULL)
						{
							// create a box at given position
							new npc_cl(lvl, d->actor_id, d->npc_type, &d->pos, &d->angle, d->health, d->target, renderer);
						}
						else log(LOG_ERROR, "Received NET_SYNC_NPC without level");

						break;
					}

					case NET_SYNC_FINISH:
					{
						// mark client as joinable
						local_state = 1;

						hud->show_status_join(false);

						break;
					}

					case NET_REMOVE_ACTOR:
					{
						s_net_remove_actor *d = (s_net_remove_actor*)data;

						if (lvl != NULL)
						{
							// get actor at position
							actor *ac = lvl->actorlist.at(d->actor_id);

							// own player deleted
							if (d->actor_id == own_actor_id)
							{
								if (local_state == 2)
								{
									local_state = 1;
									respawn_timer = RESPAWN_TIME;
								}
							}

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

							hud->hide_status();

							player_cl *pl = get_own_player();
							if (pl != NULL) pl->local_player = true;
								else log(LOG_ERROR, "FATAL! Invalid actor_id on NET_JOIN!");

							// display help
							if (points == 0) hud->show_message("Use the crates to build a defence. Move them with right-click.");

						}
						else log (LOG_ERROR, "Received NET_JOIN while not spectating");

						break;
					}
					
					case NET_GAME_STATE:
					{
						s_net_game_state *d = (s_net_game_state*)data;
						
						state = d->state;

						if (state == GAME_STATE_END)
						{
							local_state = 3;
							hud->show_status_end();
						}
						
						break;
					}
					
					case NET_GAME_WAVE:
					{
						s_net_game_wave *d = (s_net_game_wave*)data;
						
						wave = d->game_wave;
						
						break;
					}
					
					case NET_WAVE_WAIT_TIMER:
					{
						s_net_wave_wait_timer *d = (s_net_wave_wait_timer*)data;
						
						wave_wait_timer = d->wave_wait_timer;

						if (wave_wait_timer > 0) hud->show_wave_timer(wave_wait_timer);
						else hud->hide_wave_timer();
						
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
									if (!(input & INPUT_SPRINT)) pl->wpmgr->show_wp();
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
									pl->wpmgr->cancel_reload();
									pl->wpmgr->hide_wp();
									box->taker_id = d->actor_id;
									box->state = BOX_STATE_TAKEN;
								}
								else log(LOG_ERROR, "Received NET_TAKE with invalid box");
							}
						}
						else log(LOG_ERROR, "Received NET_TAKE invalid player");
						
						break;
					}

					case NET_UPDATE_POS:
					{
						s_net_update_pos *d = (s_net_update_pos *)data;

						actor *ac = lvl_cl->actorlist.at(d->actor_id);
						if (ac != NULL)
						{
							ac->position.set(&d->pos);
						}
						else log(LOG_ERROR, "Received NET_UPDATE_POS for invalid actor");

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
						else log(LOG_ERROR, "Received NET_UPDATE_ANG for non-player or invalid actor");
						
						break;
					}
					
					case NET_UPDATE_HEALTH:
					{
						// get send data
						s_net_update_health *d = (s_net_update_health *)data;
						
						actor * ac = lvl_cl->actorlist.at(d->actor_id);
						if (ac != NULL)
						{
							if (local_state == 2 && ac->type == ACTOR_TYPE_PLAYER && ac->id == own_actor_id)
							{
								//printf("setting health for own player %i \n", d->actor_id);
								if (ac->health > d->health) effmgr->eff_pl_flash(1);
								if (ac->health < d->health) effmgr->eff_pl_flash(2);
							}

							//printf("setting health for %i \n", d->actor_id);
							ac->health = d->health;

							
						}
						else log(LOG_ERROR, "Received NET_UPDATE_HEALTH for invalid actor");
						
						break;
					}

					case NET_UPDATE_TARGET:
					{
						// get send data
						s_net_update_target *d = (s_net_update_target *)data;
						
						npc_cl *np= lvl_cl->get_npc(d->actor_id);
						if (np != NULL)
						{
							//printf("setting target for %i \n", d->actor_id);
							np->target = d->target;
						}
						else log(LOG_ERROR, "Received NET_UPDATE_TARGET for non-npc or invalid actor");
						
						break;
					}
					
					case NET_UPDATE_AMMO_MAGAZIN:
					{
						s_net_update_ammo_magazin *d = (s_net_update_ammo_magazin *)data;

						player_cl *pl= lvl_cl->get_player(d->actor_id);
						if (pl != NULL)
						{
							if (!(pl->wpmgr->pickups & (1 << d->weapon_id))) hud->show_message("You've picked up a new weapon! Change weapons with mouse wheel or number keys.");

							pl->wpmgr->set_mag_ammo(d->weapon_id, (short)d->ammo_magazin, d->ammo_magazin >> 16);
						}
						else log(LOG_ERROR, "Received NET_UPDATE_AMMO_MAGAZIN for non-player or invalid actor");
						
						break;
					}

					case NET_SHOOT:
					{
						s_net_shoot *d = (s_net_shoot *)data;

						player_cl *pl= lvl_cl->get_player(d->actor_id);
						if (pl != NULL)
						{
							vec shoot_origin;
							shoot_origin.x = pl->position.x;
							shoot_origin.y = pl->position.y;
							shoot_origin.z = pl->position.z+pl->bb_max.z-CAMERA_VIEW_HEIGHT;
							pl->wpmgr->shoot(shoot_origin, d->shoot_dir, d->rnd_seed);
						}
						else log(LOG_ERROR, "Received NET_UPDATE_AMMO_MAGAZIN for non-player or invalid actor");
						
						break;
					}

					case NET_CHANGE_WEAPON:
					{
						s_net_update_curr_weapon *d = (s_net_update_curr_weapon*)data;

						// get player
     
						player_cl *pl= lvl_cl->get_player(d->actor_id);
						if (pl != NULL)
						{
							// start teh change
							log(LOG_ERROR, "Received NET_CHANGE_WEAPON, starting change animation");
							pl->wpmgr->switch_cl(d->new_weapon_id);
						}
						else log(LOG_ERROR, "Received NET_CHANGE_WEAPON for non-player actor");

						break;
					}

					case NET_UPDATE_CURR_WEAPON:
					{
						s_net_update_curr_weapon *d = (s_net_update_curr_weapon*)data;

						// get player
     
						player_cl *pl= lvl_cl->get_player(d->actor_id);
						if (pl != NULL)
						{
							pl->wpmgr->update_curr_weapon(d->new_weapon_id);
						}
						else log(LOG_ERROR, "Received NET_CHANGE_WEAPON for non-player actor");

						break;
					}

					case NET_UPDATE_SCORE:
					{
						s_net_update_score *d = (s_net_update_score*)data;

						// get player
     
						points = d->score;

						break;
					}

					case NET_RELOAD:
					{
						s_net_reload *d = (s_net_reload*)data;

						// get player
     
						player_cl *pl= lvl_cl->get_player(d->actor_id);
						if (pl != NULL)
						{
							//pl->wpmgr->reload();
							// is done locally now
						}
						else log(LOG_ERROR, "Received NET_CHANGE_WEAPON for non-player actor");

						break;
					}

					case NET_SCOREBOARD:
					{
						hud->update_scoreboard(data);

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
			
			/*std::ostringstream ss;
			ss << "Client done with packet";
			log(LOG_DEBUG, ss.str().c_str());*/

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
	// deferred creation
	if (hud == NULL) hud = new gui_hud(renderer->gui, &renderer->resources, &quit);
	
	// Handle packages
	ENetEvent event;
	while (net_client->host_service (&event, 0) > 0)
			handle_netevent(& event);

	// move spectator camera
	if (local_state == 1)
	{
		hud->set_state(gui_hud::hud_state::spectating);
		
		float key_vely = ((input & INPUT_BACK ? 1 : 0) - (input & INPUT_FORW ? 1 : 0)) * 25.f * (float)time_delta;
		float key_velx = ((input & INPUT_RIGHT ? 1 : 0) - (input & INPUT_LEFT ? 1 : 0)) * 20.f * (float)time_delta;
		if (input & INPUT_SPRINT) {key_vely *= 3.f; key_velx *= 3.f;}

		renderer->CameraPos.x -= (float) (cos(toRadians(renderer->CameraAngle.x))*cos(toRadians(renderer->CameraAngle.y))) * key_vely;
		renderer->CameraPos.y -= (float) (sin(toRadians(renderer->CameraAngle.x))*cos(toRadians(renderer->CameraAngle.y))) * key_vely;
		renderer->CameraPos.z -= (float) (sin(toRadians(renderer->CameraAngle.y))) * key_vely;

		renderer->CameraPos.x += (float) (cos(toRadians(renderer->CameraAngle.x-90.f))) * key_velx;
		renderer->CameraPos.y += (float) (sin(toRadians(renderer->CameraAngle.x-90.f))) * key_velx;
		
		if (respawn_timer >= 0.f)
		{

			respawn_timer -= (float)(time_delta/16.);
			if (respawn_timer < 0.f) {respawn_timer = 0.f; hud->show_status_join(true);}
			else hud->show_status_respawn_timer(((int)respawn_timer)+1);

		}

		hud->frame(time_delta, 0, 0, 0, wave, points);
	}
	if (local_state == 2)
	{
		player_cl *pl = get_own_player();
		if (pl != NULL)
		{
		
			hud->set_state(gui_hud::hud_state::playing);

			// only when player is alive
			if (pl->health > 0.f)
			{
				// bob camera
				if ((input & INPUT_FORW || input & INPUT_BACK || input & INPUT_LEFT || input & INPUT_RIGHT) && !(input & INPUT_JUMP))
				{
					cam_bob_prog += pl->move_speed*6.2831853f*(float)time_delta;
					cam_bob_offset = sin(toRadians(cam_bob_prog))*2.f;
				}
				else
				{
					cam_bob_prog = 0;
					if (abs(cam_bob_offset) < CAMERA_BOB_STOP_RATE*time_delta) cam_bob_offset = 0;
					else
					{
						if (cam_bob_offset > 0) {cam_bob_offset -= CAMERA_BOB_STOP_RATE*(float)time_delta;}
						else {cam_bob_offset += CAMERA_BOB_STOP_RATE*(float)time_delta;}
					}
				}

				// give input to player
				if (input != pl->input)
				{
					// update for this player and all others
					pl->input = input;
					net_client->send_input_keys(pl->id, input, net_client->serverpeer);
				}
			}


			// stick camera to player
			pl->ro->visible = false;

			renderer->CameraPos.x = pl->position.x;
			renderer->CameraPos.y = pl->position.y;
			renderer->CameraPos.z = pl->position.z + pl->bb_max.z - CAMERA_VIEW_HEIGHT + cam_bob_offset;

			std::ostringstream s;

			//s << "Player "<<pl->position << ", tilt=" << renderer->CameraAngle[1] << ", curr_weapon=" << pl->curr_weapon << ", wp->anim_state=" << pl->wpmgr->anim_state;

			s << "DEBUG wp_ready: " << pl->wpmgr->wp_ready << ", wp_reloading: " << pl->wpmgr->wp_reloading << "wp->anim_state=" << pl->wpmgr->anim_state;

			hud->set_debug(s.str());
		
			hud->frame(time_delta, pl->health, pl->wpmgr->get_curr_ammo(), pl->wpmgr->get_curr_magazin(), wave, points);

			pl->wpmgr->frame(time_delta);
		}
	}
	if (local_state == 3)
	{
		// game end
		hud->set_state(gui_hud::hud_state::game_end);
	}

	effmgr->frame(time_delta);

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
	if (input_enable == false) return;

	b_settings *set = b_settings::instance();
	player_cl *pl = get_own_player();

	// special case: pipe to ingame menu
	if (hud != NULL && hud->ingame_menu_visible)
	{
		if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_ESCAPE) hud->toggle_ingame_menu();
		if (evt->type == SDL_JOYBUTTONDOWN && evt->jbutton.button == 1) hud->toggle_ingame_menu();
		renderer->gui->event_mouse(evt);
		return;
	}
	
	if (evt->type == SDL_MOUSEMOTION)
	{
		float mouse_fac = 0.025f*set->mouse_sensitivity;
		renderer->CameraAngle.x -= (float)evt->motion.xrel*mouse_fac;
		if (set->mouse_invert) mouse_fac *= -1.f;
		renderer->CameraAngle.y -= (float)evt->motion.yrel*mouse_fac;

		renderer->CameraAngle.y = clamp(renderer->CameraAngle.y, -89.f, 89.f);
	}
	if (evt->type == SDL_JOYAXISMOTION)
	{
		if (evt->jaxis.axis == 0)
		{
			if (evt->jaxis.value > 10000) input |= INPUT_RIGHT; else input &= ~INPUT_RIGHT;
			if (evt->jaxis.value < -10000) input |= INPUT_LEFT; else input &= ~INPUT_LEFT;
		}
		if (evt->jaxis.axis == 1)
		{
			if (evt->jaxis.value > 10000) input |= INPUT_BACK; else input &= ~INPUT_BACK;
			if (evt->jaxis.value < -10000) input |= INPUT_FORW; else input &= ~INPUT_FORW;
		}
		if (evt->jaxis.axis == 2)
		{
			renderer->CameraJoyInputX = evt->jaxis.value;
		}
		if (evt->jaxis.axis == 3)
		{
			renderer->CameraJoyInputY = evt->jaxis.value;
		}
		if (evt->jaxis.axis == 5) // shoot
		{
			if (evt->jaxis.value > 3200)
			{
				if (local_state == 1)
				{
					// send request to join
					net_client->send_request_join(net_client->serverpeer);
				}
				if (local_state == 2)
				{
					// shoot
					pl->input_shoot = true;
				}
			}
			else
			{
				if (local_state == 2)
				{
					// shoot
					pl->input_shoot = false;
				}
			}
		}
		if (evt->jaxis.axis == 4) // sprint
		{
			if (evt->jaxis.value > 3200)
			{
				input |= INPUT_SPRINT;
				if (local_state == 2)
				{
					// cancel reload
					pl->wpmgr->cancel_reload();
					pl->wpmgr->hide_wp();
				}
			}
			else
			{
				input &= ~INPUT_SPRINT;
				if (local_state == 2 && pl->object_taken == -1) pl->wpmgr->show_wp();
			}
		}

		renderer->CameraAngle.y = clamp(renderer->CameraAngle.y, -89.f, 89.f);
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
				// shoot
				pl->input_shoot = true;
			}
		}
		if (evt->button.button == SDL_BUTTON_RIGHT)
		{
			if (local_state == 2)
			{
				// take
				if (pl != NULL) pl->order_take_object();
				
			}
		}
	}
	if (evt->type == SDL_MOUSEBUTTONUP)
	{
		if (evt->button.button == SDL_BUTTON_LEFT)
		{
			if (local_state == 2) pl->input_shoot = false;
		}
	}
	if (evt->type == SDL_MOUSEWHEEL)
	{
		if (evt->wheel.y > 0 && local_state == 2) pl->wpmgr->input_scroll_up();
		if (evt->wheel.y < 0 && local_state == 2) pl->wpmgr->input_scroll_down();
	}
	if (evt->type == SDL_JOYBUTTONDOWN)
	{
		if (evt->jbutton.button == 0)
		{
			if (local_state == 2)
			{
				// take
				if (pl != NULL) pl->order_take_object();
				
			}
		}
		if (evt->jbutton.button == 1) hud->toggle_ingame_menu();
		if (evt->jbutton.button == 2 && local_state == 2) pl->wpmgr->input_scroll_down();
		if (evt->jbutton.button == 3 && local_state == 2) pl->wpmgr->input_scroll_up();
		if (evt->jbutton.button == 9) input |= INPUT_JUMP;
		if (evt->jbutton.button == 10)
		{
			if (local_state == 2 && !(input & INPUT_SPRINT) && pl->object_taken == -1) pl->wpmgr->input_reload();
		}
			
		if (evt->jbutton.button == 11) input |= INPUT_FORW;
		if (evt->jbutton.button == 12) input |= INPUT_BACK;
		if (evt->jbutton.button == 13) input |= INPUT_LEFT;
		if (evt->jbutton.button == 14) input |= INPUT_RIGHT;
		std::ostringstream ss;
		ss << "button " << (int)evt->jbutton.button << " pressed";
		log(LOG_DEBUG, ss.str().c_str());
	}
	if (evt->type == SDL_JOYBUTTONUP)
	{
		if (evt->jbutton.button == 9) input &= ~INPUT_JUMP;
		if (evt->jbutton.button == 11) input &= ~INPUT_FORW;
		if (evt->jbutton.button == 12) input &= ~INPUT_BACK;
		if (evt->jbutton.button == 13) input &= ~INPUT_LEFT;
		if (evt->jbutton.button == 14) input &= ~INPUT_RIGHT;
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
			if (local_state == 2)
			{
				// cancel reload
				pl->wpmgr->cancel_reload();
				pl->wpmgr->hide_wp();
			}
			break;
		case SDLK_SPACE:
			input |= INPUT_JUMP;
			break;

		case SDLK_r:
			if (local_state == 2 && !(input & INPUT_SPRINT) && pl->object_taken == -1) pl->wpmgr->input_reload();
			break;

		case SDLK_1:
			if (local_state == 2) pl->wpmgr->input_switch(1);
			break;
		case SDLK_2:
			if (local_state == 2) pl->wpmgr->input_switch(2);
			break;
		case SDLK_3:
			if (local_state == 2) pl->wpmgr->input_switch(3);
			break;
		case SDLK_4:
			if (local_state == 2) pl->wpmgr->input_switch(4);
			break;
		case SDLK_5:
			if (local_state == 2) pl->wpmgr->input_switch(5);
			break;
		case SDLK_6:
			if (local_state == 2) pl->wpmgr->input_switch(6);
			break;

		case SDLK_TAB:
			if (!net_client->local_only) hud->show_scoreboard();
			break;

		case SDLK_ESCAPE:
			hud->toggle_ingame_menu();
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
				if (local_state == 2 && pl->object_taken == -1) pl->wpmgr->show_wp();
				break;
			case SDLK_SPACE:
				input &= ~INPUT_JUMP;
				break;

			case SDLK_TAB:
				if (!net_client->local_only) hud->hide_scoreboard();
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
