#ifndef __COLLECTIBLE_H__
#define __COLLECTIBLE_H__

#include "actor.h"

#define COLLECTIBLE_TYPE_WP_COLT 1
#define COLLECTIBLE_TYPE_WP_CHAINSAW 2
#define COLLECTIBLE_TYPE_WP_WESSON 3
#define COLLECTIBLE_TYPE_WP_HKSL8 4
#define COLLECTIBLE_TYPE_WP_SHOTGUN 5
#define COLLECTIBLE_TYPE_WP_USAS12 6

#define COLLECTIBLE_TYPE_HEALTH 20

#define COLLECTIBLE_STATE_DEFAULT 1
#define COLLECTIBLE_STATE_PARACHUTE 2

class collectible : public actor
{
public:
	char collectible_type;

	collectible(level *, char collectible_type, vec *);
	collectible(level *, uint actor_id, char collectible_type, vec *);

	void movement(double);

protected:
private:
};

#endif
