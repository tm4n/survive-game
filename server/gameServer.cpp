#include "gameServer.h"

using namespace std;

#include "SDL2/SDL.h"

#include "Timer.h"

#define _USE_MATH_DEFINES
#include <cmath>

#include <ctime>
#include <iostream>
#include <sstream>

#include "helper.h"

#include "net_sv.h"
#include "level_sv.h"
#include "player_sv.h"
#include "collectible_sv.h"
#include "scoremgr.h"



gameServer::gameServer()
{
	// create networked game server
	net_server = new net_sv();
	
	//Initialize
    if( init() == false )
    {
        log(LOG_ERROR, "Failed to initialize subsystems! Exitting..");
        exit(EXIT_FAILURE);
    }
    
    // TODO: set settings from the outside
    sv_num_npcs_limit = 30;
	sv_barrier_probability = 5.f;
}

gameServer::gameServer(std::list<ENetPacket*> *in_queue, std::mutex *mutex_in_queue, std::list<ENetPacket*> *out_queue, std::mutex *mutex_out_queue)
{
	// create local game server
	net_server = new net_sv(in_queue, mutex_in_queue, out_queue, mutex_out_queue);
	
	//Initialize
    if( init() == false )
    {
        log(LOG_ERROR, "Failed to initialize subsystems! Exitting..");
        exit(EXIT_FAILURE);
    }
    
    // TODO: set settings from the outside
    sv_num_npcs_limit = 30;
	sv_barrier_probability = 5.f;
}



////////////////////////////////////
// initialize/deinitialize  needed subsystems
bool gameServer::init()
{

    //Initialize the SDL Timer subsystem
    if( SDL_Init( SDL_INIT_TIMER ) == -1 )
    {
        return false;
    }

    std::string s;
    get_workdir(&s);

    printf("Workdir: %s \n", s.c_str());



    //If everything initialized fine
    return true;
}


void gameServer::clean_up()
{
    // close net
    enet_deinitialize();
}



