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


// base85 functions from base95.cpp
void encode_85(char *buf, const unsigned char *data, int bytes);
int decode_85(char *dst, const char *buffer, int len);


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
    printf("Network gameServer created!\n");
    
    
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
			// check if a player wants to start
		}
		if (state == GAME_STATE_RUNNING)
		{
			// game logic
			//sv_spawner();
		
			// check if the generator has been destroyed
			//if (game_generator == ANET_ERROR) game_state = GAME_STATE_END;
			
			// check if there are still players connected!
			//if (enet_connected_clients() <= 0) game_state = GAME_STATE_END;
			
		}
		if (state == GAME_STATE_END)
		{
			// Timeout, then switch to GAME_STATE_WAITING
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
            std::cout << "Average Frames Per Second: " << 1000.f / fps.get_ticks() << endl;

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


/////////////////////////////////////
// Start next wave
void gameServer::next_wave()
{
	puts("Waiting for next wave.");
	
	// set waiting timer
	//game_wait_timer = 45 + game_wave*10;
	//game_wait_timer = minv(game_wait_timer, 200);
	wait_timer = 5; 	//DEBUG: waves immediately
	
	//enet_sendto(&game_wait_timer, sizeof(int), BROADCAST);
	
	if (wait_timer > 0 && state == GAME_STATE_RUNNING)
	{
		/*wait(-1);
		game_wait_timer -= 1;
		enet_sendto(&game_wait_timer, sizeof(int), BROADCAST);*/
	}
	else
	{
		sv_spawned_npcs = 0;
		sv_spawn_timer = 0;
		sv_barrier_timer = 0;
		
		wave += 1;
		
		// set number of npcs to spawn
		//sv_amount_npcs = 5 + (int)(log(wave+1)*(5 + game_num_players*3));
		
		// set number of barriers to spawn
		//sv_barrier_probability -= sv_barrier_probability/4; don't lower anymore in current versions
		
		//if (wave > 15) sv_wave_bonus += 0.1;
		
		
		// update the players
		//enet_sendto(&game_wave, sizeof(int), BROADCAST);
		
		std::cout << "Wave " << wave << " started" << std::endl;
	}
}

//////////////////////////////////////
// destructor
gameServer::~gameServer()
{
	
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
                net_server->send_sync_player(i, &pl->position, &pl->angle, pl->health, pl->name, pl->weapon, pl->input, pl->object_taken, receiver);
            }
            if (lvl->actorlist.elem[i]->type == ACTOR_TYPE_BOX)
            {
                box_sv *bo = (box_sv*)lvl->actorlist.elem[i];
                net_server->send_sync_box(i, &bo->position, bo->health, receiver);
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
	
	// reset level (lat0r)   
	
	// add level starters
    lvl_sv->spawn_starters();
}
