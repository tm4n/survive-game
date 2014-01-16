#include "gameClient.h"

#include "net_cl.h"
#include "helper.h"


gameClient::gameClient(gameRenderer *arenderer)
{
	renderer = arenderer;

	////////////////////////////////////////////////////
	// Initialize enet host, no connections is done yet
    // Create enet client
	serverpeer = NULL;

    Ehost = enet_host_create (NULL /* create a client host */,
                1 /* only allow 1 outgoing connection */,
                2 /* allow up 2 channels to be used, 0 and 1 */,
                0 /* no downstream limit */,
                0 /* no upstream limit */);

    if (Ehost == NULL)
    {
        puts("An error occurred while trying to create an ENet client host.\n");
        exit (EXIT_FAILURE);
    }

	// todo: move to level

	//display stuff
	RenderObject *objTerrain = new RenderObject();
	objTerrain->scale = glm::vec3(2.0f, 2.0f, 2.0f);
    renderer->resources.getMesh(ResourceLoader::meshType::Terrain)->addRenderObject(objTerrain);
        
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 50; j++) {
        RenderObject *tmp = new RenderObject();
        	
        tmp->translation[1] += i*50.f;
        tmp->translation[2] += j*50.f;
        	
        renderer->resources.getMesh(ResourceLoader::meshType::Crate)->addRenderObject(tmp);
        }
    }
        
    
	soldiers = new RenderObject[30];
    for (int i = 0; i < 30; i++) {       	
        soldiers[i].rotation[0] += i*180.f;
        	
        soldiers[i].translation[1] += i*80.f;
        soldiers[i].translation[0] += -200;
        	
        renderer->resources.getMesh(ResourceLoader::meshType::Soldier)->addRenderObject(&soldiers[i]);
    }

	// create trees at their location
	RenderObject *tmp = new RenderObject();
	tmp->translation = glm::vec3(-328.0f, -1400.0f, -312.0f);
	tmp->rotation[0] = 200.0f;
	renderer->resources.getMesh(ResourceLoader::meshType::Tree)->addRenderObject(tmp);
	tmp = new RenderObject();
	tmp->translation = glm::vec3(800.0f, 1296.0f, -300.0f);
	tmp->rotation[0] = 36.0f;
	renderer->resources.getMesh(ResourceLoader::meshType::Tree)->addRenderObject(tmp);
	tmp = new RenderObject();
	tmp->rotation[0] = 6.0f;
	tmp->translation = glm::vec3(-1440.0f, -784.0f, -300.0f);
	renderer->resources.getMesh(ResourceLoader::meshType::Tree)->addRenderObject(tmp);
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
			printf ("A packet of length %u, event %d, was received from %s on channel %u.\n",
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
    serverpeer = enet_host_connect (Ehost, &address, 1, 0);

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
	while (enet_host_service (Ehost, &event, 0) > 0)
			handle_netevent(& event);
			
			
	// animation test
	for (int i = 0; i < 30; i++) {
			
		if (soldiers[i].animProgress > 1.0f) {
			soldiers[i].animProgress -= 1.0f;
			soldiers[i].animFrame = soldiers[i].animNextFrame;
			soldiers[i].animNextFrame += 1;
			soldiers[i].animNextFrame %= 65;
		}
		soldiers[i].animProgress += 0.1f;
			
	}
}