void gameServer::run() 
{
    quit = false;

    //reset frame count
    frame = 0;

    //Timer used to calculate the frames per second
    Timer fps;

    //Timer used to update the caption
    Timer update;

    // Timer used to calculate time_delta (frame time)
    Timer frametime;


    std::ostringstream s;

    s << "Starting Survive Gameserver";

    log(LOG_DEBUG, s.str().c_str());

    //printf("Size of short: %lu, Size of int: %lu, Size of float: %lu, sizeof double: %lu, size of long: %lu \n", sizeof(short), sizeof(int), sizeof(float), sizeof(double), sizeof(long));


    ///////////////////////////////////////////////////////////
    // Initialize Enet server
    ENetAddress address;
    /* Bind the server to the default localhost.     */
    /* A specific host address can be specified by   */
    /* enet_address_set_host (& address, "x.x.x.x"); */
    address.host = ENET_HOST_ANY;
    /* Bind the server to port 1234. */
    address.port = 1201;

    net_server->host_create (&address /* the address to bind the server host to */,
                                 8      /* allow up to x clients and/or outgoing connections */,
                                  1      /* allow up to 1 channel to be used, 0 */,
                                  0      /* assume any amount of incoming bandwidth */,
                                  0      /* assume any amount of outgoing bandwidth */);
    if (net_server->eHost == NULL)
    {
        fprintf (stderr,
                 "An error occurred while trying to create an network gameServer host.\n");
        exit (EXIT_FAILURE);
    }
    log(LOG_DEBUG, "Network gameServer created!\n");
    
    
	//////////////////////////////////////////////////////////
    // load level

    lvl_sv = new level_sv("desert");
    lvl = lvl_sv;


    //Start the update timer
    update.start();

    double time_delta = 0.;

    ///////////////////////////////////////////////////////////
    //While the user hasn't quit the server
    while( quit == false )
    {
        //Start the frame timer
        fps.start();
        frametime.start();

        /////////////////////////////
        // While there's events from enet to handle
        ENetEvent event;

        while (net_server->host_service (&event, 0) > 0)
            handle_netevent(&event);
            
            
            
		//////////////////////////////////////////////////
		// Game logic
		if (state == GAME_STATE_WAITING)
		{
			reset();
			// check if a player wants to start (is done with event currently!)
		}
		if (state == GAME_STATE_RUNNING)
		{			
			// spawn npcs and boxes
			spawner(time_delta);
		
			// check if the generator has been destroyed
			if (lvl_sv->get_box(lvl_sv->get_generator()) == NULL) state = GAME_STATE_END;
			
			// check if there are still players connected!
			if (net_server->num_connected_clients() <= 0) state = GAME_STATE_END;
			
			
			// check if we are counting down a wave
			if (wave_wait_timer > 0 && state == GAME_STATE_RUNNING)
			{
				// wait one second!
				wave_wait_tick += ((float)time_delta) / 16.f;
				if (wave_wait_tick > 1.0f)
				{
					wave_wait_tick -= 1.0f;
					
					wave_wait_timer -= 1;
					net_server->broadcast_wave_wait_timer(wave_wait_timer);
					
					if (wave_wait_timer == 0)
					{
						// advance to next wave! 
						
						sv_spawned_npcs = 0;
						sv_spawn_timer = 0.f;
						sv_barrier_timer = 0.f;
						
						wave += 1;
						
						// set number of npcs to spawn
						sv_amount_npcs = 5 + (int)(log((double)(wave+1))*(5.f + get_num_players()*3.f));
						
						// set number of barriers to spawn
						//sv_barrier_probability -= sv_barrier_probability/4; //don't lower anymore in current versions
						
						if (wave > 15) sv_wave_bonus += 0.1f;
						
						// update the players
						net_server->broadcast_game_wave(wave);
						
						std::cout << "Wave " << wave << " started" << std::endl;
					}
				}
			}
			
		}
		if (state == GAME_STATE_END)
		{
			// Timeout, then switch to GAME_STATE_WAITING
			
			// only do once:
		}


        //////////////////////////////////////////////////
        // Then CALLBACK to entity actions

        for (uint i = 0; i < lvl->actorlist.size; i++)
        {
            if (lvl->actorlist.elem[i] != 0) lvl->actorlist.elem[i]->frame(time_delta);
        }



        //Increment the frame counter
        frame++;

        // cap the frame rate
        if( fps.get_ticks() < 1000 / FRAMES_PER_SECOND )
        {
            //Sleep the remaining frame time
            SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - fps.get_ticks() );
        }

        //If 10 seconds have passed since the output was last printed
        if( update.get_ticks() > 10000 )
        {

            //Calculate the frames per second and create the string
            std::cout << "Server average Frames Per Second: " << 1000.f / fps.get_ticks() << endl;

            //Restart the update timer
            update.start();
        }

        time_delta = ((double)frametime.get_ticks()) / (1000./16.);
    }

    // destroy enet_server
    net_server->host_destroy();

    //Clean up
    clean_up();


}


//////////////////////////////////////
// destructor
gameServer::~gameServer()
{
	
}

int gameServer::get_num_players()
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
				if (pd->clstate == 2) ct++; // if player is in game
			}
		}
	}
	return ct;
}

//////////////////////////////////////
// network data

void gameServer::synchronizeClient(ENetPeer *receiver)
{
    // send server info
    net_server->send_sync_server(lvl->filename, receiver);

    // send the client to be synchronized all actors in the level
    for (uint i = 0; i < lvl->actorlist.size; i++)
    {
        if (lvl->actorlist.elem[i] != 0)
        {
            if (lvl->actorlist.elem[i]->type == ACTOR_TYPE_PLAYER)
            {
                player *pl = (player*)lvl->actorlist.elem[i];
                net_server->send_sync_player(i, &pl->position, &pl->angle, pl->health, pl->name, pl->curr_weapon, pl->input, pl->object_taken, receiver);
            }
            if (lvl->actorlist.elem[i]->type == ACTOR_TYPE_BOX)
            {
                box_sv *bo = (box_sv*)lvl->actorlist.elem[i];
				net_server->send_sync_box(i, bo->box_type, &bo->position, bo->health, receiver);
            }
			if (lvl->actorlist.elem[i]->type == ACTOR_TYPE_COLLECTIBLE)
            {
                collectible_sv *co = (collectible_sv*)lvl->actorlist.elem[i];
				net_server->send_sync_collectible(i, co->collectible_type, &co->position, receiver);
            }
			if (lvl->actorlist.elem[i]->type == ACTOR_TYPE_NPC)
			{
				npc_sv *np = (npc_sv*)lvl->actorlist.elem[i];
				net_server->broadcast_sync_npc(i, np->npc_type, &np->position, &np->angle, np->health, np->target);
			}
        }
    }
    
    net_server->send_sync_finish(receiver);
}

