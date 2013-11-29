#include "include/gameServer.h"

using namespace std;

#include "SDL/SDL.h"

#include "include/Timer.h"

#include <mysql++.h>

#include <pthread.h>

#define _USE_MATH_DEFINES
#include <cmath>

#include <ctime>

#include "helper.h"

#include "include/net_sv.h"
#include "include/player_sv.h"
#include "include/level_sv.h"


// base85 functions from base95.cpp
void encode_85(char *buf, const unsigned char *data, int bytes);
int decode_85(char *dst, const char *buffer, int len);




////////////////////////////////////
// initialize/deinitialize  needed subsystems
bool gameServer::init()
{

    // initialize enet
    if (enet_initialize() != 0)
    {
        fprintf (stderr, "An error occurred while initializing ENet.\n");
        return EXIT_FAILURE;
    }

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
    // close enet
    enet_deinitialize();

    //Quit SDL
    SDL_Quit();
}






///////////////////////////////////////////
// Constructor

gameServer::gameServer(bool networked) // todo: server settings
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

    //Initialize
    if( init() == false )
    {
        log(LOG_ERROR, "Failed to initialize subsystems! Exitting..");
        exit(EXIT_FAILURE);
    }

    //printf("Size of short: %lu, Size of int: %lu, Size of float: %lu, sizeof double: %lu, size of long: %lu \n", sizeof(short), sizeof(int), sizeof(float), sizeof(double), sizeof(long));


    //////////////////////////////////////////////////////////
    // load level

    lvl_sv = new level_sv();
    lvl = lvl_sv;


    ///////////////////////////////////////////////////////////
    // Initialize Enet server
    ENetAddress address;
    /* Bind the server to the default localhost.     */
    /* A specific host address can be specified by   */
    /* enet_address_set_host (& address, "x.x.x.x"); */
    address.host = ENET_HOST_ANY;
    /* Bind the server to port 1234. */
    address.port = (*it)->result[0]["port"];

    Ehost = enet_host_create (&address /* the address to bind the server host to */,
                                 8      /* allow up to x clients and/or outgoing connections */,
                                  1      /* allow up to 1 channel to be used, 0 */,
                                  0      /* assume any amount of incoming bandwidth */,
                                  0      /* assume any amount of outgoing bandwidth */);
    if (Ehost == NULL)
    {
        fprintf (stderr,
                 "An error occurred while trying to create an ENet gameServer host.\n");
        exit (EXIT_FAILURE);
    }
    printf("ENet gameServer created!\n");


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

        while (enet_host_service (Ehost, & event, 0) > 0)
            handle_netevent(&event);


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
            //cout << "Average Frames Per Second: " << 1000.f / fps.get_ticks() << endl;

            //Restart the update timer
            update.start();
        }

        time_delta = ((double)frametime.get_ticks()) / (1000./16.);
    }

    // destroy enet_server
    enet_host_destroy(Ehost);

    //Clean up
    clean_up();


}





//////////////////////////////////////
// network data

void gameServer::synchronizeClient(ENetPeer *receiver)
{
    // send server info
    net_send_sync_server(lvl->filename, receiver);

    // send the client to be synchronized all actors in the level

    for (uint i = 0; i < lvl->actorlist.size; i++)
    {
        if (lvl->actorlist.elem[i] != 0)
        {
            /*if (lvl->actorlist.elem[i]->type == ACTOR_TYPE_PLAYER)
            {
                player *pl = (player*)lvl->actorlist.elem[i];
                net_send_sync_player(i, pl->class_id, pl->name, receiver);
            }*/
        }
    }
    
    net_send_sync_finish(receiver);
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
            ((s_peer_data*)event->peer->data)->game_state = 0;
            ((s_peer_data*)event->peer->data)->player_id = -1;
            ((s_peer_data*)event->peer->data)->player_name[0] = '\0';


            // send my version information
            net_send_version(DEF_VERSION, event->peer);

            break;

        case ENET_EVENT_TYPE_RECEIVE:

            printf ("A packet of length %u, event %d, was received on channel %u.\n",
                    event->packet -> dataLength,
                    *((short*) event->packet->data),
                    event->channelID);


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

                        if (pd->game_state > 0 && strnlen(pd->player_name, 32) > 0)
                        {
                            std::string s(pd->player_name);
                            s.append(": ");
                            s.append(data);

                            log(LOG_DEBUG, s.c_str());

                            // resend to everyone
                            net_broadcast_chat(s.c_str(), s.length()+1, Ehost);
                            
                        } else log (LOG_ERROR, "NET_CHAT: received message from unsynced player!");

                        break;
                    }
                    
                    case NET_SYNC_PLAYER:
                    {
                    	s_net_sync_player *ps = (s_net_sync_player *)data;
                    	
                    	// get player data
                    	s_peer_data *pd = (s_peer_data *)event->peer->data;
                    	
                    	// save player name in it
                    	ps->name[31] = '\0';
                    	strncpy(pd->player_name, ps->name, 32);
                    	
                    	// start syncing player
                    	synchronizeClient(event->peer);
                    	
                    	pd->game_state = 1;
                    	
                    	break;
                    }

                    /////////////////////////////////////////
                    // TODO: player input


            }

            /* Clean up the packet now that we're done using it. */
            enet_packet_destroy (event->packet);


            break;

        case ENET_EVENT_TYPE_DISCONNECT:
            printf ("%s disconnected.\n", (char*)(event->peer -> data));


            // get player (TODO: external function, pl sanity check) data
            s_peer_data *pd = (s_peer_data *)event->peer->data;

            if (pd->game_state == 2) // player created
            {

                // delete player
                player_sv * pl = (player_sv *)lvl->actorlist.elem[pd->player_id];


                if (pl != NULL)
                {
                    // sending LEAVE message
                    std::string s(pd->player_name);
                    s.append(" has left this region.");
                    net_broadcast_chat(s.c_str(), s.length()+1, Ehost);

                    // remove player
                    delete(pl); // will also remove player from actorlist

                }
            }

            /* Reset the peer's client information. */
            delete(pd);
            event->peer -> data = NULL;

            break;
        }

}






