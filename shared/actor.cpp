#include "actor.h"
#include <cmath>
#include <algorithm>
#include "defs.h"
#include "stdio.h"


using namespace std;

actor::actor(level* lvl, uint type, vec *pos, vec *pan)
{
    // register in actorlist
    id = lvl->actorlist.add(this);

	printf("freely created actor (type %u) at id=%u \n", type, id);

    this->lvl = lvl;
    this->type = type;

    if (pos != NULL) move(pos); else position.zero();
    if (pan != NULL) turn(pan); else angle.zero();

    move_force = 0;
	move_speed = 1; // default move speed
	state = 0;
}

actor::actor(level* lvl, uint actor_id, uint type, vec *pos, vec *pan)
{
	printf("creating actor (type = %i) with id=%i fixed \n", type, actor_id);

    // register in actorlist at fixed position
    id = lvl->actorlist.add_at(this, actor_id);

    this->lvl = lvl;
    this->type = type;

    if (pos != NULL) move(pos); else position.zero();
    if (pan != NULL) turn(pan); else angle.zero();
    
	move_force = 0;
	move_speed = 1; // default move speed
	state = 0;
}

actor::~actor()
{
    log(LOG_DEBUG_VERBOSE, "actor: deleting from serverlist");
    // remove me from actorlist
    lvl->actorlist.remove(id);
}

/////////////////////////////////////////////////////////
// stats

void actor::change_health(float change)
{
    health += change;
    health = min(health, max_health);
    health = max(health, 0.f);
}

/////////////////////////////////////////////////////////
// movement

void actor::move(vec *newpos)
{
    position.set(newpos);
}

void actor::move_rel(vec *force)
{
    position.x += force->x * (float)cos(angle.x*M_PI/180.);
    position.y += force->x * (float)sin(angle.x*M_PI/180.);
}

void actor::turn(vec *newpan)
{
    angle.set(newpan);
}

void actor::turn_to(vec *pos, double time_delta)
{
    float ang_pan = 90.0f - vec::angle(atan2(pos->x - position.x, pos->y - position.y))*(float)(180.0/M_PI);

    angle.x += vec::angle(vec::angle(ang_pan) - vec::angle(angle.x))*(float)(0.5*time_delta);

    // old code
	/*float n = pos->y - position.y;
	if (n == 0) n += 0.0001;
    angle.x = atan((pos->x - position.x)/(n));
	while (angle.x >= 360) angle.x -= 360;
	while (angle.x < 0) angle.x += 360;*/
}


/////////////////////////////////////////////////////////
// statemanager
void actor::set_state(uint new_state)
{
    // WARNING: this code is overwritten on server and client!

	state = new_state;
}

void actor::state_manager(double time_delta)
{
	// WARNING: this code may be overwritten on server and client!

}

// FRAME
void actor::frame(double time_delta)
{
	// WARNING: this code is overwritten on server and client!
}