//////////////////////////////////////////
// handle network events

void gameServer::handle_netevent(ENetEvent *event)
{
    switch (event->type)
    {
        case ENET_EVENT_TYPE_NONE:
            break;

        case ENET_EVENT_TYPE_CONNECT:
            printf ("A new client connected from %x:%u.\n",
                    event->peer -> address.host,
                    event->peer -> address.port);

            /* Store any relevant client information here. */
            event->peer->data = new s_peer_data;
            ((s_peer_data*)event->peer->data)->clstate = 0;
            ((s_peer_data*)event->peer->data)->player_actor_id = -1;
            ((s_peer_data*)event->peer->data)->player_name = NULL;
			((s_peer_data*)event->peer->data)->score = 0;


            // send my version information
            net_server->send_version(DEF_VERSION, event->peer);

            break;

        case ENET_EVENT_TYPE_RECEIVE:
		{
            /*printf ("A packet of length %lu, event %d, was received on channel %u.\n",
                    event->packet -> dataLength,
                    *((short*) event->packet->data),
                    event->channelID); */


            // check minimal packet size
            if (event->packet->dataLength >= 2)
            {

                // extract packet data
                short net_type = *((short*) event->packet->data);
                char *data = (char*)(event->packet -> data) +2;

                ////////////////////////////
                // differentiate net type
                //
                switch (net_type)
                {
                    case NET_CHAT:
                    {
                        // add player name
                        s_peer_data *pd = (s_peer_data *)event->peer->data;

                        if (pd->clstate > 0 && pd->player_name != NULL)
                        {
                            std::string s(pd->player_name);
                            s.append(": ");
                            s.append(data);

                            log(LOG_DEBUG, s.c_str());

                            // resend to everyone
                            net_server->broadcast_chat(s.c_str(), s.length()+1);
                            
                        } else log (LOG_ERROR, "NET_CHAT: received message from unsynced player!");

                        break;
                    }
                    
                    case NET_SYNC_CLIENT:
                    {
                    	// char *data contains name as string
                    	// get player data
                    	s_peer_data *pd = (s_peer_data *)event->peer->data;
                    	
                    	size_t length = strlen(data);
                    	
                    	if (length <= PLAYERNAME_LENGTH)
						{
							// safe player
							pd->player_name = new char[length+1];
							strncpy(pd->player_name, data, length+1);
							
							// start syncing player
							synchronizeClient(event->peer);
							
							pd->clstate = 1;
						}
						else
						{
							// TODO: malicious client!
							log(LOG_ERROR, "Too long player name received!");
						}
                    	

                    	
                    	break;
                    }
                    
                    case NET_REQUEST_JOIN:
					{
						// get player data
                    	s_peer_data *pd = (s_peer_data *)event->peer->data;
						
						
						// check player
						if (state != GAME_STATE_RUNNING && pd->clstate == 1) // && respawn timer
						{
							if (state == GAME_STATE_WAITING) start_match();
							
							// create player
							vec pos(0.f, 0.f, 0.f);
							vec ang(0.f, 0.f, 0.f);
							player_sv * pl = new player_sv(lvl, &pos, &ang, 100.f, pd->player_name, event->peer);
							
							pd->player_actor_id = pl->id;
							
							// send update
							net_server->send_join(pl->id, event->peer);
							
							pd->clstate = 2;
						}
						else log(LOG_WARNING, "NET_REQUEST_JOIN not granted");
						
						break;
					}
					
                    case NET_INPUT_KEYS:
					{
						// get send data
						s_net_input_keys *d = (s_net_input_keys *)data;
						// get player data
                    	s_peer_data *pd = (s_peer_data *)event->peer->data;
						
						if (d->actor_id == pd->player_actor_id)
						{
							player_sv *pl= lvl_sv->get_player(d->actor_id);
							if (pl != NULL)
							{
								pl->input = d->input;
							}
							else log(LOG_ERROR, "Received NET_INPUT_KEYS for non-player actor");
							
						}
						else log(LOG_ERROR, "Received NET_INPUT_KEYS for actor thats not owned by this client");
						
						break;
					}
					
					case NET_TAKE:
					{
						s_net_take *d = (s_net_take*)data;
						// get player data
                    	s_peer_data *pd = (s_peer_data *)event->peer->data;
						
						player_sv *pl = lvl_sv->get_player(d->actor_id);
						
						if (d->actor_id == pd->player_actor_id && pl != NULL)
						{
							if (d->taken_id < 0)
							{
								box_sv *box = lvl_sv->get_box((uint)pl->object_taken);
								if (box != NULL)
								{
									pl->object_taken = -1;
									box->taker_id = -1;
									box->state = BOX_STATE_DEFAULT;
									
									net_server->broadcast_take(d->actor_id, d->taken_id);
									net_server->broadcast_update_pos(box->id, &box->position);
								}
								else
								{
									pl->object_taken = -1;
									log(LOG_ERROR, "Received NET_TAKE, and player had invalid object in hand");
								}
							}
							else
							{
								box_sv *box = lvl_sv->get_box((uint)d->taken_id);
								if (box != NULL)
								{
									if (box->position.dist(&pl->position) < 2000000.f && pl->object_taken < 0)
									{
										pl->object_taken = d->taken_id;
										pl->wpmgr->cancel_reload();
										box->taker_id = d->actor_id;
										box->state = BOX_STATE_TAKEN;
										
										net_server->broadcast_take(d->actor_id, d->taken_id);
									}
									else log(LOG_ERROR, "Received NET_TAKE but box was too far away or player already had one");
								}
								else log(LOG_ERROR, "Received NET_TAKE with invalid box");
							}
						}
						else log(LOG_ERROR, "Received NET_TAKE for actor thats not owned by this client");
						
						break;
					}
					
					case NET_UPDATE_ANG:
					{
						// get send data
						s_net_update_ang *d = (s_net_update_ang *)data;
						// get player data
                    	s_peer_data *pd = (s_peer_data *)event->peer->data;
						
						if (d->actor_id == pd->player_actor_id)
						{
							player_sv *pl= lvl_sv->get_player(d->actor_id);
							if (pl != NULL)
							{
								pl->angle.x = d->ang;
								pl->ang_interp_dir = d->ang_interp_dir;
								
								// send update to other players, every other time
								pl->ang_count++;
								if (pl->ang_count >= 2)
								{
									net_server->broadcast_update_ang_except(d->actor_id, d->ang, d->ang_interp_dir, event->peer);
									pl->ang_count = 0;
								}
							}
							else log(LOG_ERROR, "Received NET_UPDATE_ANG for non-player actor");
							
						}
						else log(LOG_ERROR, "Received NET_UPDATE_ANG for actor thats not owned by this client");
						
						break;
					}
					
					case NET_UPDATE_POS:
					{
						// get send data
						s_net_update_pos *d = (s_net_update_pos *)data;
						// get player data
                    	s_peer_data *pd = (s_peer_data *)event->peer->data;
						
						if (d->actor_id == pd->player_actor_id)
						{
							player_sv *pl= lvl_sv->get_player(d->actor_id);
							if (pl != NULL)
							{
								// check how much the difference already is
								if (pl->position.dist(&d->pos) > 60.f)
								{
									// force update to client
									net_server->send_update_pos(pl->id, &pl->position, pl->owner);
								}
								else
								{
									// take this as correct position
									pl->position.set(&d->pos);
								}
							}
							else log(LOG_ERROR, "Received NET_UPDATE_POS for non-player actor");
							
						}
						else log(LOG_ERROR, "Received NET_UPDATE_POS for actor thats not owned by this client");
						
						break;
					}
                    
					case NET_SHOOT:
					{
						// get send data
						s_net_shoot *d = (s_net_shoot *)data;
						// get player data
                    	s_peer_data *pd = (s_peer_data *)event->peer->data;

						if (d->actor_id == pd->player_actor_id)
						{
							player_sv *pl= lvl_sv->get_player(d->actor_id);
							if (pl != NULL)
							{
								vec shoot_origin;
								shoot_origin.x = pl->position.x;
								shoot_origin.y = pl->position.y;
								shoot_origin.z = pl->position.z+pl->bb_max.z-CAMERA_VIEW_HEIGHT;
								pl->wpmgr->shoot(shoot_origin, d->shoot_dir);
							}
							else log(LOG_ERROR, "Received NET_SHOOT for non-player actor");
							
						}
						else log(LOG_ERROR, "Received NET_SHOOT for actor thats not owned by this client");

						break;
					}

					case NET_CHANGE_WEAPON:
					{
						s_net_update_curr_weapon *d = (s_net_update_curr_weapon*)data;
						// get player data
                    	s_peer_data *pd = (s_peer_data *)event->peer->data;

						if (d->actor_id == pd->player_actor_id)
						{
							player_sv *pl= lvl_sv->get_player(d->actor_id);
							if (pl != NULL)
							{
								// start teh change
								pl->wpmgr->wp_switch_impl(d->new_weapon_id);
							}
							else log(LOG_ERROR, "Received NET_CHANGE_WEAPON for non-player actor");
							
						}
						else log(LOG_ERROR, "Received NET_CHANGE_WEAPON for actor thats not owned by this client");

						break;
					}

					case NET_RELOAD:
					{
						s_net_reload *d = (s_net_reload*)data;
						// get player data
                    	s_peer_data *pd = (s_peer_data *)event->peer->data;

						if (d->actor_id == pd->player_actor_id)
						{
							player_sv *pl= lvl_sv->get_player(d->actor_id);
							if (pl != NULL)
							{
								// start reload
								pl->wpmgr->wp_reload_impl();
							}
							else log(LOG_ERROR, "Received NET_RELOAD for non-player actor");
							
						}
						else log(LOG_ERROR, "Received NET_RELOAD for actor thats not owned by this client");

						break;
					}

					case NET_SCOREBOARD:
					{
						// get player data
                    	s_peer_data *pd = (s_peer_data *)event->peer->data;

						std::string scores;

						scoremgr::get_full_string(&scores);

						net_server->send_scoreboard(scores, event->peer);

						break;
					}

					default:
						log(LOG_ERROR, "Packed with unkown/invalid type received");
                    
                }


            }

            /* Clean up the packet now that we're done using it. */
            enet_packet_destroy (event->packet);


            break;
		}

        case ENET_EVENT_TYPE_DISCONNECT:
            printf ("%s disconnected.\n", (char*)(event->peer -> data));


            // get player (TODO: external function, pl sanity check) data
            s_peer_data *pd = (s_peer_data *)event->peer->data;

            if (pd->clstate == 2) // player created
            {

                // delete player
                //player_sv * pl = (player_sv *)lvl->actorlist.elem[pd->player_id];


                if (1 != 0)
                {
                    // sending LEAVE message
                    std::string s(pd->player_name);
                    s.append(" has left the game.");
                    net_server->broadcast_chat(s.c_str(), s.length()+1);

                    // remove player
                    //delete(pl); // will also remove player from actorlist

                }
            }

            /* Reset the peer's client information. */
            delete(pd);
            event->peer -> data = NULL;

            break;
        }

}


