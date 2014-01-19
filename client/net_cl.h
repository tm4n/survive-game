#ifndef __NET_CL_H__
#define __NET_CL_H__

#include "net.h"

int net_send_sync_client(const char *name, ENetPeer *receiver);
int net_send_request_join(ENetPeer *receiver);

int net_send_chat(const char *msg, uint len, ENetPeer *receiver);


#endif