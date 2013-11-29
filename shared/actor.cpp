#include "actor.h"
#define _USE_MATH_DEFINES
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
	this->inv = NULL;
	this->buffl = new bufflist();

    switch (type)
    {
        case ACTOR_TYPE_NPC:
            this->faction = 2; break;

        case ACTOR_TYPE_PLAYER:
            this->faction = 1; break;

        default:
            this->faction = 0; break;

    }

    move(pos);
    turn(pan);

    // register my current tile
    current_tile = lvl->world_to_tile(&position);

    move_force = 0;
	move_speed = 1; // default move speed
	state = ACTOR_STATE_IDLE;
	timer_attack = 0;
	timer_attack_cooldown = 0;
}

actor::actor(level* lvl, uint actor_id, uint type)
{
	printf("creating actor (type = %i) at id=%i fixed \n", type, actor_id);

    // register in actorlist at fixed position
    id = lvl->actorlist.add_at(this, actor_id);

    this->lvl = lvl;
    this->type = type;
	this->inv = NULL;
	this->buffl = new bufflist();


    switch (type)
    {
        case ACTOR_TYPE_NPC:
            this->faction = 2; break;

        case ACTOR_TYPE_PLAYER:
            this->faction = 1; break;

        default:
            this->faction = 0; break;

    }

    // set everything to default
	position.zero();
	angle.zero();

	// Can not register current tile here..

	move_force = 0;
	move_speed = 1; // default move speed
	state = ACTOR_STATE_IDLE;
    timer_attack = 0;
	timer_attack_cooldown = 0;

}

actor::~actor()
{
	if (this->inv != NULL) delete this->inv;
	if (this->buffl != NULL) delete this->buffl;

    log(LOG_DEBUG_VERBOSE, "actor: deleting from serverlist");
    // remove me from actorlist
    lvl->actorlist.remove(id);
}

/////////////////////////////////////////////////////////
// stats

inline
float actor::get_damage()
{
    return calc_min_damage() + (calc_max_damage() - calc_min_damage()) * (float)((rand() % 100)*0.01f);
}

inline
float actor::calc_max_damage()
{
    return att_dmg_min;
}

inline
float actor::calc_min_damage()
{
    return att_dmg_max;
}

inline
float actor::calc_defence()
{
    return defence;
}

inline
float actor::calc_move_speed()
{
    return move_speed;
}

inline
float actor::calc_att_range()
{
    return att_range;
}

inline
float actor::calc_att_speed()
{
    return att_speed;
}

inline
float actor::calc_att_cooldown()
{
    return att_cooldown;
}

inline
float actor::calc_max_health()
{
    return max_health;
}

inline
float actor::calc_max_mana()
{
    return max_mana;
}

void actor::change_health(float change)
{
    health += change;
    health = min(health, max_health);
    health = max(health, 0.f);
}

void actor::change_mana(float change)
{
    mana += change;
    mana = min(mana, max_mana);
    mana = max(mana, 0.f);
}

bool actor::do_damage(uint target_actor, float damage)
{
    log(LOG_ERROR, "ERROR in actor::do_damage. Should never be called because virtual!");
    return false;
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

    //if (state == ACTOR_STATE_IDLE)


    if (state == ACTOR_STATE_WALKING)
    {
        // transfer target tile to world coords
        vec t(0, 0, 0);
        lvl->tile_to_world(target, &t);

        // turn towards target
        turn_to(&t, time_delta);

        // accelerate

        move_force += 1.0f*(float)time_delta;
        move_force = min(move_force, move_speed);
    }


    // no movement
	if (state != ACTOR_STATE_WALKING) //&& my.state != 3)) // deaccelerate
	{
		move_force -= 2.5f*(float)time_delta;
		move_force = max(0.f, move_force);
	}

    // play attack animation
	if (state == ACTOR_STATE_ATTACKING)
	{
		// turn to target
		if (lvl->actorlist.elem[target] != NULL) turn_to(&lvl->actorlist.elem[target]->position, time_delta);

	    // TODO: maybe create timer only on server? ->dublicate code, leave it for now
        // timer is only checked for progress/reset on server
	    timer_attack += att_speed*time_delta;
	}
	else
	{
        // let timer run
        if (timer_attack_cooldown > 0) timer_attack_cooldown -= time_delta;
	}

	if (state == ACTOR_STATE_ATTACK_PAUSE)
	{
		// turn to target
		if (lvl->actorlist.elem[target] != NULL) turn_to(&lvl->actorlist.elem[target]->position, time_delta);
	}


    // move forward with move force
    vec v(move_force*(float)time_delta, 0, 0);

    move_rel(&v);

}

// FRAME
void actor::frame(double time_delta)
{
    // mainly a dummy
}