void gameServer::start_match()
{
	// reset values
	reset();
	
	// reset level (lat0r, maybe done on exit?  
	
	// add level starters
	vec v, t;
	// TODO: create generator
	v.set(0, 0, lvl->level_ground);
	new box_sv(lvl_sv, BOX_TYPE_GENERATOR, &v, &sv_num_barriers);
	
	// create starting crates
	v.set(180, 180, lvl->border_ground);
	new box_sv(lvl_sv, BOX_TYPE_WOOD, &v, &sv_num_barriers);
	v.set(130, 180, lvl->border_ground);
	new box_sv(lvl_sv, BOX_TYPE_WOOD, &v, &sv_num_barriers);
	v.set(180, 130, lvl->border_ground);
	new box_sv(lvl_sv, BOX_TYPE_WOOD, &v, &sv_num_barriers);
	
	v.set(-180, 180, lvl->border_ground);
	new box_sv(lvl_sv, BOX_TYPE_WOOD, &v, &sv_num_barriers);
	v.set(-130, 180, lvl->border_ground);
	new box_sv(lvl_sv, BOX_TYPE_WOOD, &v, &sv_num_barriers);
	v.set(-180, 130, lvl->border_ground);
	new box_sv(lvl_sv, BOX_TYPE_WOOD, &v, &sv_num_barriers);
	
	v.set(-180, -180, lvl->border_ground);
	new box_sv(lvl_sv, BOX_TYPE_WOOD, &v, &sv_num_barriers);
	v.set(-130, -180, lvl->border_ground);
	new box_sv(lvl_sv, BOX_TYPE_WOOD, &v, &sv_num_barriers);
	v.set(-180, -130, lvl->border_ground);
	new box_sv(lvl_sv, BOX_TYPE_WOOD, &v, &sv_num_barriers);
	
	v.set(180, -180, lvl->border_ground);
	new box_sv(lvl_sv, BOX_TYPE_WOOD, &v, &sv_num_barriers);
	v.set(130, -180, lvl->border_ground);
	new box_sv(lvl_sv, BOX_TYPE_WOOD, &v, &sv_num_barriers);
	v.set(180, -130, lvl->border_ground);
	new box_sv(lvl_sv, BOX_TYPE_WOOD, &v, &sv_num_barriers);

	v.set(400, 0, lvl->level_ground);
	new npc_sv(lvl_sv, NPC_MUMMY, &v, &t, &sv_num_npcs);

	v.set(400, 300, 0.f);
	new npc_sv(lvl_sv, NPC_HARPY, &v, &t, &sv_num_npcs);
    
    state = GAME_STATE_RUNNING;
    net_server->broadcast_game_state(state);
    
    // start countdown to first
    next_wave();
}