/////////////////////////////////////////////////////
// mysql stuff

/*void gameServer::handle_query(mysqlRequest* req)
{

                log (LOG_DEBUG, "Client successfully logged in, Syncronizing");

                // sychronize actors with client
                synchronizeClient(req->peer);


                    vec vpos;

                    log (LOG_DEBUG, "Creating Player");

                    player_sv * pl = new player_sv(zone_id, gateway_id, lvl_sv, &zonelist, Ehost, &vpos, &gt->ang,
                                               req->result[0]["account_id"], // account id
                                               req->peer,               // owner peer
                                               req->result[0]["class"], // class_id
                                               req->result[0]["username"].c_str(), // name
                                               req->result[0]["level"], // level
                                               req->result[0]["health"], // health
                                               req->result[0]["mana"], // mana
                                               req->result[0]["strength"], // strength
                                               req->result[0]["agility"], // agility
                                               req->result[0]["intelligence"], // intelligence
                                               req->result[0]["points_remain"], // skill points to distribute
                                               req->result[0]["xp"], // xp points
                                               req->result[0]["gold"],
                                               req->result[0]["respawn_timer"],
                                               inv_ch, // inventory in char*
                                               quests_ch, // quests in char*
                                               skills_ch); // skills in char*

                    //  save player in peer data
                    s_peer_data *pd = (s_peer_data *)req->peer->data;

                    pd->player_id = pl->id;
                    pd->game_state = 1;

                    // send additional data thats not transfered to all clients, only to owner
                    net_send_update_gold(pl->id, pl->gold, pl->id, req->peer);
                    net_send_update_xp(pl->id, pl->xp, pl->id, req->peer);
                    net_send_update_points_remain(pl->id, pl->points_remain, req->peer);
                    net_send_update_strength(pl->id, pl->strength, req->peer);
                    net_send_update_agility(pl->id, pl->agility, req->peer);
                    net_send_update_intelligence(pl->id, pl->intelligence, req->peer);

                    // send loaded inventory
                    log (LOG_DEBUG, "Sending inventory");

                    net_send_it_full(pl->id, pl->inv, req->peer);

                    pl->recalc_equip();


                    // send loaded questlog
                    log (LOG_DEBUG, "Sending questlog");

                    net_send_qe_full(pl->id, pl->quests, req->peer);



                    // Sending skills and skillbar
                    log (LOG_DEBUG, "Sending skills and skillbar");


                    net_send_sk_full(pl->id, pl->skills, req->peer);
                    net_send_skbar_full(pl->id, skillbar_ch, req->peer);

                    log (LOG_DEBUG, "Giving controll to player");


                    // send finish with own actor id
                    net_send_sync_finish(pl->id, req->peer);


                    // sending ENTER message
                    std::string s(pl->name);
                    s.append(" has entered this region.");
                    net_broadcast_chat(s.c_str(), s.length()+1, Ehost);



                    //////////////////////////////
                    //set last_logout to 0 to avoid several logins at once

                    mysqlpp::Connection *cp = mysqlpool->grab();
                    mysqlpp::Query query(cp->query());

                    query << "UPDATE accounts SET last_logout = " << 0l
                    << " WHERE account_id = " << pl->account_id << " LIMIT 1";
                    // log query
                    log (LOG_DEBUG_VERBOSE, query.str().c_str());

                    mysqlpool->release(cp);
                    // use query_mysql to do query
                    query_mysql(QUERY_SAVE_LAST_LOGIN, query.str(), NULL);

                }
                else
                {
                    log (LOG_ERROR, "ERROR on creating player: did not find player gateway_id in level");

                    if (net_send_event(NET_LOGIN_FAILED, NULL, 0, req->peer) != 0)
                    log (LOG_ERROR, "Failed to send login fail message!");
                }

                delete[] inv_ch;
                delete[] quests_ch;
                delete[] skills_ch;
                delete[] skillbar_ch;

            }
            else
            {
                log (LOG_ERROR, "Client failed to log in. Player was already logged in");

                // TODO: send different message
                if (net_send_event(NET_LOGIN_FAILED, NULL, 0, req->peer) != 0)
                log (LOG_ERROR, "Failed to send login fail message!");
            }


        }
        else
        {
            log (LOG_DEBUG, "Client failed to log in. Wrong password/username");
            // fail!

            // TODO: send why there was a fail
            if (net_send_event(NET_LOGIN_FAILED, NULL, 0, req->peer) != 0)
                log (LOG_ERROR, "Failed to send login fail message!");
        }

        break;

*/

///////////////////////////////7
// log data to console
void gameServer::log(int importance, const char *msg)
{
    puts(msg);
}