void gameServer::reset()
{
	wave = 0;
	wave_wait_timer = 0;
	wave_wait_tick = 0.f;
	
	sv_spawned_npcs = 0;  // number of spawned npcs this round
	sv_amount_npcs = 0;  // number of npcs to spawn this round
	sv_num_npcs = 0;  // number of npcs currently alive

	sv_num_barriers = 0;

	sv_spawn_timer = 0.f;
	sv_spawn_cap = 0.f;
	sv_barrier_timer = 0.f;
	
	sv_wave_bonus = 0.f;
}

void gameServer::spawner(double time_frame)
{
	int spawn_type = 0;
	int dice;
	
	// don't do any enemy if we are currently waiting for a new wave
	if (wave_wait_timer <= 0)
	{
		
		// check if we are ready a new wave, don't wait for all enemies to die
		if (sv_spawned_npcs >= sv_amount_npcs && sv_num_npcs < sv_num_npcs_limit/4 && wave_wait_timer == 0)
		{
			next_wave();
		}
		
		// spawn npcs
		sv_spawn_cap = -0.2f*(float)get_num_players()+1.8f;
		if (wave > 12) sv_spawn_cap -= ((float)wave-12.f)*0.1f;
		sv_spawn_cap = std::max(sv_spawn_cap, 0.1f);
		
		if (sv_spawn_timer >= sv_spawn_cap)
		{
			sv_spawn_timer -= sv_spawn_cap;
			
			if (sv_spawned_npcs < sv_amount_npcs && sv_num_npcs < sv_num_npcs_limit)
			{
				// roll the dice
				dice = (int)random_range(100);

				// decide which to spawn
				
				switch(wave)
				{
					case 1:
						spawn_type = NPC_MUMMY;
					break;
					
					case 2:
						if (dice < 12) spawn_type = NPC_SMASHER; else spawn_type = NPC_MUMMY;
					break;
				
					case 3:
						if (dice < 20) spawn_type = NPC_SMASHER; else spawn_type = NPC_MUMMY;
					break;
					
					
					case 4:
						if (dice < 15) spawn_type = NPC_SMASHER; else
							if (dice > 80) spawn_type = NPC_WEREWOLF;
								else spawn_type = NPC_MUMMY;
					break;
				
					case 5:
						if (dice < 60) spawn_type = NPC_WITCH;
								else spawn_type = NPC_SMASHER;
					break;
				
					case 6:
						if (dice < 40) spawn_type = NPC_WITCH; else
							if (dice > 80) spawn_type = NPC_WEREWOLF;
								else spawn_type = NPC_MUMMY;
					break;
				
					case 7:
						if (dice < 30) spawn_type = NPC_WITCH; else
							if (dice > 70) spawn_type = NPC_WEREWOLF;
								else spawn_type = NPC_MUMMY;
					break;
					
					
					case 8:
						if (dice < 20) spawn_type = NPC_HOGMAN; else
							if (dice > 70) spawn_type = NPC_WITCH;
								else spawn_type = NPC_MUMMY;
					break;
					
					case 9:
						spawn_type = NPC_WEREWOLF;
					break;
					
					case 10:
						if (dice < 50) spawn_type = NPC_HOGMAN; else
							if (dice > 80) spawn_type = NPC_KNIGHT;
								else spawn_type = NPC_MUMMY;
					break;
					
					case 11:
						if (dice < 30) spawn_type = NPC_WITCH; else
							if (dice > 85) spawn_type = NPC_HARPY;
								else spawn_type = NPC_KNIGHT;
					break;
					
					case 12:
						if (dice < 30) spawn_type = NPC_SMASHER; else
							if (dice > 85) spawn_type = NPC_HARPY;
								else spawn_type = NPC_WITCH;
					break;
					
					case 13:
						spawn_type = NPC_HARPY;
					break;
					
					case 14:
						if (dice < 40) spawn_type = NPC_WITCH; else
							if (dice > 90) spawn_type = NPC_HARPY;
								else spawn_type = NPC_KNIGHT;
					break;
					
					case 15:
						if (dice < 30) spawn_type = NPC_WITCH; else
							if (dice > 50) spawn_type = NPC_HARPY;
								else spawn_type = NPC_BAUUL;
					break;
					
					default:
					if (dice < 25) {spawn_type = NPC_WITCH; break;}
					if (dice < 50) {spawn_type = NPC_HOGMAN; break;}
					if (dice < 60) {spawn_type = NPC_SMASHER; break;}
					if (dice < 65) {spawn_type = NPC_HARPY; break;}
					if (dice < 70) {spawn_type = NPC_WEREWOLF; break;}
					spawn_type = NPC_KNIGHT;
					break;
				}
				
				npc_spawn(spawn_type, 1.0f + ((get_num_players()-1.0f)*(1.0f/MAX_PLAYERS)) + sv_wave_bonus);
				sv_spawned_npcs += 1;
			}
		}
		
	}
	
	// TODO: maybe make spawning more random without counter?
	// spawn barriers
	if (sv_barrier_timer >= 0.5)
	{
		sv_barrier_timer -= 0.5;
		
		if (random_range(100) < sv_barrier_probability)
		{
			if (wave >= 2 && lvl_sv->wpdrops[COLLECTIBLE_TYPE_WP_CHAINSAW] == false)
			{
				lvl_sv->wpdrops[COLLECTIBLE_TYPE_WP_CHAINSAW] = true;
				wpdrop_spawn(COLLECTIBLE_TYPE_WP_CHAINSAW);
				return;
			}
			
			if (wave >= 4 && lvl_sv->wpdrops[COLLECTIBLE_TYPE_WP_WESSON] == false)
			{
				lvl_sv->wpdrops[COLLECTIBLE_TYPE_WP_WESSON] = true;
				wpdrop_spawn(COLLECTIBLE_TYPE_WP_WESSON);
				return;
			}
			if (wave >= 7 && lvl_sv->wpdrops[COLLECTIBLE_TYPE_WP_HKSL8] == false)
			{
				lvl_sv->wpdrops[COLLECTIBLE_TYPE_WP_HKSL8] = true;
				wpdrop_spawn(COLLECTIBLE_TYPE_WP_HKSL8);
				return;
			}
			if (wave >= 10 && lvl_sv->wpdrops[COLLECTIBLE_TYPE_WP_SHOTGUN] == false)
			{
				lvl_sv->wpdrops[COLLECTIBLE_TYPE_WP_SHOTGUN] = true;
				wpdrop_spawn(COLLECTIBLE_TYPE_WP_SHOTGUN);
				return;
			}
			if (wave >= 14 && lvl_sv->wpdrops[COLLECTIBLE_TYPE_WP_USAS12] == false)
			{
				lvl_sv->wpdrops[COLLECTIBLE_TYPE_WP_USAS12] = true;
				wpdrop_spawn(COLLECTIBLE_TYPE_WP_USAS12);
				return;
			}
			box_spawn();
		}
	}
	
	sv_spawn_timer += (float)time_frame / 16.f;
	sv_barrier_timer += (float)time_frame / 16.f;
}




/////////////////////////////////////
// Start next wave
void gameServer::next_wave()
{
	log(LOG_DEBUG, "Waiting for next wave.");
	
	// set waiting timer
	//game_wait_timer = 45 + game_wave*10;
	//game_wait_timer = minv(game_wait_timer, 200);
	wave_wait_timer = 5; 	//DEBUG: waves immediately
	
	net_server->broadcast_wave_wait_timer(wave_wait_timer);
}


void gameServer::npc_spawn(int etype, float ebonus)
{
	// debug message:
	/*tr_cpy (str_temp, "Spawning NPC: ");
	str_cat(str_temp, str_for_int(NULL, etype));
	str_cat(str_temp, " with bonus ");
	str_cat(str_temp, str_for_num(NULL, ebonus));
	sv_debug(str_temp);*/
	
	// determine where they should spawn
	float ang = toRadians(random_range(360));
	vec v;
	v.x = cos(ang) * lvl->level_size;
	v.y = sin(ang) * lvl->level_size;
	
	if (b_npcs::instance()->at(etype)->ai_type == NPC_AI_PLAYER_FLYING) v.z = 200.f + random_range(400);
	else v.z = lvl->level_ground;
	
	npc_sv *np = new npc_sv(lvl_sv, etype, &v, NULL, &sv_num_npcs);
	np->health *= ebonus;
}

void gameServer::box_spawn()
{
	vec pos;
	int dice;
	
	log(LOG_DEBUG, "Spawning barrier crate");
		
	// get position anywhere on the level^
	pos.x = random_range(lvl->level_size*2.f) - lvl->level_size;
	pos.y = random_range(lvl->level_size*2.f) - lvl->level_size;
	pos.z = 1500.f;
	
	dice = (int)random_range(75);
	if (dice <= 12)
	{
		if (dice <= 1.5)
			new box_sv(lvl_sv, BOX_TYPE_TURRET, &pos, &sv_num_barriers);
		else new box_sv(lvl_sv, BOX_TYPE_METAL, &pos, &sv_num_barriers);
	}
	else
	{
		new box_sv(lvl_sv, BOX_TYPE_WOOD, &pos, &sv_num_barriers);
	}
}

void gameServer::wpdrop_spawn(int wtype)
{
	vec pos;

	log(LOG_DEBUG, "Spawning weapon crate, Type ");
	
	// get position anywhere on the level^
	pos.x = random_range(lvl->level_size*2.f) - lvl->level_size;
	pos.y = random_range(lvl->level_size*2.f) - lvl->level_size;
	pos.z = 1500.f;
	
	new collectible_sv(lvl_sv, (char)wtype, &pos);
}